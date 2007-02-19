// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#define CODEINE_DEBUG_PREFIX "VideoWindow"

#include "actions.h"
#include <cmath> //std::log10
#include <cstdlib>
#include "debug.h"
#include <kapplication.h> //::makeStandardCaption
#include <kconfig.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kwin.h>
#include "mxcl.library.h"
#include <qcursor.h>
#include <qevent.h>
#include "slider.h"
#include "theStream.h"
#include <X11/Xlib.h>
#include <xine.h>
#include "xineEngine.h"


namespace Codeine
{
   namespace X
   {
      // we get thread locks if we don't cache these values
      // (I don't know which ones exactly)
      Display *d;
      int s, w;
   }


void
VideoWindow::initVideo()
{
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

   connect( &m_timer, SIGNAL(timeout()), SLOT(hideCursor()) );

   XUnlockDisplay( X::d );
}

void
VideoWindow::cleanUpVideo()
{
   XCloseDisplay( X::d );
}

void*
VideoWindow::x11Visual() const
{
   DEBUG_FUNC_INFO

   x11_visual_t* visual = new x11_visual_t;

   visual->display          = X::d;
   visual->screen           = X::s;
   visual->d                = winId();//X::w;
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
      video_width  = (int) ( (double) (video_width * video_aspect / vw->m_displayRatio + 0.5) );
   else
      video_height = (int) ( (double) (video_height * vw->m_displayRatio / video_aspect) + 0.5);

   #undef vw
}

void
VideoWindow::contextMenuEvent( QContextMenuEvent *e )
{
   e->accept();

   KPopupMenu popup;

   if( state() == Engine::Playing )
      popup.insertItem( SmallIconSet("player_pause"), i18n("Pause"), 1 );
   else
      action( "play" )->plug( &popup );

   popup.insertSeparator();

   if( TheStream::url().protocol() == "dvd" )
      action( "toggle_dvd_menu" )->plug( &popup ),
      popup.insertSeparator();
   if( !((KToggleAction*)actionCollection()->action( "fullscreen" ))->isChecked() )
      action( "reset_zoom" )->plug( &popup );
   action( "capture_frame" )->plug( &popup );
   popup.insertSeparator();
   action( "video_settings" )->plug( &popup );
   popup.insertSeparator();
   action( "fullscreen" )->plug( &popup );
   //show zoom information?

   if( e->state() & Qt::MetaButton ) { //only on track end, or for special users
      popup.insertSeparator();
      action( "file_quit" )->plug( &popup );
   }

   if( popup.exec( e->globalPos() ) == 1 && state() == Engine::Playing )
      // we check we are still paused as the menu generates a modal event loop
      // so anything might have happened in the meantime.
      pause();
}

bool
VideoWindow::event( QEvent *e )
{
   //TODO it would perhaps make things more responsive to
   // deactivate mouse tracking and use the x11Event() function to transfer mouse move events?
   // perhaps even better would be a x11 implementation

   switch( e->type() )
   {
      case QEvent::DragEnter:
      case QEvent::Drop:
         //FIXME why don't we just ignore the event? It should propogate down
         return QApplication::sendEvent( qApp->mainWidget(), e );

      case QEvent::Resize:
         if( !TheStream::url().isEmpty() ) {
            const QSize defaultSize = TheStream::defaultVideoSize();
            const bool notDefaultSize = width() != defaultSize.width() && height() != defaultSize.height();

            Codeine::action( "reset_zoom" )->setEnabled( notDefaultSize );

            //showOSD( i18n("Scale: %1%").arg( size()
         }
         break;

      case QEvent::Leave:
         m_timer.stop();
         break;

      // Xlib.h sucks fucking balls!!!!11!!1!
      #undef FocusOut
      case QEvent::FocusOut:
         // if the user summons some dialog via a shortcut or whatever we need to ensure
         // the mouse gets shown, because if it is modal, we won't get mouse events after
         // it is shown! This works because we are always the focus widget.
         // @see MainWindow::MainWindow where we setFocusProxy()
      case QEvent::Enter:
      case QEvent::MouseMove:
      case QEvent::MouseButtonPress:
         unsetCursor();
         if( hasFocus() )
            // see above comment
            m_timer.start( CURSOR_HIDE_TIMEOUT, true );
         break;

      case QEvent::MouseButtonDblClick:
         Codeine::action( "fullscreen" )->activate();
         break;

      default: ;
   }

   if( !m_xine )
      return QWidget::event( e );

   switch( e->type() )
   {
      case QEvent::Close:
         stop();
         return false;

      case VideoWindow::ExposeEvent:
         //see VideoWindow::x11Event()

         return true;

      // Xlib.h sucks fucking balls!!!!11!!1!
      #undef KeyPress
      case QEvent::KeyPress: {
         if( m_url.protocol() != "dvd" )
            // let MainWindow handle this
            return QWidget::event( e );

         //FIXME left and right keys don't work during DVDs

         int keyCode = XINE_EVENT_INPUT_UP;

         //#define XINE_EVENT_INPUT_UP             110
         //#define XINE_EVENT_INPUT_DOWN           111
         //#define XINE_EVENT_INPUT_LEFT           112
         //#define XINE_EVENT_INPUT_RIGHT          113
         //#define XINE_EVENT_INPUT_SELECT         114

         switch( static_cast<QKeyEvent*>(e)->key() ) {
         case Key_Return:
         case Key_Enter: keyCode++;
         case Key_Right: keyCode++;
         case Key_Left:  keyCode++;
         case Key_Down:  keyCode++;
         case Key_Up:
         {
            //this whole shebang is cheeky as xine doesn't
            //guarentee the codes will stay the same

            xine_event_t xineEvent;

            xineEvent.type = keyCode;
            xineEvent.data = NULL;
            xineEvent.data_length = 0;

            xine_event_send( m_stream, &xineEvent );

            return true;
         }
         default:
            return false;
         }
      }

      case QEvent::MouseButtonPress:

         #define mouseEvent static_cast<QMouseEvent*>(e)

         if( mouseEvent->button() != Qt::LeftButton )
            return false;

         mouseEvent->accept();

         //FALL THROUGH

      case QEvent::MouseMove:
      {
         x11_rectangle_t   x11Rect;
         xine_event_t      xineEvent;
         xine_input_data_t xineInput;

         x11Rect.x = mouseEvent->x();
         x11Rect.y = mouseEvent->y();
         x11Rect.w = 0;
         x11Rect.h = 0;

         xine_gui_send_vo_data( m_stream, XINE_GUI_SEND_TRANSLATE_GUI_TO_VIDEO, (void*)&x11Rect );

         xineEvent.type        = e->type() == QEvent::MouseMove ? XINE_EVENT_INPUT_MOUSE_MOVE : XINE_EVENT_INPUT_MOUSE_BUTTON;
         xineEvent.data        = &xineInput;
         xineEvent.data_length = sizeof( xine_input_data_t );
         xineInput.button      = 1; //HACK e->type() == QEvent::MouseMove ? 0 : 1;
         xineInput.x           = x11Rect.x;
         xineInput.y           = x11Rect.y;
         xine_event_send( m_stream, &xineEvent );

         return e->type() == QEvent::MouseMove ? false : true;

         #undef mouseEvent
      }

      case QEvent::Wheel:
      {
         //TODO seek amount should depend on the length, basically seek at most say 30s, and at least 0.5s
         //TODO this is replicated (somewhat) in MainWindow::keyPressEvent

         int pos, time, length;
         xine_get_pos_length( m_stream, &pos, &time, &length );
         pos += int(std::log10( (double)length ) * static_cast<QWheelEvent*>(e)->delta());

         seek( pos > 0 ? (uint)pos : 0 );

         return true;
      }

      default: ;
   }

   return QWidget::event( e );
}

bool
VideoWindow::x11Event( XEvent *e )
{
   if( m_stream && e->type == Expose && e->xexpose.count == 0 ) {
      xine_gui_send_vo_data(
            m_stream,
            XINE_GUI_SEND_EXPOSE_EVENT,
            e );

      return true;
   }

   return false;
}

void
VideoWindow::hideCursor()
{
   setCursor( Qt::BlankCursor );
}

QSize
VideoWindow::sizeHint() const //virtual
{
   QSize s = TheStream::profile()->readSizeEntry( "Preferred Size" );

   if( !s.isValid() )
      s = TheStream::defaultVideoSize();

   if( s.isValid() && !s.isNull() )
      return s;

   return minimumSizeHint();
}

QSize
VideoWindow::minimumSizeHint() const //virtual
{
   const int x = fontMetrics().width( "x" ) * 4;

   return QSize( x * 12, x * 4 ); //FIXME
}

void
VideoWindow::resetZoom()
{
   TheStream::profile()->deleteEntry( "Preferred Size" );
   topLevelWidget()->adjustSize();
}

} //namespace Codeine
