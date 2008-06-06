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


#define CODEINE_DEBUG_PREFIX "engine"

#include "videoWindow.h"
#include "timeLabel.h"

#include "actions.h"        //::seek() FIXME unfortunate
#include "debug.h"
#include "mxcl.library.h"
#include "theStream.h"

#include <xine.h>

#include <QActionGroup>
#include <QContextMenuEvent>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>

#include <KApplication>
#include <KLocale>
#include <KMenu>
#include <KStandardDirs>

#include <Phonon/AudioOutput>
#include <Phonon/MediaController>
#include <Phonon/MediaObject>
#include <Phonon/Path>
#include <Phonon/SeekSlider>
#include <Phonon/VideoWidget>
#include <Phonon/VolumeFaderEffect>
#include <Phonon/VolumeSlider>

#include <Solid/Block>
#include <Solid/Device>
#include <Solid/OpticalDisc>

using Phonon::AudioOutput;
using Phonon::MediaObject;
using Phonon::VideoWidget;
using Phonon::SeekSlider;
using Phonon::VolumeSlider;
using Phonon::MediaController;

namespace Codeine {


VideoWindow *VideoWindow::s_instance = 0;

VideoWindow::VideoWindow( QWidget *parent )
        : QWidget( parent )
        , m_cursorTimer( new QTimer( this ) )
        , m_justLoaded( false )
		, m_adjustedSize( false)
        , m_xineStream( 0 )
        , m_subLanguages( new QActionGroup( this ) )
        , m_audioLanguages( new QActionGroup( this ) )
        , m_logo( new QLabel( this ) )
{
    DEBUG_BLOCK

    s_instance = this;
    setObjectName( "VideoWindow" );

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
    connect( m_media, SIGNAL( tick( qint64 ) ), this, SIGNAL( tick( qint64 ) ) );
    connect( m_media, SIGNAL( totalTimeChanged( qint64 ) ), this, SIGNAL( totalTimeChanged( qint64 ) ) );
    connect( m_media, SIGNAL( seekableChanged( bool ) ), this, SIGNAL( seekableChanged( bool ) ) );
    connect( m_aOutput, SIGNAL( mutedChanged( bool ) ), this, SIGNAL( mutedChanged( bool ) ) );

    connect( m_media, SIGNAL( hasVideoChanged( bool ) ), m_vWidget, SLOT( setVisible( bool ) ) ); //hide video widget if no video to show
    connect( m_media, SIGNAL( hasVideoChanged( bool ) ), m_logo, SLOT( setHidden( bool ) ) );

    connect( m_controller, SIGNAL( availableSubtitlesChanged() ), this, SLOT( updateChannels() ) );

    {
        m_subLanguages->setExclusive( true );
        QAction* turnOff = new QAction( i18n("&DVD Subtitle Selection"), m_subLanguages );
        turnOff->setCheckable( true );
        turnOff->setProperty( TheStream::CHANNEL_PROPERTY, -1 );
        connect( turnOff, SIGNAL( triggered() ), this, SLOT( slotSetSubtitle() ) );

        QAction* seperator = new QAction( m_subLanguages );
        seperator->setSeparator( true );
    }
    {
        m_audioLanguages->setExclusive( true );
        QAction* autoLang = new QAction( i18n("&Auto"), m_audioLanguages );
        autoLang->setProperty( TheStream::CHANNEL_PROPERTY, -1 );
        autoLang->setCheckable( true );
        connect( autoLang, SIGNAL( triggered() ), this, SLOT( slotSetAudio() ) );

        QAction* seperator = new QAction( m_audioLanguages );
        seperator->setSeparator( true );
    }

    connect( m_media, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(stateChanged(Phonon::State,Phonon::State)) );
    connect( m_cursorTimer, SIGNAL( timeout() ), this, SLOT( hideCursor() ) );
    m_cursorTimer->setSingleShot( true );
    {
        m_logo->setAutoFillBackground( true );
        QPalette pal;
        pal.setColor( QPalette::Window, Qt::white );
        m_logo->setPalette( pal );
        QLayout* layout = new QVBoxLayout( m_logo );
        layout->setAlignment( Qt::AlignCenter );
//        QLabel* logoImage = new QLabel( m_logo );
//        logoImage->setPixmap( KStandardDirs::locate( "appdata",  "dragonlogo.png" ) );
//        layout->addWidget( logoImage );
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
    DEBUG_BLOCK
    /* let us never forget, mxcl's cum variable. RIP
    int cum = 0;
    for( int v = 99; v >= 0; v-- ) {
        xine_set_param( m_stream, XINE_PARAM_AUDIO_AMP_LEVEL, v );
        int sleep = int(32000 * (-std::log10( double(v + 1) ) + 2));
        ::usleep( sleep );
        cum += sleep;
    }
    debug() << "Total sleep: " << cum << "x10^-6 s\n"; */

    eject();
    KConfigGroup config = KGlobal::config()->group( "General" );
    config.writeEntry( "Volume", static_cast<double>( m_aOutput->volume() ) );

    if( m_media->state() == Phonon::PlayingState )
    {
        Phonon::VolumeFaderEffect* faderEffect = new Phonon::VolumeFaderEffect( this );
        m_audioPath.insertEffect( faderEffect );
        faderEffect->setFadeCurve( Phonon::VolumeFaderEffect::Fade12Decibel );
        faderEffect->fadeOut( 500 );
        ::usleep( 700000 );
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
    DEBUG_BLOCK
    mxcl::WaitCursor allocateOnStack;
    eject();
    m_media->setCurrentSource( url );
    m_justLoaded = true;
	m_adjustedSize=false;
    return true;
}

bool
VideoWindow::play( qint64 offset )
{
    DEBUG_BLOCK
    mxcl::WaitCursor allocateOnStack;
    m_justLoaded = false;
    if( offset > 0 )
        seek( offset );
    m_media->play();
    return true;
}

bool
VideoWindow::resume()
{
    m_media->play();
    return true;
}

bool
VideoWindow::mouseUnderWidget()
{
	return m_vWidget->underMouse();
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
DEBUG_BLOCK
    QString devicePath;
    {
        const Solid::Block* block = device.as<const Solid::Block>();
        if( block )
            devicePath = block->device();
        else
        {
            debug() << "device was not a block";
            return false;
        }
    }
    const Solid::OpticalDisc* disc = device.as<const Solid::OpticalDisc>();
    if( disc )
    {
        Phonon::DiscType phononType = Phonon::NoDisc;
        {
            Solid::OpticalDisc::ContentTypes solidType = disc->availableContent();
            if( solidType & Solid::OpticalDisc::VideoDvd )
                phononType = Phonon::Dvd;
            else if( solidType & ( Solid::OpticalDisc::VideoCd | Solid::OpticalDisc::SuperVideoCd ) )
                phononType = Phonon::Vcd;
            else if( solidType &  Solid::OpticalDisc::Audio )
                phononType = Phonon::Cd;
            else
            {
                debug() << "not a playable disc type: " << disc->availableContent() << " type";
                return false;
            }
        }
        eject();
        m_media->setCurrentSource( Phonon::MediaSource( phononType, devicePath ) );
        debug() << "actually playing the disc at " << devicePath;
        m_media->play();
        return true;
    }
    else
    {
        debug() << "device was not a disc";
        return false;
    }
}

void
VideoWindow::relativeSeek( qint64 step )
{
    m_media->pause();
    const qint64 new_pos = currentTime() + step;
    if( new_pos > 0 )
        seek( new_pos );
    play();
}

void
VideoWindow::stop()
{
    eject();
    m_media->stop();
    m_media->setCurrentSource( Phonon::MediaSource() ); //set the current source to invalid
    m_vWidget->hide();
    m_logo->show();
}

void
VideoWindow::pause()
{
    m_media->pause();
}

QString
VideoWindow::urlOrDisc() const
{
    Phonon::MediaSource source = m_media->currentSource();
    switch( source.type() )
    {
        case Phonon::MediaSource::Invalid:
            return "Invalid"; //no i18n, used for DBus responses
            break;
        case Phonon::MediaSource::Url:
        case Phonon::MediaSource::LocalFile:
            return source.url().toString();
            break;
        case Phonon::MediaSource::Disc:
            return source.deviceName();
            break;
        case Phonon::MediaSource::Stream:
            return "Data Stream";
            break;
        default:
            break;
    }
    return "Error";
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


Engine::State
VideoWindow::state() const
{
    return state( m_media->state() ); 
}

Engine::State
VideoWindow::state( Phonon::State state ) const
{
    if( m_media->currentSource().type() == Phonon::MediaSource::Invalid )
        return Engine::Empty;
    else if( m_justLoaded )
        return Engine::Loaded;
    
/*
    enum State
    {
        Uninitialised = 0,
        Empty = 1,
        Loaded = 2,
        Playing = 4,
        Paused = 8,
        TrackEnded = 16
    }; */
    switch( state )
    {
        case Phonon::StoppedState:
            return Engine::TrackEnded;
        break;
        case Phonon::BufferingState:
        case Phonon::LoadingState:
            return Engine::Loaded;
		break;
        case Phonon::PlayingState:
            return Engine::Playing;
        break;
        case Phonon::PausedState:
            return Engine::Paused;
        break;
        case Phonon::ErrorState:
        default:
            return Engine::Uninitialised;
        break;
    }
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
    DEBUG_BLOCK
//    bool wasPaused = false;

    // If we seek to the end the track ended event is sent, but it is
    // delayed as it happens in xine-event loop and before that we are
    // already processing the next seek event (if user uses mouse wheel
    // or keyboard to seek) and this causes the ui to think video is
    // stopped but xine is actually playing the track. Tada!
    // TODO set state based on events from xine only

    switch( state() ) {
    case Engine::Uninitialised:
        //NOTE should never happen
        Debug::warning() << "Seek attempt thwarted! xine not initialised!\n";
        return;
    case Engine::Empty:
        Debug::warning() << "Seek attempt thwarted! No media loaded!\n";
        return;
    default:
        ;
    }
    m_media->pause(); //pausing first gives Phonon a chance to recognize seekable media
    m_media->seek( pos );
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
    if( mainWindow() )
        mainWindow()->adjustSize();
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
VideoWindow::isDVD() const
{
    return m_media->currentSource().discType() == Phonon::Dvd;
}

QWidget*
VideoWindow::newPositionSlider()
{
    SeekSlider *seekSlider = new SeekSlider();
    seekSlider->setMediaObject( m_media );
    return seekSlider;
}

QWidget*
VideoWindow::newVolumeSlider()
{
    VolumeSlider *volumeSlider = new VolumeSlider();
    volumeSlider->setObjectName( "volume" );
    volumeSlider->setAudioOutput( m_aOutput );
    volumeSlider->setMuteVisible( false );
    volumeSlider->setOrientation( Qt::Vertical );
    return volumeSlider;
}

void
VideoWindow::refreshXineStream()
{
DEBUG_BLOCK
   if( m_media->property( "xine_stream_t" ).canConvert<void*>() )
  //  if( m_media->property( "xine_stream_t" ).isValid() )
    {
        debug() << "value property " <<  m_media->property( "xine_stream_t" ).type();
        m_xineStream = (xine_stream_t*) m_media->property( "xine_stream_t" ).value<void*>();
    }
    else
    {
        debug() << "mrrrrrr, QVariant property xine_stream_t isn't a void*.";
        m_xineStream = 0;
    }
}

void
VideoWindow::stateChanged(Phonon::State currentState, Phonon::State oldstate) // slot
{
DEBUG_BLOCK
debug() << "chapters: " << m_controller->availableChapters() << " titles: " << m_controller->availableTitles();
    QStringList states;
    states << "Loading" << "Stopped" << "Playing" << "Buffering" << "Paused" << "Error";
    debug() << "going from " << states.at(oldstate) << " to " << states.at(currentState);

    if( currentState == Phonon::LoadingState )
        m_xineStream = 0;


    //N.B this code is also run when coming out of Paused state, as Phonon goes Paused->Buffering->Playing (but at least this saves doing it twice)
    if( currentState == Phonon::PlayingState && oldstate != Phonon::PausedState && m_media->hasVideo() )
    {
        m_logo->hide();
        m_vWidget->show();
        refreshXineStream();
        updateChannels();

		if(m_adjustedSize==false)
		{
		  ( (QWidget*) mainWindow() )->adjustSize();
		  m_adjustedSize=true;
		  debug() << "adjusting size to video resolution";
		}
        //m_vWidget->updateGeometry();
        //updateGeometry();
        
    }
    emit stateChanged( state( currentState ) ); 
}

void
VideoWindow::settingChanged( int setting )
{
    const QString name = sender()->objectName();
    const double dSetting = static_cast<double>( setting ) * 0.01;
    debug() << "setting " << name << " to " << dSetting;
    if( name == "brightnessSlider" )
    {
        m_vWidget->setBrightness( dSetting );
    }
    else if( name == "contrastSlider" )
    {
        m_vWidget->setContrast( dSetting );
    }
    else if( name == "hueSlider" )
    {
        m_vWidget->setHue( dSetting );
    }
    else if( name == "saturationSlider" )
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
        debug() << "the text is: \"" << channel.name() << "\" and index " << channel.index();
        lang->setCheckable( true );
        lang->setText( channel.name() );
        lang->setProperty( TheStream::CHANNEL_PROPERTY, channel.index() );
        connect( lang, SIGNAL( triggered() ), this, actionSlot );
    }
}

void
VideoWindow::updateChannels()
{
    DEBUG_BLOCK
    updateActionGroup( m_subLanguages, m_controller->availableSubtitles(), SLOT( slotSetSubtitle() ) );
    emit subChannelsChanged( m_subLanguages->actions() );
    updateActionGroup( m_audioLanguages, m_controller->availableAudioChannels(), SLOT( slotSetAudio() ) );
    emit audioChannelsChanged( m_audioLanguages->actions() );
}

void
VideoWindow::hideCursor()
{
   DEBUG_BLOCK
   if(m_media->hasVideo() && m_vWidget->underMouse() )
       kapp->setOverrideCursor( Qt::BlankCursor );
}

void
VideoWindow::setSubtitle( int channel )
{
    DEBUG_BLOCK
    Phonon::SubtitleDescription desc = Phonon::SubtitleDescription::fromIndex( channel );
    debug() << "using index: " << channel << " returned desc has index: " << desc.index();
    m_controller->setCurrentSubtitle( desc );
}

void
VideoWindow::slotSetSubtitle()
{
    DEBUG_BLOCK
    if( sender() && sender()->property( TheStream::CHANNEL_PROPERTY ).canConvert<int>() )
        setSubtitle( sender()->property( TheStream::CHANNEL_PROPERTY ).toInt() );
}

void
VideoWindow::setAudioChannel( int channel )
{
    DEBUG_BLOCK
    Phonon::AudioChannelDescription desc = Phonon::AudioChannelDescription::fromIndex( channel );
    debug() << "using index: " << channel << " returned desc has index: " << desc.index();
    m_controller->setCurrentAudioChannel( desc );
}

void
VideoWindow::slotSetAudio()
{
    DEBUG_BLOCK
    if( sender() && sender()->property( TheStream::CHANNEL_PROPERTY ).canConvert<int>() )
        setAudioChannel( sender()->property( TheStream::CHANNEL_PROPERTY ).toInt() );
}

void
VideoWindow::toggleDVDMenu()
{
    if( m_xineStream )
    {
        xine_event_t e;
        e.type = XINE_EVENT_INPUT_MENU1;
        e.data = NULL;
        e.data_length = 0;
        xine_event_send( m_xineStream, &e );
    }
}

int
VideoWindow::videoSetting( const QString& setting )
{
    double dValue = 0.0;
    if( setting == "brightnessSlider" )
    {
        dValue = m_vWidget->brightness();
    }
    else if( setting == "contrastSlider" )
    {
        dValue = m_vWidget->contrast();
    }
    else if( setting == "hueSlider" )
    {
        dValue = m_vWidget->hue();
    }
    else if( setting == "saturationSlider" )
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
VideoWindow::tenBack()
  {
    qint64 newTime = m_media->currentTime() - (m_media->totalTime() / 10);
    if (newTime > 0)
	m_media -> seek (newTime);
      else
        m_media -> seek ( 0 ) ;
  }

void
VideoWindow::tenForward()
  {
    qint64 newTime = m_media->currentTime() + (m_media->totalTime() / 10);
    if (newTime < m_media->totalTime())
	m_media -> seek (newTime);
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
         m_cursorTimer->stop(), debug() << "stop cursorTimer";
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
         if( hasFocus() )
            // see above comment
            debug() << "cursor will disappear in 2000 seconds";
            m_cursorTimer->start( CURSOR_HIDE_TIMEOUT );
         break;
      default: return QWidget::event( event );
    }
    return false;
}

void
VideoWindow::contextMenuEvent( QContextMenuEvent * event )
{
    DEBUG_BLOCK
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
DEBUG_BLOCK
    if( m_media->currentSource().type() == Phonon::MediaSource::Invalid )
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
		debug() << "trying to fetch subtitle information";
        const int subtitle = TheStream::subtitleChannel();
        const int audio = TheStream::audioChannel();
		debug() << "fetched subtitle information";


        if( subtitle != -1 )
            profile.writeEntry( "Subtitle", subtitle );
        else
            profile.deleteEntry( "Subtitle" );

        if( audio != -1 )
            profile.writeEntry( "AudioChannel", audio );
        else
            profile.deleteEntry( "AudioChannel" );
    }
    profile.sync();
}

} //namespace Codeine

#include "videoWindow.moc"
