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

#include "debug.h"
#include "extern.h"
#include "fullScreenAction.h"
#include "videoWindow.h" //videoWindow()

#include <KActionCollection>
#include <KMainWindow>
#include <KLocale>
#include <KToggleFullScreenAction>
#include <QEvent>
#include <QWidget>

FullScreenAction::FullScreenAction( QWidget* window, KActionCollection *parent )
        : KToggleAction( parent )
        //, m_window( window )
        //, m_shouldBeDisabled( false )
        //, m_state( 0 )
{
 //KToggleAction( QString::null, Qt::Key_F, 0, 0, parent, "fullscreen" )
    setObjectName( "fullscreen" );
    setShortcut( Qt::Key_F );
    parent->addAction( objectName(), this );
    window->installEventFilter( this );
    setChecked( false );
    setText( i18n("F&ull Screen Mode") );
    setIcon( KIcon("view-fullscreen") );
    setCheckedState( KGuiItem( i18n("Exit F&ull Screen Mode"), KIcon("window_nofullscreen") ) );
    connect( this, SIGNAL( toggled( bool ) ), Codeine::mainWindow(), SLOT( setFullScreen( bool ) ) );
}



/*
void
FullScreenAction::setEnabled( bool setEnabled )
{
    if( setEnabled == false && isChecked() )
        // don't disable the action if we are currently in fullscreen mode
        // as then the user can't exit fullscreen mode! Instead disable it
        // when we next get toggled out of fullscreen mode
        m_shouldBeDisabled = true;

    else {
        //FIXME Codeine specific (because videoWindow isn't the window we control, we control the KMainWindow)
        //NOTE also if the videoWindow is hidden at some point, this is broken..
        //TODO new type of actionclass that event filters and is always correct state
        if( setEnabled && reinterpret_cast<QWidget*>(Codeine::videoWindow())->isHidden() )
            setEnabled = false;

        m_shouldBeDisabled = false;
        KToggleAction::setEnabled( setEnabled );
    }
}

bool
FullScreenAction::eventFilter( QObject *o, QEvent *e )
{
    if( o == m_window )
        if(e->type() == QEvent::WindowStateChange)
        {
            if (m_window->isFullScreen() != isChecked())
                slotToggled( m_window->isFullScreen() ); // setChecked( window->isFullScreen()) wouldn't emit signals

            if (m_window->isFullScreen() && !isEnabled()) {
                m_shouldBeDisabled = true;
                setEnabled( true );
            }
        }
    return false;
}
 */
#include "fullScreenAction.moc"
