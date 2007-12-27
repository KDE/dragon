/***********************************************************************
 * Copyright 2005  Max Howell <max.howell@methylblue.com>
 *           2007  Ian Monroe <ian@monroe.nu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy 
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************/

#include "partToolBar.h"

#include <QApplication>
#include <QEvent>
#include <QResizeEvent>

MouseOverToolBar::MouseOverToolBar( QWidget *parent )
      : KToolBar( parent )
{
   parent->installEventFilter( this );
 //  move( 0, 0 ); //TODO necessary?
   hide();

   setPalette( QApplication::palette() ); //videoWindow palette has a black background
}

bool
MouseOverToolBar::eventFilter( QObject */*o*/, QEvent *e )
{
   switch( e->type() )
   {
   /*case QEvent::Resize:
      resize( static_cast<QResizeEvent*>(e)->size().width(), sizeHint().height() );
      break;*/

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
