/***********************************************************************
 * Copyright 2005  Max Howell <max.howell@methylblue.com>
 *           2007  Christoph Pfister <christophpfister@gmail.com>
 *           2007  Ian Monroe <ian@monroe.nu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************/

#include <config.h>


#include "videoWindow.h"
#include "timeLabel.h"

#include "actions.h"        //::seek() FIXME unfortunate
#include "theStream.h"

#include <QActionGroup>
#include <QContextMenuEvent>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QPainter>

#include <KApplication>
#include <KDebug>
#include <KIcon>
#include <KLocale>
#include <KMenu>
#include <KMimeType>
#include <KStandardDirs>

#include <Phonon/AudioOutput>
#include <phonon/audiodataoutput.h>
#include <Phonon/MediaController>
#include <Phonon/MediaObject>
#include <Phonon/MediaSource>
#include <Phonon/Path>
#include <Phonon/SeekSlider>
#include <Phonon/VideoWidget>
#include <Phonon/VolumeFaderEffect>
#include <Phonon/VolumeSlider>

#include <Solid/Block>
#include <Solid/Device>
#include <Solid/OpticalDisc>

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

using Phonon::AudioOutput;
using Phonon::MediaObject;
using Phonon::VideoWidget;
using Phonon::SeekSlider;
using Phonon::VolumeSlider;
using Phonon::MediaController;

