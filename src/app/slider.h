// (c) 2004 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINESLIDER_H
#define CODEINESLIDER_H

#include <qslider.h>

namespace Codeine
{
   class Slider : public QSlider
   {
   Q_OBJECT

   public:
      static Slider *instance() { return s_instance; }

   public:
      Slider( QWidget*, uint max = 0 );

      virtual void setValue( int );

   signals:
      //we emit this when the user has specifically changed the slider
      //so connect to it if valueChanged() is too generic
      //Qt also emits valueChanged( int )
      void sliderReleased( uint );

   protected:
      virtual void wheelEvent( QWheelEvent* );
      virtual void mouseMoveEvent( QMouseEvent* );
      virtual void mouseReleaseEvent( QMouseEvent* );
      virtual void mousePressEvent( QMouseEvent* );
      virtual void keyPressEvent( QKeyEvent *e ) { e->ignore(); } //so that MainWindow gets the keypress

      virtual QSize sizeHint() const { return QSlider::sizeHint() + QSize( 0, 6 ); }
      virtual QSize minimumSizeHint() const { return sizeHint(); }

      bool m_sliding;

   private:
      static Slider *s_instance;

      bool m_outside;
      int  m_prevValue;

      Slider( const Slider& ); //undefined
      Slider &operator=( const Slider& ); //undefined
   };
}

#endif
