// (c) 2004 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef ANALYZER_H
#define ANALYZER_H

#ifdef __FreeBSD__
   #include <sys/types.h>
#endif

#include <qpixmap.h> //stack allocated and convenience
#include <qtimer.h>  //stack allocated
#include <qwidget.h> //baseclass
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <QEvent>
#include <vector>    //included for convenience

namespace Analyzer
{
   typedef std::vector<float> Scope;

   template<class W> class Base : public W
   {
   public:
      uint timeout() const { return m_timeout; }

   protected:
      Base( QWidget*, uint );

      virtual void transform( Scope& ) = 0;
      virtual void analyze( const Scope& ) = 0;

   private:
      virtual bool event( QEvent* );

   protected:
      QTimer m_timer;
      uint   m_timeout;
   };

   class Base2D : public Base<QWidget>
   {
   Q_OBJECT
   public:
      const QPixmap *canvas() const { return &m_canvas; }

   private slots:
      void draw();

   protected:
      Base2D( QWidget*, uint timeout );

      QPixmap *canvas() { return &m_canvas; }

      void paintEvent( QPaintEvent* ) { if( !m_canvas.isNull() ) bitBlt( this, 0, 0, canvas() ); }
      void resizeEvent( QResizeEvent* );

   private:
      QPixmap m_canvas;
   };

   class Block : public Analyzer::Base2D
   {
   public:
      Block( QWidget* );

   protected:
      virtual void transform( Analyzer::Scope& );
      virtual void analyze( const Analyzer::Scope& );

      virtual int heightForWidth( int ) const;

      virtual void show() {} //TODO temporary as the scope plugin causes freezes
   };
}

#endif