namespace Dragon {


VideoWindow *VideoWindow::s_instance = 0;

VideoWindow::VideoWindow( QWidget *parent )
        : QWidget( parent )
        , m_cursorTimer( new QTimer( this ) )
        , m_justLoaded( false )
        , m_adjustedSize( false)
        , m_subLanguages( new QActionGroup( this ) )
        , m_audioLanguages( new QActionGroup( this ) )
        , m_logo( new QLabel( this ) )
        , m_initialOffset( 0 )
        , m_aDataOutput(0)
{

    m_isPreview = false;

    s_instance = this;
    setObjectName( QLatin1String( "VideoWindow" ) );

    QVBoxLayout *box = new QVBoxLayout( this );
    box->setMargin(0);
    box->setSpacing(0);
    m_vWidget = new VideoWidget( this );
    m_vWidget->hide();
    box->addWidget( m_vWidget );
    m_aOutput = new AudioOutput( Phonon::VideoCategory, this );
    m_media = new MediaObject( this );
    m_controller = new MediaController( m_media );
    Phonon::createPath(m_media, m_vWidget);
    m_audioPath = Phonon::createPath(m_media, m_aOutput);
    m_media->setTickInterval( 1000 );
    connect( m_media, SIGNAL(tick(qint64)), this, SIGNAL(tick(qint64)) );
    connect( m_media, SIGNAL(currentSourceChanged(Phonon::MediaSource)), this, SIGNAL(currentSourceChanged(Phonon::MediaSource)) );
    connect( m_media, SIGNAL(totalTimeChanged(qint64)), this, SIGNAL(totalTimeChanged(qint64)) );
    connect( m_media, SIGNAL(seekableChanged(bool)), this, SIGNAL(seekableChanged(bool)) );
    connect( m_media, SIGNAL(metaDataChanged()), this, SIGNAL(metaDataChanged()) );
    connect( m_aOutput, SIGNAL(mutedChanged(bool)), this, SIGNAL(mutedChanged(bool)) );
    connect( m_aOutput, SIGNAL(volumeChanged(qreal)), this, SIGNAL(volumeChanged(qreal)));
    connect( m_media, SIGNAL(hasVideoChanged(bool)), this, SIGNAL(hasVideoChanged(bool)) );
    connect( m_media, SIGNAL(hasVideoChanged(bool)), m_vWidget, SLOT(setVisible(bool)) ); //hide video widget if no video to show
    connect( m_media, SIGNAL(hasVideoChanged(bool)), m_logo, SLOT(setHidden(bool)) );

    connect( m_controller, SIGNAL(availableSubtitlesChanged()), this, SLOT(updateChannels()) );

    {
        m_subLanguages->setExclusive( true );
        QAction* turnOff = new QAction( i18n("&DVD Subtitle Selection"), m_subLanguages );
        turnOff->setCheckable( true );
        turnOff->setProperty( TheStream::CHANNEL_PROPERTY, -1 );
        connect( turnOff, SIGNAL(triggered()), this, SLOT(slotSetSubtitle()) );

        QAction* separator = new QAction( m_subLanguages );
        separator->setSeparator( true );
    }
    {
        m_audioLanguages->setExclusive( true );
        QAction* autoLang = new QAction( i18n("&Auto"), m_audioLanguages );
        autoLang->setProperty( TheStream::CHANNEL_PROPERTY, -1 );
        autoLang->setCheckable( true );
        connect( autoLang, SIGNAL(triggered()), this, SLOT(slotSetAudio()) );

        QAction* separator = new QAction( m_audioLanguages );
        separator->setSeparator( true );
    }

    connect( m_media, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(stateChanged(Phonon::State,Phonon::State)) );
    connect( m_cursorTimer, SIGNAL(timeout()), this, SLOT(hideCursor()) );
    m_cursorTimer->setSingleShot( true );
    {
        m_logo->setAutoFillBackground( true );
        QPalette pal;
        pal.setColor( QPalette::Window, Qt::white );
        m_logo->setPalette( pal );
        QLayout* layout = new QVBoxLayout( m_logo );
        layout->setAlignment( Qt::AlignCenter );
        m_logo->setLayout( layout );
        box->addWidget( m_logo );
        m_logo->show();
    }
    {
        KConfigGroup config = KGlobal::config()->group( "General" );
        m_aOutput->setVolume( config.readEntry<double>( "Volume", 1.0 ) );
    }
}

VideoWindow::~VideoWindow()
{
    eject();
    KConfigGroup config = KGlobal::config()->group( "General" );
    config.writeEntry( "Volume", static_cast<double>( m_aOutput->volume() ) );

    if( m_media->state() == Phonon::PlayingState )
    {
        Phonon::VolumeFaderEffect* faderEffect = new Phonon::VolumeFaderEffect( this );
        m_audioPath.insertEffect( faderEffect );
        faderEffect->setFadeCurve( Phonon::VolumeFaderEffect::Fade12Decibel );
        faderEffect->fadeOut( 500 );
#ifndef Q_WS_WIN
        ::usleep( 700000 );
#else
        ::Sleep( 700 );
#endif
    }
    else
        m_media->stop(); //hangs if its destroyed while paused?
}

bool
VideoWindow::init()
{
    return m_media->state() != Phonon::ErrorState;
}

bool
VideoWindow::load( const KUrl &url )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );

    eject();

    KMimeType::Ptr mimeType = KMimeType::findByUrl( url );
    kDebug() << "detected mimetype: " << mimeType->name();
    if( mimeType->is( QLatin1String( "application/x-cd-image" ) ) || mimeType->is( QLatin1String( "inode/directory" ) ) )
        m_media->setCurrentSource( Phonon::MediaSource( Phonon::Dvd, url.path() ) );
    else
        m_media->setCurrentSource( url );
    m_justLoaded = true;
    m_adjustedSize=false;

    QApplication::restoreOverrideCursor();

    return true;
}

bool
VideoWindow::play( qint64 offset )
{

    QApplication::setOverrideCursor( Qt::WaitCursor );

    m_justLoaded = false;
    m_initialOffset = offset;
    m_media->play();

    QApplication::restoreOverrideCursor();

    return true;
}

bool
VideoWindow::resume()
{
    m_media->play();
    return true;
}

bool
VideoWindow::playDvd()
{
    eject();
    m_media->setCurrentSource( Phonon::MediaSource( Phonon::Dvd ) );
    m_media->play();
    return true;
}

