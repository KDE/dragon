/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "fullScreenToolBarHandler.h"

#include "videoWindow.h"
#include "mainWindow.h"

#include <QEvent>
#include <QMouseEvent>
#include <QDebug>

#include <KToolBar>
#include <KMainWindow>

Dragon::FullScreenToolBarHandler::FullScreenToolBarHandler( KMainWindow *parent )
    : QObject( parent )
    , m_timer_id( 0 )
    , m_parent(parent)
{
    parent->installEventFilter( this );

    m_timer_id = startTimer( Dragon::VideoWindow::CURSOR_HIDE_TIMEOUT ); // We want to hide automatically some time after fullscreening
}

bool Dragon::FullScreenToolBarHandler::eventFilter( QObject */*o*/, QEvent *e )
{
    if (e->type() == QEvent::MouseMove) {
        if (m_timer_id) {
            qDebug() << "mouse move, killing timer";
            killTimer( m_timer_id );
            m_timer_id = 0;
        }

        QMouseEvent const * const me = (QMouseEvent*)e;

        if (m_parent->toolBar()->geometry().contains(me->pos()) ||
                static_cast<Dragon::MainWindow*>( Dragon::mainWindow() )->volumeContains(me->pos())) {
            // no discussion here, mouse is in toolbar or volume slider area
            qDebug() << "mouse in toolbar area, show toolbar";
            m_parent->toolBar()->show();
            static_cast<Dragon::MainWindow*>( Dragon::mainWindow() )->showVolume( true );
        } else if( m_parent->toolBar()->isHidden() ) {
            qDebug() << "mouse moved while toolbar is hidden";
            if( m_home.isNull() ) {
                qDebug() << "set home";
                m_home = me->pos(); // store the position where the mouse was when we saw it
            } else if( ( m_home - me->pos() ).manhattanLength() > 6) {
                // then cursor has moved far enough to trigger show toolbar
                qDebug() << "show toolbar";
                m_parent->toolBar()->show();
                static_cast<Dragon::MainWindow*>( Dragon::mainWindow() )->showVolume( true );
                m_home = QPoint();
            } else {
                qDebug() << "cursor hasn't moved far enough yet " << ( m_home - me->pos() ).manhattanLength();
                // cursor hasn't moved far enough yet
            }
        } else {
            // reset the hide timer
            qDebug() << "mouse moved in video window while toolbar is shown, starting hide timer: " << Dragon::VideoWindow::CURSOR_HIDE_TIMEOUT;
            m_timer_id = startTimer( Dragon::VideoWindow::CURSOR_HIDE_TIMEOUT );
        }
    } else if (e->type() == QEvent::Resize) {
        //we aren't managed by mainWindow when at FullScreen
        videoWindow()->move( 0, 0 );
        videoWindow()->resize( ((QWidget*)parent())->size() );
        videoWindow()->lower();
    } else if (e->type() == QEvent::Leave) {
        // reset the hide timer
        m_timer_id = startTimer( Dragon::VideoWindow::CURSOR_HIDE_TIMEOUT );
    }

    return false;
}

void
Dragon::FullScreenToolBarHandler::timerEvent( QTimerEvent*e )
{
    killTimer( e->timerId() ); // timers are NOT single-shot!
    m_timer_id = 0;

    qDebug() << "hide timer triggered";
    static_cast<Dragon::MainWindow*>( Dragon::mainWindow() )->showVolume( false );
    m_parent->toolBar()->hide();
}
