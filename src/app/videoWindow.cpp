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

#include "actions.h"        //::seek() FIXME unfortunate
#include "debug.h"
#include "mxcl.library.h"
#include "theStream.h"
#include "videoWindow.h"

#include <xine.h>

#include <QActionGroup>
#include <QContextMenuEvent>
#include <QVBoxLayout>

#include <KLocale>
#include <KMenu>
#include <Phonon/Path>
#include <Phonon/AudioOutput>
#include <Phonon/MediaObject>
#include <Phonon/VideoWidget>
#include <Phonon/SeekSlider>
#include <Phonon/VolumeSlider>

using Phonon::AudioOutput;
using Phonon::MediaObject;
using Phonon::VideoWidget;
using Phonon::SeekSlider;
using Phonon::VolumeSlider;

namespace Codeine {


VideoWindow *VideoWindow::s_instance = 0;

VideoWindow::VideoWindow( QWidget *parent )
        : QWidget( parent )
        , m_justLoaded( false )
        , m_xineStream( 0 )
        , m_languages( new QActionGroup( this ) )
{
    DEBUG_BLOCK

    s_instance = this;
    setObjectName( "VideoWindow" );

    QVBoxLayout *box = new QVBoxLayout( this );
    box->setMargin(0);
    box->setSpacing(0);
    m_vWidget = new VideoWidget( this );
    box->addWidget( m_vWidget );
    m_aOutput = new AudioOutput( Phonon::VideoCategory, this );
    m_media = new MediaObject( this );
    Phonon::createPath(m_media, m_vWidget);
    Phonon::createPath(m_media, m_aOutput);
    m_media->setTickInterval( 350 );

    m_languages->setExclusive( true );

    connect( m_media, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(stateChanged(Phonon::State,Phonon::State)) );

}

VideoWindow::~VideoWindow()
{
    DEBUG_BLOCK
    return;
}

bool
VideoWindow::init()
{
    return true;
}

bool
VideoWindow::load( const KUrl &url )
{
    DEBUG_BLOCK
    mxcl::WaitCursor allocateOnStack;
    m_media->setCurrentSource( url );
    m_url = url;
    m_justLoaded = true;
    return true;
}

bool
VideoWindow::play( qint64 offset )
{
    DEBUG_BLOCK
    mxcl::WaitCursor allocateOnStack;
    m_justLoaded = false;
    seek( offset );
    m_media->play();
    return true;
}
bool
VideoWindow::playDvd()
{
    m_media->setCurrentSource( Phonon::MediaSource( Phonon::Dvd ) );
    m_media->play();
    return true;
}

void
VideoWindow::record()
{ 
    return;
}

void
VideoWindow::stop()
{
    m_media->stop();
}

void
VideoWindow::playPause()
{
    if( m_media->state() == Phonon::PlayingState )
        m_media->pause();
    else
        m_media->play();
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
    switch( state )
    {

        case Phonon::StoppedState:
            return Engine::TrackEnded;
        break;

        case Phonon::LoadingState:
        case Phonon::BufferingState:
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

uint
VideoWindow::volume() const
{
    return static_cast<uint>( m_aOutput->volume() * 1.0 );
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
    case Engine::Loaded:
    // then the state is changing and we should announce it
        play( pos );
        return;
    default:
        ;
    }

    if( !TheStream::canSeek() ) {
        // for http streaming it is not a good idea to seek as xine freezes
        // and/or just breaks, this is xine 1.0.1
        Debug::warning() << "We won't try to seek as the media is not seekable!\n";
        return;
    }
    m_media->seek( pos );
    const bool fullscreen = toggleAction("fullscreen")->isChecked();
    if( fullscreen ) {
//        xine_osd_draw_text( m_osd, 0, 0, osd.utf8(), XINE_OSD_TEXT1 );
    }
}

void
VideoWindow::setStreamParameter( int value )
{
    return;
}

void
VideoWindow::showOSD( const QString &message )
{
    return;
}

void
VideoWindow::setFullScreen( bool full )
{
     m_vWidget->setFullScreen( full );
}

QString
VideoWindow::fileFilter() const
{
    return "*.avi *.mp3 *.mpg *.mpeg";
}

qint64
VideoWindow::currentTime() const
{
 //   return currentTime();
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
    return volumeSlider;
}

void
VideoWindow::refreshXineStream()
{
DEBUG_BLOCK
   // if( m_media->property( "xine_stream_t" ).canConvert<void*>() )
    if( m_media->property( "xine_stream_t" ).isValid() )
    {
        debug() << "value property " <<  m_media->property( "xine_stream_t" ).type();
        m_xineStream = (xine_stream_t*) m_media->property( "xine_stream_t" ).value<void*>();
    }
    else
    {
        debug() << "mrrrrrr, xine_stream_t is invalid";
        m_xineStream = 0;
    }
}

void
VideoWindow::stateChanged(Phonon::State currentState, Phonon::State /*oldstate*/) // slot
{
    if( currentState == Phonon::LoadingState )
        m_xineStream = 0;
    if( currentState == Phonon::PlayingState )
    {
        debug() << "hooray, a playing state";
        refreshXineStream();
        if( m_xineStream )
        {
            {
                QList<QAction*> subActions = m_languages->actions();
                foreach( QAction* subAction, subActions )
                    delete subAction;
            }
            int channels = xine_get_stream_info( m_xineStream, XINE_STREAM_INFO_MAX_SPU_CHANNEL );
            debug() << "\033[0;43mOne xine stream pls: " << m_xineStream << "\033[0m" << ' ' << channels;
            for( int j = 0; j < channels; j++ )
            {
                char s[128];
                QAction* lang = new QAction( m_languages );
                lang->setText( xine_get_spu_lang( m_xineStream, j, s ) ? s : i18n("Channel %1", j+1 ) );
                debug() << "added language " << lang->text();
                lang->setProperty( TheStream::CHANNEL_PROPERTY, j );
                m_languages->addAction( lang );
            }
            emit channelsChanged( m_languages->actions() );
        }
        else
            debug() << "\033[0;43mWhy is there no m_xineStream?\033[0m";
    }
    emit stateChanged( state( currentState ) ); 
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

///////////
///Protected
///////////
void
VideoWindow::contextMenuEvent( QContextMenuEvent * event )
{
    KMenu menu;
    menu.addAction( action( "play" ) );
    menu.addAction( action( "fullscreen" ) );
    menu.addAction( action( "reset_zoom" ) );
    menu.addAction( action( "xine_settings" ) );
    menu.exec( event->globalPos() );
}


} //namespace Codeine

#include "videoWindow.moc"