bool
VideoWindow::playDisc(const Solid::Device& device )
{
    QString devicePath;
    {
        const Solid::Block* block = device.as<const Solid::Block>();
        if( block )
            devicePath = block->device();
        else
        {
            kDebug() << "device was not a block";
            return false;
        }
    }
    const Solid::OpticalDisc* disc = device.as<const Solid::OpticalDisc>();
    if( disc )
    {
        Phonon::DiscType phononType = Phonon::NoDisc;
        {
            Solid::OpticalDisc::ContentTypes solidType = disc->availableContent();
            if (solidType & Solid::OpticalDisc::VideoDvd)
                phononType = Phonon::Dvd;
            if (solidType & (Solid::OpticalDisc::VideoCd | Solid::OpticalDisc::SuperVideoCd))
                phononType = Phonon::Vcd;
            if (solidType & Solid::OpticalDisc::Audio)
                phononType = Phonon::Cd;

            // No change -> cannot play the disc.... should not really happen as
            // mainWindow already preprocesses the type, so this would indicate
            // bogus handling in one of the classes -> assertation.
            Q_ASSERT(phononType != Phonon::NoDisc);
            if (phononType == Phonon::NoDisc){
                kDebug() << "not a playable disc type: " << disc->availableContent() << " type";
                return false;
            }
        }
        eject();
        m_media->setCurrentSource( Phonon::MediaSource( phononType, devicePath ) );
        kDebug() << "actually playing the disc at " << devicePath;
        m_media->play();
        return true;
    }
    else
    {
        kDebug() << "device was not a disc";
        return false;
    }
}

bool
VideoWindow::isPreview(const bool &v)
{
   if( v )
     {
       m_isPreview = v;
     }
   return m_isPreview;
}

void
VideoWindow::relativeSeek( qint64 step )
{
    kDebug() << "** relative seek";
    const qint64 new_pos = currentTime() + step;
    if( ( new_pos >= 0 ) && ( new_pos < length() ) )
    {
        seek( new_pos );
        play();
    }
    else if( new_pos < 0 )
    {
        seek( 0 );
        play();
    }
}

void
VideoWindow::stop()
{
    kDebug() << "Stop called";
    eject();
    m_media->stop();
    m_media->setCurrentSource(Phonon::MediaSource()); //set the current source to    Phonon::MediaSource::Empty
    kDebug() << "Media source valid? "<<  TheStream::hasMedia();
      m_vWidget->hide();
    m_logo->show();
}

void
VideoWindow::pause()
{
    m_media->pause();
}

void
VideoWindow::playPause()
{
  if(m_media->state() == Phonon::PlayingState)
    pause();
  else
    resume();
}

QString
VideoWindow::urlOrDisc() const
{
    Phonon::MediaSource source = m_media->currentSource();
    switch( source.type() )
    {
        case Phonon::MediaSource::Invalid:
        case Phonon::MediaSource::Empty:
            return QLatin1String( "Invalid" ); //no i18n, used for DBus responses
            break;
        case Phonon::MediaSource::Url:
        case Phonon::MediaSource::LocalFile:
            return source.url().toString();
            break;
        case Phonon::MediaSource::Disc:
            return source.deviceName();
            break;
        case Phonon::MediaSource::Stream:
            return QLatin1String( "Data Stream" );
            break;
        default:
            break;
    }
    return QLatin1String( "Error" );
}

Phonon::MediaSource::Type
VideoWindow::mediaSourceType() const
{
    return m_media->currentSource().type();
}

QMultiMap<QString, QString>
VideoWindow::metaData() const
{
    return m_media->metaData();
}

bool
VideoWindow::isSeekable() const
{
    return m_media->isSeekable();
}


Phonon::State
VideoWindow::state() const
{
    return m_media->state();
}

qreal
VideoWindow::volume() const
{
    return m_aOutput->volume();
}

void
VideoWindow::setVolume( qreal vol )
{
    m_aOutput->setVolume( vol );
}

void
VideoWindow::mute(bool muted)
{
    m_aOutput->setMuted( muted );
}

bool
VideoWindow::isMuted()
{
    return m_aOutput->isMuted();
}

void
VideoWindow::seek( qint64 pos )
{
      m_media->seek( pos );
}

qint32
VideoWindow::tickInterval() const
{
    return m_media->tickInterval();
}

void
VideoWindow::showOSD( const QString &/*message*/ )
{
    return;
}

void
VideoWindow::resetZoom()
{
    TheStream::profile().deleteEntry( "Preferred Size" );
    window()->adjustSize();
}

qint64
VideoWindow::currentTime() const
{
   return m_media->currentTime();
}

qint64
VideoWindow::length() const
{
    return m_media->totalTime();
}

