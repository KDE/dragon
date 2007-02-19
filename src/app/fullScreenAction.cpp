// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include "extern.h"
#include "fullScreenAction.h"
#include "xineEngine.h" //videoWindow()

#include <kactioncollection.h>
#include <klocale.h>
#include <kwin.h>
#include <QEvent>
#include <qwidget.h>

FullScreenAction::FullScreenAction( QWidget* window, KActionCollection *parent )
        : KToggleAction( QString::null, parent )
        , m_window( window )
        , m_shouldBeDisabled( false )
        , m_state( 0 )
{
 //KToggleAction( QString::null, Qt::Key_F, 0, 0, parent, "fullscreen" )
    setObjectName( "fullscreen" );
    setShortcut( Qt::Key_F );
    window->installEventFilter( this );
    setChecked( false );
}

void
FullScreenAction::setChecked( bool setChecked )
{
    KToggleAction::setChecked( setChecked );

    m_window->raise();

    const int id = m_window->winId();
    if( setChecked ) {
        setText( i18n("Exit F&ull Screen Mode") );
        setIcon( KIcon("window_nofullscreen") );
        m_state = KWin::windowInfo( id, NET::WMState, 0 ).state();
        KWin::setState( id, NET::FullScreen );
    }
    else {
        setText(i18n("F&ull Screen Mode"));
        setIcon( KIcon("window_fullscreen") );
        KWin::clearState( id, NET::FullScreen );
        KWin::setState( id, m_state ); // get round bug in KWin where it forgets maximisation state
    }

    if( setChecked == false && m_shouldBeDisabled )
        setEnabled( false );
}

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

/**
 * 
 * @param o 
 * @param e 
 * @return 
 */
bool
FullScreenAction::eventFilter( QObject *o, QEvent *e )
{
    if( o == m_window )
        switch( e->type() ) {
            #if QT_VERSION >= 0x030300
            case QEvent::WindowStateChange:
            #else
            case QEvent::ShowFullScreen:
            case QEvent::ShowNormal:
            case QEvent::ShowMaximized:
            case QEvent::ShowMinimized:
            #endif
                if (m_window->isFullScreen() != isChecked())
                    slotToggled( m_window->isFullScreen() ); // setChecked( window->isFullScreen()) wouldn't emit signals

                if (m_window->isFullScreen() && !isEnabled()) {
                    m_shouldBeDisabled = true;
                    setEnabled( true );
                }

                break;

            default:
                ;
        }

    return false;
}

#include "fullScreenAction.moc"