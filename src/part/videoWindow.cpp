// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#define CODEINE_DEBUG_PREFIX "videoWindow"

#include <cstdlib>
#include "debug.h"
#include <qapplication.h> //sendEvent()
#include <qcursor.h>
#include <qevent.h>
#include "videoWindow.h"
#include <X11/Xlib.h>     //TODO this breaks compile for lots of people due to excessive macro content
#include <xine.h>         //x11_visual_t


namespace Codeine {


VideoWindow *VideoWindow::s_instance = 0;


namespace X
{
   Display *d;
   int s, w;
}


VideoWindow::VideoWindow( QWidget *parent, const char *name )
      : QWidget( parent, name )
      , m_osd( 0 )
      , m_stream( 0 )
      , m_eventQueue( 0 )
      , m_videoPort( 0 )
      , m_audioPort( 0 )
      , m_xine( 0 )
      , m_displayRatio( 1 )
{
   s_instance = this;

   // with this Konqueror would crash on exit
   // without this we may be unstable!
   //XInitThreads();

   show();

   setWFlags( Qt::WNoAutoErase );
   setMouseTracking( true );
   setAcceptDrops( true );
   setUpdatesEnabled( false ); //to stop Qt drawing over us
   setPaletteBackgroundColor( Qt::black );

   X::d = XOpenDisplay( std::getenv("DISPLAY") );
   X::s = DefaultScreen( X::d );
   X::w = winId();

   XLockDisplay( X::d );
   XSelectInput( X::d, X::w, ExposureMask );

   {
      using X::d; using X::s;

      //these are Xlib macros
      double w = DisplayWidth( d, s ) * 1000 / DisplayWidthMM( d, s );
      double h = DisplayHeight( d, s ) * 1000 / DisplayHeightMM( d, s );

      m_displayRatio = w / h;
   }

   XUnlockDisplay( X::d );

   connect( &m_timer, SIGNAL(timeout()), SLOT(hideCursor()) );
}

VideoWindow::~VideoWindow()
{
   DEBUG_BLOCK

   if( m_osd )        xine_osd_free( m_osd );
   if( m_stream )     xine_close( m_stream );
   if( m_eventQueue ) xine_event_dispose_queue( m_eventQueue );
   if( m_stream )     xine_dispose( m_stream );
   if( m_videoPort )  xine_close_video_driver( m_xine, m_videoPort );
   if( m_audioPort )  xine_close_audio_driver( m_xine, m_audioPort );
   if( m_xine )       xine_exit( m_xine );

   XCloseDisplay( X::d );
}

void*
VideoWindow::x11Visual() const
{
   x11_visual_t* visual = new x11_visual_t;

   visual->display          = X::d;
   visual->screen           = X::s;
   visual->d                = X::w;
   visual->dest_size_cb     = &VideoWindow::destSizeCallBack;
   visual->frame_output_cb  = &VideoWindow::frameOutputCallBack;
   visual->user_data        = (void*)this;

   return visual;
}

void
VideoWindow::destSizeCallBack(
      void* p, int /*video_width*/, int /*video_height*/,
      double /*video_aspect*/, int* dest_width,
      int* dest_height, double* dest_aspect )
{
   if( !p )
      return;

   #define vw static_cast<VideoWindow*>(p)

   *dest_width  = vw->width();
   *dest_height = vw->height();
   *dest_aspect = vw->m_displayRatio;
}

void
VideoWindow::frameOutputCallBack(
      void* p, int video_width, int video_height, double video_aspect,
      int* dest_x, int* dest_y, int* dest_width, int* dest_height,
      double* dest_aspect, int* win_x, int* win_y )
{
   if( !p )
      return;

   *dest_x = 0;
   *dest_y = 0 ;
   *dest_width  = vw->width();
   *dest_height = vw->height();
   *win_x = vw->x();
   *win_y = vw->y();
   *dest_aspect = vw->m_displayRatio;

   // correct size with video aspect
   // TODO what's this about?
   if( video_aspect >= vw->m_displayRatio )
      video_width  = int( double(video_width * video_aspect / vw->m_displayRatio + 0.5) );
   else
      video_height = int( double(video_height * vw->m_displayRatio / video_aspect) + 0.5 );

   #undef vw
}

bool
VideoWindow::event( QEvent *e )
{
   switch( e->type() )
   {
   case QEvent::MouseMove:
   case QEvent::MouseButtonPress:
      unsetCursor();
      m_timer.start( CURSOR_HIDE_TIMEOUT, true );
      break;

   case QEvent::Close:
   case QEvent::Hide:
      xine_stop( m_stream );
      break;

   case QEvent::Leave:
      m_timer.stop();
      break;

   default:
      ;
   }

   return QWidget::event( e );
}

bool
VideoWindow::x11Event( XEvent *e )
{
   if( e->type == Expose && e->xexpose.count == 0 ) {
      xine_gui_send_vo_data( m_stream, XINE_GUI_SEND_EXPOSE_EVENT, e );
      return true;
   }

   return false;
}

void
VideoWindow::hideCursor()
{
   setCursor( Qt::BlankCursor );
}

} //namespace Codeine