bool
VideoWindow::setupAnalyzer(QObject* analyzer)
{
    if(!m_aDataOutput)
    {
        m_aDataOutput = new Phonon::AudioDataOutput(this);
        m_audioDataPath = Phonon::createPath(m_media, m_aDataOutput);
        connect(m_aDataOutput, SIGNAL(dataReady(QMap<Phonon::AudioDataOutput::Channel,QVector<qint16> >)),
                analyzer,  SLOT(drawFrame(QMap<Phonon::AudioDataOutput::Channel,QVector<qint16> >)));
    }

    return m_audioDataPath.isValid();
}

bool
VideoWindow::isDVD() const
{
    return m_media->currentSource().discType() == Phonon::Dvd;
}

QWidget*
VideoWindow::newPositionSlider()
{
    SeekSlider *seekSlider = new SeekSlider();
    seekSlider->setIconVisible( false );
    seekSlider->setMediaObject( m_media );
    seekSlider->setSingleStep( 5000 );
    return seekSlider;
}

QWidget*
VideoWindow::newVolumeSlider()
{
    VolumeSlider *volumeSlider = new VolumeSlider();
    volumeSlider->setObjectName( QLatin1String( "volume" ) );
    volumeSlider->setAudioOutput( m_aOutput );
    volumeSlider->setMuteVisible( false );
    volumeSlider->setOrientation( Qt::Vertical );
    return volumeSlider;
}

void
VideoWindow::stateChanged(Phonon::State currentState, Phonon::State oldstate) // slot
{
kDebug() << "chapters: " << m_controller->availableChapters() << " titles: " << m_controller->availableTitles();
    QStringList states;
    states << QLatin1String( "Loading" ) << QLatin1String( "Stopped" ) << QLatin1String( "Playing" ) << QLatin1String( "Buffering" ) << QLatin1String( "Paused" ) << QLatin1String( "Error" );
    kDebug() << "going from " << states.at(oldstate) << " to " << states.at(currentState);

    if( currentState == Phonon::PlayingState && m_initialOffset > 0) {
        seek(m_initialOffset);
        m_initialOffset = 0;
    }

    if( currentState == Phonon::PlayingState  && m_media->hasVideo() )
    {
        m_logo->hide();
        m_vWidget->show();
        updateChannels();

        if(m_adjustedSize==false)
        {
           if( mainWindow() )
             ( (QWidget*) mainWindow() )->adjustSize();
          m_adjustedSize=true;
          kDebug() << "adjusting size to video resolution";
        }
    }
    emit stateUpdated( currentState, oldstate );
}

void
VideoWindow::settingChanged( int setting )
{
    const QString name = sender()->objectName();
    const double dSetting = static_cast<double>( setting ) * 0.01;
    kDebug() << "setting " << name << " to " << dSetting;
    if( name == QLatin1String( "brightnessSlider" ) )
    {
        m_vWidget->setBrightness( dSetting );
    }
    else if( name == QLatin1String( "contrastSlider" ) )
    {
        m_vWidget->setContrast( dSetting );
    }
    else if( name == QLatin1String( "hueSlider" ) )
    {
        m_vWidget->setHue( dSetting );
    }
    else if( name == QLatin1String( "saturationSlider" ) )
    {
        m_vWidget->setSaturation( dSetting );
    }
}

void
VideoWindow::loadSettings()
{
    if( TheStream::hasProfile() )
    {
        KConfigGroup profile = TheStream::profile();
        m_vWidget->setBrightness( profile.readEntry<double>( "Brightness", 0.0 ) );
        m_vWidget->setContrast( profile.readEntry<double>( "Contrast", 0.0 ) );
        m_vWidget->setHue( profile.readEntry<double>( "Hue", 0.0 ) );
        m_vWidget->setSaturation( profile.readEntry<double>( "Saturation", 0.0 ) );
        setAudioChannel( profile.readEntry<int>( "AudioChannel", -1 ) );
        setSubtitle( profile.readEntry<int>( "Subtitle",  -1 ) );
    }
    else
    {
        m_vWidget->setBrightness( 0.0 );
        m_vWidget->setContrast( 0.0 );
        m_vWidget->setHue( 0.0 );
        m_vWidget->setSaturation( 0.0 );
    }
}

