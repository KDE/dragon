// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#define CODEINE_DEBUG_PREFIX "engine"

#include "actions.h"        //::seek() FIXME unfortunate
#include "configfn.h"
#include "debug.h"
#include <klocale.h>
#include "mxcl.library.h"
#include "slider.h"
#include "theStream.h"
#include "xineEngine.h"


namespace Codeine {


VideoWindow *VideoWindow::s_instance = 0;


VideoWindow::VideoWindow( QWidget *parent )
        : QWidget( parent, "VideoWindow" )
{
    DEBUG_BLOCK

    s_instance = this;

    setWindowFlags( Qt::WNoAutoErase );
    setMouseTracking( true );
    setAcceptDrops( true );
    setUpdatesEnabled( false ); //to stop Qt drawing over us
    setPaletteBackgroundColor( Qt::black );
    setFocusPolicy( Qt::ClickFocus );

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
    mxcl::WaitCursor allocateOnStack;
    return true;
}

bool
VideoWindow::play( uint offset )
{
    mxcl::WaitCursor allocateOnStack;
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
    return;
}

void
VideoWindow::pause()
{
    return;
}


Engine::State
VideoWindow::state() const
{
    return Engine::Uninitialised;
}

uint
VideoWindow::volume() const
{
    return 0;
}

void
VideoWindow::seek( uint pos )
{
    bool wasPaused = false;

    // If we seek to the end the track ended event is sent, but it is
    // delayed as it happens in xine-event loop and before that we are
    // already processing the next seek event (if user uses mouse wheel
    // or keyboard to seek) and this causes the ui to think video is
    // stopped but xine is actually playing the track. Tada!
    // TODO set state based on events from xine only
    if( pos > 65534 )
        pos = 65534;

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
    case Engine::Paused:
        // xine_play unpauses stream if stream was paused
        // was broken at 1.0.1 still
        wasPaused = true;
//        xine_set_param( m_stream, XINE_PARAM_AUDIO_AMP_MUTE, 1 );
        break;
    default:
        ;
    }

    if( !TheStream::canSeek() ) {
        // for http streaming it is not a good idea to seek as xine freezes
        // and/or just breaks, this is xine 1.0.1
        Debug::warning() << "We won't try to seek as the media is not seekable!\n";
        return;
    }

    //TODO depend on a version that CAN seek in flacs!

    //better feedback
    //NOTE doesn't work! I can't tell why..
    Slider::instance()->QSlider::setValue( pos );
    Slider::instance()->repaint( false );

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
VideoWindow::toggleDVDMenu()
{
    return;
}

void
VideoWindow::showOSD( const QString &message )
{
    return;
}

QString
VideoWindow::fileFilter() const
{
    return "*.avi *.mp3 *.mpg *.mpeg";
}

} //namespace Codeine

#include "xineEngine.moc"
