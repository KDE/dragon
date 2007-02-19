// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include <klocale.h>
#include <ktoolbar.h>
#include <qevent.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qslider.h>

#include "debug.h"
#include "volumeAction.h"
#include "volumeAction.moc"
#include "xineEngine.h"


class VolumeSlider : public QFrame
{
public:
   VolumeSlider( QWidget *parent )
         : QFrame( parent )
   {
      slider = new QSlider( Qt::Vertical, this, "volume" );
      label = new QLabel( this );

      QBoxLayout *lay = new QVBoxLayout( this );
      lay->addWidget( slider, 0, Qt::AlignHCenter );
      lay->addWidget( label, 0, Qt::AlignHCenter );
      lay->setMargin( 4 );

      slider->setRange( 0, 100 );

      setFrameStyle( QFrame::Plain | QFrame::Box );
      setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );

      hide();
   }

   QLabel *label;
   QSlider *slider;
};


VolumeAction::VolumeAction( KToolBar *bar, KActionCollection *ac )
      : KToggleAction( i18n("Volume"), "volume", Qt::Key_1, 0, 0, ac, "volume" )
      , m_anchor( 0 )
{
   m_widget = new VolumeSlider( bar->topLevelWidget() );

   connect( this, SIGNAL(toggled( bool )), SLOT(toggled( bool )) );
   connect( m_widget->slider, SIGNAL(sliderMoved( int )), SLOT(sliderMoved( int )) );
   connect( m_widget->slider, SIGNAL(sliderMoved( int )), Codeine::engine(), SLOT(setStreamParameter( int )) );
   connect( m_widget->slider, SIGNAL(sliderReleased()), SLOT(sliderReleased()) );
}

int
VolumeAction::plug( QWidget *bar, int index )
{
   DEBUG_BLOCK

   int const id = KAction::plug( bar, index );

   m_anchor = (QWidget*)bar->child( "toolbutton_volume" ); //KAction creates it with this name
   m_anchor->installEventFilter( this ); //so we can keep m_widget anchored

   return id;
}

void
VolumeAction::toggled( bool const b )
{
   DEBUG_BLOCK

   m_widget->raise();
   m_widget->setShown( b );
}

void
VolumeAction::sliderMoved( int v )
{
   v = 100 - v; //Qt sliders are wrong way round when vertical

   QString const t = QString::number( v ) + '%';

   setToolTip( i18n( "Volume: %1" ).arg( t ) );
   m_widget->label->setText( t );
}

bool
VolumeAction::eventFilter( QObject *o, QEvent *e )
{
   switch (e->type()) {
      case QEvent::Move:
      case QEvent::Resize: {
         QWidget const * const &a = m_anchor;

         m_widget->move( a->mapTo( m_widget->parentWidget(), QPoint( 0, a->height() ) ) );
         m_widget->resize( a->width(), m_widget->sizeHint().height() );
         return false;
      }

      //TODO one click method, flawed currently in fullscreen mode by palette change in mainwindow.cpp
/*      case QEvent::MouseButtonPress:
         m_widget->show();
         break;

      case QEvent::MouseButtonRelease:
         m_widget->hide();
         break;*/

      default:
         return false;
   }
}
