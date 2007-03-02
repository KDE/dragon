// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include "debug.h"
#include "extern.h"
#include "fullScreenAction.h"
#include "videoWindow.h" //videoWindow()

#include <kactioncollection.h>
#include <kmainwindow.h>
#include <klocale.h>
#include <ktogglefullscreenaction.h>
#include <kwin.h>
#include <QEvent>
#include <qwidget.h>

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
    setIcon( KIcon("window_fullscreen") );
    setCheckedState( KGuiItem( i18n("Exit F&ull Screen Mode"), KIcon("window_nofullscreen") ) );
    connect( this, SIGNAL( toggled( bool ) ), Codeine::videoWindow(), SLOT( setFullScreen( bool ) ) );
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
