// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include <kwin.h>
#include "mxcl.library.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qslider.h>
#include "videoSettings.h"
#include <xine.h>
#include "xineEngine.h"

extern "C"
{
   // #include <X11/Xlib.h> is just dangerous! Here, there is a macro for Below that conflicts
   // with QSlider::Below. Stupid X11 people.
   typedef unsigned long XID;
   typedef XID Window;
   extern int XSetTransientForHint( Display*, Window, Window );
}


//TODO update from engine when new video is played
//TODO show a warning that when paused the changes aren't updated to the display, show an unpause button too


class SnapSlider : public QSlider
{
   int m_offset;

public:
   SnapSlider( const int value, QWidget *parent, const char *name )
         : QSlider( (65536/4)-1, (3*(65536/4))-1, 1000, value, Qt::Horizontal, parent, name )
         , m_offset( 0 )
   {
      setTickmarks( QSlider::Below );
      setTickInterval( 65536 / 4 );
      setMinimumWidth( fontMetrics().width( name ) * 3 );
      connect( this, SIGNAL(valueChanged( int )), Codeine::engine(), SLOT(setStreamParameter( int )) );
   }

   virtual void mousePressEvent( QMouseEvent *e )
   {
      m_offset = e->pos().x() - (sliderStart() + (sliderRect().width()/2));
      QSlider::mousePressEvent( e );
   }

   virtual void mouseMoveEvent( QMouseEvent *e )
   {
      const int MIDDLE = width() / 2;
      const int x = e->pos().x() - m_offset;
      const int F = sliderRect().width() / 2;

      if( x > MIDDLE - F && x < MIDDLE + F ) {
         QMouseEvent e2( e->type(), QPoint( MIDDLE + m_offset, e->pos().y() ), e->button(), e->state() );
         QSlider::mouseMoveEvent( &e2 );
         QRangeControl::setValue( 65536 / 2 - 1 ); // to ensure we are absolutely exact
      }
      else
         QSlider::mouseMoveEvent( e );
   }
};


Codeine::VideoSettingsDialog::VideoSettingsDialog( QWidget *parent )
      : KDialog( parent, "video_settings_dialog", false, WType_TopLevel | WDestructiveClose )
{
   XSetTransientForHint( x11Display(), winId(), parent->winId() );
   KWin::setType( winId(), NET::Utility );
   KWin::setState( winId(), NET::SkipTaskbar );

   QFrame *frame = new QFrame( this );
   (new QVBoxLayout( this, 10 ))->addWidget( frame );
   frame->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
   frame->setPaletteBackgroundColor( backgroundColor().dark( 102 ) );

   QGridLayout *grid = new QGridLayout( frame, 4, 2, 15, 10 );
   grid->setAutoAdd( true );

   #define makeSlider( PARAM, name ) \
            new QLabel( name, frame ); \
            new SnapSlider( xine_get_param( *Codeine::engine(), PARAM ), frame, name );

   makeSlider( XINE_PARAM_VO_BRIGHTNESS, "brightness" );
   makeSlider( XINE_PARAM_VO_CONTRAST, "contrast" );
   makeSlider( XINE_PARAM_VO_SATURATION, "saturation" );
   makeSlider( XINE_PARAM_VO_HUE, "hue" );

   #undef makeSlider

   setCaption( i18n("Video Settings") );
   setMaximumSize( sizeHint().width() * 5, sizeHint().height() );

   KDialog::show();
}

void
Codeine::VideoSettingsDialog::stateChanged( QWidget *parent, Engine::State state ) //static
{
   QWidget *me = (QWidget*)parent->child( "video_settings_dialog" );

   if( !me )
      return;

   switch( state )
   {
   case Engine::Playing:
   case Engine::Paused:
      me->setEnabled( true );
      break;

   case Engine::Loaded:
      #define update( param, name ) static_cast<QSlider*>(me->child( name ))->setValue( xine_get_param( *Codeine::engine(), param ) );
      update( XINE_PARAM_VO_BRIGHTNESS, "brightness" );
      update( XINE_PARAM_VO_CONTRAST, "contrast" );
      update( XINE_PARAM_VO_SATURATION, "saturation" );
      update( XINE_PARAM_VO_HUE, "hue" );
      #undef update

   default:
      me->setEnabled( false );
      break;
   }
}

namespace Codeine
{
   void showVideoSettingsDialog( QWidget *parent )
   {
      // ensure that the dialog is shown by deleting the old one
      delete parent->child( "video_settings_dialog" );

      new VideoSettingsDialog( parent );
   }
}