template<class ChannelDescription> void
VideoWindow::updateActionGroup( QActionGroup* channelActions
    , const QList<ChannelDescription>& availableChannels
    , const char* actionSlot )
{
    {
        QList<QAction*> subActions = channelActions->actions();
        while( 2 < subActions.size() )
            delete subActions.takeLast();
    }
    foreach( const ChannelDescription &channel, availableChannels )
    {
        QAction* lang = new QAction( channelActions );
        kDebug() << "the text is: \"" << channel.name() << "\" and index " << channel.index();
        lang->setCheckable( true );
        lang->setText( channel.name() );
        lang->setProperty( TheStream::CHANNEL_PROPERTY, channel.index() );
        connect( lang, SIGNAL(triggered()), this, actionSlot );
    }
}

void
VideoWindow::updateChannels()
{
    updateActionGroup( m_subLanguages, m_controller->availableSubtitles(), SLOT(slotSetSubtitle()) );
    emit subChannelsChanged( m_subLanguages->actions() );
    updateActionGroup( m_audioLanguages, m_controller->availableAudioChannels(), SLOT(slotSetAudio()) );
    emit audioChannelsChanged( m_audioLanguages->actions() );
}

void
VideoWindow::hideCursor()
{
   if(m_media->hasVideo() && m_vWidget->underMouse() )
       kapp->setOverrideCursor( Qt::BlankCursor );
}

void
VideoWindow::setSubtitle( int channel )
{
    Phonon::SubtitleDescription desc = Phonon::SubtitleDescription::fromIndex( channel );
    kDebug() << "using index: " << channel << " returned desc has index: " << desc.index();
    if(desc.isValid())
      m_controller->setCurrentSubtitle( desc );
}

void
VideoWindow::slotSetSubtitle()
{
    if( sender() && sender()->property( TheStream::CHANNEL_PROPERTY ).canConvert<int>() )
        setSubtitle( sender()->property( TheStream::CHANNEL_PROPERTY ).toInt() );
}

void
VideoWindow::setAudioChannel( int channel )
{
    Phonon::AudioChannelDescription desc = Phonon::AudioChannelDescription::fromIndex( channel );
    kDebug() << "using index: " << channel << " returned desc has index: " << desc.index();
    if(desc.isValid())
      m_controller->setCurrentAudioChannel( desc );
}

void
VideoWindow::slotSetAudio()
{
    if( sender() && sender()->property( TheStream::CHANNEL_PROPERTY ).canConvert<int>() )
        setAudioChannel( sender()->property( TheStream::CHANNEL_PROPERTY ).toInt() );
}

void
VideoWindow::toggleDVDMenu()
{
#if PHONON_VERSION >= PHONON_VERSION_CHECK(4, 5, 0)
    m_controller->setCurrentMenu(MediaController::RootMenu);
#endif
}

int
VideoWindow::videoSetting( const QString& setting )
{
    double dValue = 0.0;
    if( setting == QLatin1String( "brightnessSlider" ) )
    {
        dValue = m_vWidget->brightness();
    }
    else if( setting == QLatin1String( "contrastSlider" ) )
    {
        dValue = m_vWidget->contrast();
    }
    else if( setting == QLatin1String( "hueSlider" ) )
    {
        dValue = m_vWidget->hue();
    }
    else if( setting == QLatin1String( "saturationSlider" ) )
    {
        dValue = m_vWidget->saturation();
    }
    return static_cast<int>( dValue * 100.0 );
}

void
VideoWindow::prevChapter()
  {
    m_controller->setCurrentChapter(m_controller->currentChapter() - 1);
  }

void
VideoWindow::nextChapter()
  {
    m_controller->setCurrentChapter(m_controller->currentChapter() + 1);
  }

void
VideoWindow::tenPercentBack()
{
  qint64 newTime = m_media->currentTime() - (m_media->totalTime() / 10);
  if (newTime > 0)
    m_media -> seek (newTime);
  else
    m_media -> seek ( 0 ) ;
  }

void
VideoWindow::tenPercentForward()
{
  qint64 newTime = m_media->currentTime() + (m_media->totalTime() / 10);
  if (newTime < m_media->totalTime())
	m_media -> seek (newTime);
}

void
VideoWindow::tenSecondsBack()
{
    relativeSeek( -10000 );
}

void
VideoWindow::tenSecondsForward()
{
    relativeSeek( 10000 );
}

