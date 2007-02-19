// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include <kguiitem.h>
#include <kpushbutton.h>

#include <Q3Frame>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <qapplication.h>
#include <qevent.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <QTimerEvent>

#include "adjustSizeButton.h"
#include "extern.h"
#include "theStream.h"
#include "xineEngine.h" //videoWindow()


QString i18n( const char *text );

namespace Codeine
{
   AdjustSizeButton::AdjustSizeButton( QWidget *parent )
         : Q3Frame( parent )
         , m_counter( 0 )
         , m_stage( 1 )
         , m_offset( 0 )
   {
      parent->installEventFilter( this );

      setPalette( QApplication::palette() ); //videoWindow has different palette
      setFrameStyle( Q3Frame::Plain | Q3Frame::Box );

      m_preferred = new KPushButton( KGuiItem( i18n("Preferred Scale"), "viewmag" ), this );
      connect( m_preferred, SIGNAL(clicked()), qApp->mainWidget(), SLOT(adjustSize()) );
      connect( m_preferred, SIGNAL(clicked()), SLOT(deleteLater()) );

      m_oneToOne = new KPushButton( KGuiItem( i18n("Scale 100%"), "viewmag1" ), this );
      connect( m_oneToOne, SIGNAL(clicked()), (QObject*)videoWindow(), SLOT(resetZoom()) );
      connect( m_oneToOne, SIGNAL(clicked()), SLOT(deleteLater()) );

      Q3BoxLayout *hbox = new Q3HBoxLayout( this, 8, 6 );
      Q3BoxLayout *vbox = new Q3VBoxLayout( hbox );
      vbox->addWidget( new QLabel( i18n( "<b>Adjust video scale?" ), this ) );
      vbox->addWidget( m_preferred );
      vbox->addWidget( m_oneToOne );
      hbox->addWidget( m_thingy = new Q3Frame( this ) );

      m_thingy->setFixedWidth( fontMetrics().width( "X" ) );
      m_thingy->setFrameStyle( Q3Frame::Plain | Q3Frame::Box );
      m_thingy->setPaletteForegroundColor( paletteBackgroundColor().dark() );

      QEvent e( QEvent::Resize );
      eventFilter( 0, &e );

      adjustSize();
      show();

      m_timerId = startTimer( 5 );
   }

   void
   AdjustSizeButton::timerEvent( QTimerEvent* )
   {
      Q3Frame *&h = m_thingy;

      switch( m_stage )
      {
      case 1: //raise
         move();
         m_offset++;

         if( m_offset > height() )
            killTimer( m_timerId ),
            m_timerId = startTimer( 40 ),
            m_stage = 2;

         break;

      case 2: //fill in pause timer bar
         if( m_counter < h->height() - 3 )
            QPainter( h ).fillRect( 2, 2, h->width() - 4, m_counter, palette().active().highlight() );

         if( !hasMouse() )
            m_counter++;

         if( m_counter > h->height() + 5 ) //pause for 360ms before lowering
            m_stage = 3,
            killTimer( m_timerId ),
            m_timerId = startTimer( 6 );

         break;

      case 3: //lower
         if( hasMouse() ) {
            m_stage = 1;
            m_counter = 0;
            m_thingy->repaint();
            break; }

         m_offset--;
         move();

         if( m_offset < 0 )
            deleteLater();
      }
   }

   bool
   AdjustSizeButton::eventFilter( QObject *o, QEvent *e )
   {
      if( e->type() == QEvent::Resize ) {
         const QSize preferredSize = TheStream::profile()->readSizeEntry( "Preferred Size" );
         const QSize defaultSize = TheStream::defaultVideoSize();
         const QSize parentSize = parentWidget()->size();

         m_preferred->setEnabled( preferredSize.isValid() && parentSize != preferredSize && defaultSize != preferredSize );
         m_oneToOne->setEnabled( defaultSize != parentSize );

         move();

         if( !m_preferred->isEnabled() && !m_oneToOne->isEnabled() && m_counter == 0 )
            deleteLater();
      }

      return false;
   }
}
