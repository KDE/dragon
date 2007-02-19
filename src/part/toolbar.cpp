// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include <kpushbutton.h>
#include <qapplication.h>
#include <qevent.h>
//Added by qt3to4:
#include <QResizeEvent>
#include "toolbar.h"


MouseOverToolBar::MouseOverToolBar( QWidget *parent )
      : KToolBar( parent )
{
   parent->installEventFilter( this );
   move( 0, 0 ); //TODO necessary?
   hide();

   setPalette( QApplication::palette() ); //videoWindow palette has a black background
}

bool
MouseOverToolBar::eventFilter( QObject *o, QEvent *e )
{
   Q_ASSERT( o == parent() );

   switch( e->type() )
   {
   case QEvent::Resize:
      resize( static_cast<QResizeEvent*>(e)->size().width(), sizeHint().height() );
      break;

   case QEvent::Enter:
      show();
      break;

   case QEvent::Leave:
      hide();
      break;

   default:
      ;
   }

   return false;
}