void
VideoWindow::increaseVolume()
{
    m_aOutput->setVolume(qMin(qreal(1.0), volume() + qreal(0.10)));
}

void
VideoWindow::decreaseVolume()
{
    m_aOutput->setVolume(qMax(qreal(0.0), volume() - qreal(0.10)));
}

///////////
///Protected
///////////

bool
VideoWindow::event( QEvent* event )
{
    switch( event->type() )
    {
      case QEvent::Leave:
         m_cursorTimer->stop(); kDebug() << "stop cursorTimer";
      break;
      case QEvent::FocusOut:
         // if the user summons some dialog via a shortcut or whatever we need to ensure
         // the mouse gets shown, because if it is modal, we won't get mouse events after
         // it is shown! This works because we are always the focus widget.
         // @see MainWindow::MainWindow where we setFocusProxy()
      case QEvent::Enter:
      case QEvent::MouseMove:
      case QEvent::MouseButtonPress:
            kapp->restoreOverrideCursor();
            m_cursorTimer->start( CURSOR_HIDE_TIMEOUT );
         break;
      default: return QWidget::event( event );
    }
    return false;
}

void
VideoWindow::contextMenuEvent( QContextMenuEvent * event )
{
    KMenu menu;
    if( mainWindow() )
    {
        menu.addAction( action( "play" ) );
        menu.addAction( action( "fullscreen" ) );
        menu.addAction( action( "reset_zoom" ) );
        if(isDVD())
        {
            menu.addAction( action( "toggle_dvd_menu" ) );
        }
    }
    menu.exec( event->globalPos() );
}

void
VideoWindow::mouseDoubleClickEvent( QMouseEvent* )
{
    if( mainWindow() ) //TODO: add full screen mode to kpart
        action("fullscreen")->toggle();
}

QSize
VideoWindow::sizeHint() const //virtual
{
   QSize s = TheStream::profile().readEntry<QSize>( "Preferred Size", QSize() );

   if( !s.isValid() )
      s = TheStream::defaultVideoSize();

   if( s.isValid() && !s.isNull() )
      return s;

   return QWidget::sizeHint();
}

///////////
///Private
///////////
void
VideoWindow::eject()
{
    if( m_media->currentSource().type() == Phonon::MediaSource::Invalid )
        return;

    if( m_media->currentSource().type() == Phonon::MediaSource::Empty )
        return;


    KConfigGroup profile = TheStream::profile(); // the config profile for this video file

    Phonon::State state = m_media->state();
    if( ( ( state == Phonon::PlayingState || state == Phonon::PausedState ) )
            && ( m_media->remainingTime() > 5000 ) ) // if we are really close to the end, don't remember the position
        profile.writeEntry( "Position", currentTime() );
    else
        profile.deleteEntry( "Position" );

    const QSize s = videoWindow()->size();
    const QSize defaultSize = TheStream::defaultVideoSize();
    if( defaultSize.isValid() && ( s.width() == defaultSize.width() || s.height() == defaultSize.height() ) )
        profile.deleteEntry( "Preferred Size" );
    else
        profile.writeEntry( "Preferred Size", s );

    profile.writeEntry( "Contrast", m_vWidget->contrast() );
    profile.writeEntry( "Brightness", m_vWidget->brightness() );
    profile.writeEntry( "Hue", m_vWidget->hue() );
    profile.writeEntry( "Saturation", m_vWidget->saturation() );
    profile.writeEntry( "IsVideo",m_media->hasVideo());
    {
        //this if clause - is to prevent a crash from bug 162721 (a Phonon bug), remove when fixed
        if(m_media->hasVideo())
        {
          kDebug() << "trying to fetch subtitle information";
          const int subtitle = TheStream::subtitleChannel();
          const int audio = TheStream::audioChannel();
          kDebug() << "fetched subtitle information";


        if( subtitle != -1 )
            profile.writeEntry( "Subtitle", subtitle );
        else
            profile.deleteEntry( "Subtitle" );

        if( audio != -1 )
            profile.writeEntry( "AudioChannel", audio );
        else
            profile.deleteEntry( "AudioChannel" );
        }

    }
    profile.sync();
}

} //namespace Dragon

#include "videoWindow.moc"
