/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

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
