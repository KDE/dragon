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

#include "debug.h"
#include "videoWindow.h"
#include "mainWindow.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPoint>

#include <KToolBar>
#include <KMainWindow>


Dragon::FullScreenToolBarHandler::FullScreenToolBarHandler( KMainWindow *parent )
        : QObject( parent )
        , m_toolbar( parent->toolBar() )
        , m_timer_id( 0 )
        , m_stay_hidden_for_a_bit( false )
{
    DEBUG_BLOCK

    parent->installEventFilter( this );
    m_toolbar->installEventFilter( this );
}

bool 
Dragon::FullScreenToolBarHandler::eventFilter( QObject *o, QEvent *e )
{
    if (o == parent() && e->type() == QEvent::MouseMove) {
        DEBUG_FUNC_INFO
        debug() << "mouse move, killing timer";
        killTimer( m_timer_id );

        QMouseEvent const * const me = (QMouseEvent*)e;
        if (m_stay_hidden_for_a_bit) {
            debug() << "staying hidden for a bit";
            // wait for a small pause before showing the toolbar again
            // usage = user removes mouse from toolbar after using it
            // toolbar disappears (usage is over) but usually we show
            // toolbar immediately when mouse is moved.. so we need this hack

            if (m_toolbar->geometry().contains(me->pos()))
                goto show_toolbar;

            m_timer_id = startTimer( 100 );
        }
        else {
            if( m_toolbar->isHidden() ) {
                debug() << "toolbar is hidden ";
                if( m_home.isNull() )
                {
                    debug() << "set home";
                    m_home = me->pos();
                }
                else if( ( m_home - me->pos() ).manhattanLength() > 6)
                {
                    // then cursor has moved far enough to trigger show toolbar
show_toolbar:
                    debug() << "show toolbar";
                    m_toolbar->show(),
                    static_cast<Dragon::MainWindow*>( Dragon::mainWindow() )->showVolume( true );
                    m_home = QPoint();
                }
                else
                {
                    debug() << "cursor hasn't moved far enough yet " << ( m_home - me->pos() ).manhattanLength();
                    // cursor hasn't moved far enough yet
                    // don't reset timer below, return instead
                    return false;
                }
            }

            // reset the hide timer
            m_timer_id = startTimer( Dragon::VideoWindow::CURSOR_HIDE_TIMEOUT );
        }
    }

    if (o == parent() && e->type() == QEvent::Resize)
    {
        //we aren't managed by mainWindow when at FullScreen
        videoWindow()->move( 0, 0 );
        videoWindow()->resize( ((QWidget*)o)->size() );
        videoWindow()->lower();
    }

    if (o == m_toolbar)
        switch (e->type()) {
            case QEvent::Enter:
                m_stay_hidden_for_a_bit = false;
                killTimer( m_timer_id );
            break;

            case QEvent::Leave:
                static_cast<Dragon::MainWindow*>( Dragon::mainWindow() )->showVolume( false );
                m_toolbar->hide();
                m_stay_hidden_for_a_bit = true;
                killTimer( m_timer_id );
                m_timer_id = startTimer( 100 );
            break;

            default: break;
        }

    return false;
}

void 
Dragon::FullScreenToolBarHandler::timerEvent( QTimerEvent* )
{
    if (m_stay_hidden_for_a_bit)
        ;

    else if ( videoWindow()->mouseUnderWidget() ){
        static_cast<Dragon::MainWindow*>( Dragon::mainWindow() )->showVolume( false );
        m_toolbar->hide();
        }

    m_stay_hidden_for_a_bit = false;
}

#include "fullScreenToolBarHandler.moc"
