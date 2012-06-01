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

#include "fullScreenToolBarHandler.h"

#include "videoWindow.h"
#include "mainWindow.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPoint>

#include <KDebug>
#include <KToolBar>
#include <KMainWindow>

Dragon::FullScreenToolBarHandler::FullScreenToolBarHandler( KMainWindow *parent )
        : QObject( parent )
        , m_timer_id( 0 )
        , m_parent(parent)
{
    parent->installEventFilter( this );

    startTimer( Dragon::VideoWindow::CURSOR_HIDE_TIMEOUT ); // We want to hide automatically some time after fullscreening
}

bool 
Dragon::FullScreenToolBarHandler::eventFilter( QObject */*o*/, QEvent *e )
{
    if (e->type() == QEvent::MouseMove) {
        if (m_timer_id) {
            kDebug() << "mouse move, killing timer";
            killTimer( m_timer_id );
            m_timer_id = 0;
        }

        QMouseEvent const * const me = (QMouseEvent*)e;

        if (m_parent->toolBar()->geometry().contains(me->pos()) ||
            static_cast<Dragon::MainWindow*>( Dragon::mainWindow() )->volumeContains(me->pos())) {
            // no discussion here, mouse is in toolbar or volume slider area
            kDebug() << "mouse in toolbar area, show toolbar";
            m_parent->toolBar()->show();
            static_cast<Dragon::MainWindow*>( Dragon::mainWindow() )->showVolume( true );
        }
        else if( m_parent->toolBar()->isHidden() ) {
            kDebug() << "mouse moved while toolbar is hidden";
            if( m_home.isNull() )
            {
                kDebug() << "set home";
                m_home = me->pos(); // store the position where the mouse was when we saw it
            }
            else if( ( m_home - me->pos() ).manhattanLength() > 6)
            {
                // then cursor has moved far enough to trigger show toolbar
                kDebug() << "show toolbar";
                m_parent->toolBar()->show();
                static_cast<Dragon::MainWindow*>( Dragon::mainWindow() )->showVolume( true );
                m_home = QPoint();
            }
            else
            {
                kDebug() << "cursor hasn't moved far enough yet " << ( m_home - me->pos() ).manhattanLength();
                // cursor hasn't moved far enough yet
            }
        }
        else {
            // reset the hide timer
            kDebug() << "mouse moved in video window while toolbar is shown, starting hide timer: " << Dragon::VideoWindow::CURSOR_HIDE_TIMEOUT;
            m_timer_id = startTimer( Dragon::VideoWindow::CURSOR_HIDE_TIMEOUT );
        }
    }

    else if (e->type() == QEvent::Resize)
    {
        //we aren't managed by mainWindow when at FullScreen
        videoWindow()->move( 0, 0 );
        videoWindow()->resize( ((QWidget*)parent())->size() );
        videoWindow()->lower();
    }

    return false;
}

void 
Dragon::FullScreenToolBarHandler::timerEvent( QTimerEvent*e )
{
    killTimer( e->timerId() ); // timers are NOT single-shot!
    m_timer_id = 0;

    kDebug() << "hide timer triggered";
    static_cast<Dragon::MainWindow*>( Dragon::mainWindow() )->showVolume( false );
    m_parent->toolBar()->hide();
}

#include "fullScreenToolBarHandler.moc"
