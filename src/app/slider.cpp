// (c) 2004 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include "debug.h"
#include "slider.h"
#include <qapplication.h>
#include <qlabel.h>
#include <qsize.h>
#include <qtooltip.h>

#include <qpainter.h>
#include "xineEngine.h"

using Codeine::Slider;


Slider *Slider::s_instance = 0;


Slider::Slider( QWidget *parent, uint max )
      : QSlider( Qt::Horizontal, parent )
      , m_sliding( false )
      , m_outside( false )
      , m_prevValue( 0 )
{
   s_instance = this;

   setRange( 0, max );
   setFocusPolicy( NoFocus );
   setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
}

void
Slider::wheelEvent( QWheelEvent *e )
{
   //if you use this class elsewhere, NOTE this is Codeine specific
   e->ignore(); //pass to VideoWindow
}

void
Slider::mouseMoveEvent( QMouseEvent *e )
{
   if( m_sliding )
   {
      //feels better, but using set value of 20 is bad of course
      QRect rect = this->rect();
      rect.addCoords( -20, -20, 20, 20 );

      if( !rect.contains( e->pos() ) ) {
         if( !m_outside )
            QSlider::setValue( m_prevValue );
         m_outside = true;
      } else {
         m_outside = false;

         QSlider::setValue(
               QRangeControl::valueFromPosition(
                     e->pos().x() - sliderRect().width()/2,
                     width()  - sliderRect().width() ) );

         emit sliderMoved( value() );
      }
   }
   else
      QSlider::mouseMoveEvent( e );
}

void
Slider::mousePressEvent( QMouseEvent *e )
{
   m_sliding   = true;
   m_prevValue = QSlider::value();

   if( !sliderRect().contains( e->pos() ) )
      mouseMoveEvent( e );
}

void
Slider::mouseReleaseEvent( QMouseEvent* )
{
   if( !m_outside && QSlider::value() != m_prevValue )
      emit sliderReleased( value() );

   m_sliding = false;
   m_outside = false;
}

static inline QString timeAsString( const int s )
{
   #define zeroPad( n ) n < 10 ? QString("0%1").arg( n ) : QString::number( n )
   using Codeine::engine;

   const int m  =  s / 60;
   const int h  =  m / 60;

   QString time;
   time.prepend( zeroPad( s % 60 ) ); //seconds
   time.prepend( ':' );
   time.prepend( zeroPad( m % 60 ) ); //minutes
   time.prepend( ':' );
   time.prepend( QString::number( h ) ); //hours

   return time;
}

void
Slider::setValue( int newValue )
{
   static QLabel *w1 = 0;
   static QLabel *w2 = 0;

   if (!w1) {
      w1 = new QLabel( this );
      w1->setPalette( QToolTip::palette() );
      w1->setFrameStyle( QFrame::Plain | QFrame::Box );

      w2 = new QLabel( this );
      w2->setPalette( QToolTip::palette() );
      w2->setFrameStyle( QFrame::Plain | QFrame::Box );
   }

   //TODO stupidly inefficeint! :)
   w1->setShown( mainWindow()->isFullScreen() );
   w2->setShown( mainWindow()->isFullScreen() );


   //don't adjust the slider while the user is dragging it!

   if( !m_sliding || m_outside ) {
      const int l     = engine()->length() / 1000;
      const int left  = int(l * (newValue / 65535.0));
      const int right = l - left;

      QSlider::setValue( newValue );
      w1->move( 0, height() - w1->height() - 1 );
      w1->setText( timeAsString( left ) + ' ' );
      w1->adjustSize();

      w2->move( width() - w2->width(), height() - w1->height() - 1 );
      w2->setText( timeAsString( right ) + ' ' );
      w2->adjustSize();
   }
   else
      m_prevValue = newValue;
}
