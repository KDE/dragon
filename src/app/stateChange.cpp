/***********************************************************************
 * Copyright 2004  Max Howell <max.howell@methylblue.com>
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

#include <KConfig>
#include <KLocale>
#include <KGlobal>
#include <KToolBar>

#include <QContextMenuEvent>
#include <QEvent>
#include <QLabel>

#include "actions.h"
#include "adjustSizeButton.h"
#include "debug.h"
#include "mainWindow.h"
#include "mxcl.library.h"
#include "theStream.h"
#include "videoWindow.h"


//TODO do in Sconstruct
#define QT_FATAL_ASSERT


//TODO make the XineEngine into xine::Stream and then make singleton and add functions like Stream::hasVideo() etc.
//TODO make convenience function to get fullscreen state


namespace Codeine {


void
MainWindow::engineStateChanged( Engine::State state )
{
    DEBUG_BLOCK
    Q_ASSERT( state != Engine::Uninitialised );

    KUrl const &url = TheStream::url();
    bool const isFullScreen = toggleAction("fullscreen")->isChecked();
    QWidget *const toolbar = reinterpret_cast<QWidget*>(toolBar());

    Debug::Block block( state == Engine::Empty
            ? "State: Empty" : state == Engine::Loaded
            ? "State: Loaded" : state == Engine::Playing
            ? "State: Playing" : state == Engine::Paused
            ? "State: Paused" : state == Engine::TrackEnded
            ? "State: TrackEnded" : "State: Unknown" );


    /// update actions
    {
        using namespace Engine;

        #define enableIf( name, criteria ) action( name )->setEnabled( state & criteria );
        enableIf( "stop", (Playing | Paused) );
        enableIf( "fullscreen", (Playing | Paused) );
        enableIf( "reset_zoom", ~Empty && !isFullScreen );
        enableIf( "information", ~Empty );
//        enableIf( "video_settings", (Playing | Paused) );
//        enableIf( "volume", (Playing | Paused) );
        #undef enableIf

        toggleAction( "play" )->setChecked( state == Playing );
    }

    debug() << "updated actions" << endl;

    /// update menus
    {
        using namespace Engine;

        // the toolbar play button is always enabled, but the menu item
        // is disabled if we are empty, this looks more sensible
        PlayAction* playAction = static_cast<PlayAction*>( actionCollection()->action("play") );
        playAction->setEnabled( state != Empty );
        playAction->setPlaying( state == Playing );
        actionCollection()->action("aspect_ratio_menu")->setEnabled( state & (Playing | Paused) && TheStream::hasVideo() );

        // set correct aspect ratio
        if( state == Loaded )
            TheStream::aspectRatioAction()->setChecked( true );
    }
    debug() << "updated menus" << endl;

    /// update statusBar
    {
        using namespace Engine;
        m_timeLabel->setVisible( state & (Playing | Paused) );
    }
    debug() << "updated statusbar" << endl;

    /// update position slider
    switch( state )
    {
        case Engine::Uninitialised:
        case Engine::Empty:
            m_positionSlider->setEnabled( false );
            break;
        case Engine::Loaded:
        case Engine::TrackEnded:
//            m_positionSlider->setValue( 0 );
            // NO BREAK!
        case Engine::Playing:
        case Engine::Paused:
            m_positionSlider->setEnabled( TheStream::canSeek() );
        
    }
    debug() << "update position slider" << endl;

    /// update recent files list if necessary
    if( state == Engine::Loaded ) {
        // update recently played list

        #ifndef NO_SKIP_PR0N
        // ;-)
        const QString url_string = url.url();
        if( !(url_string.contains( "porn", Qt::CaseInsensitive ) || url_string.contains( "pr0n", Qt::CaseInsensitive )) )
        #endif
            if( url.protocol() != "dvd" && url.protocol() != "vcd" ) {
                KConfigGroup config = KConfigGroup( KGlobal::config(), "General" );
                const QString prettyUrl = url.prettyUrl();

                QStringList urls = config.readPathEntry( "Recent Urls", QStringList() );
                urls.removeAll( prettyUrl );
                config.writePathEntry( "Recent Urls", urls << prettyUrl );
            }

        if( TheStream::hasVideo() && !isFullScreen )
            new AdjustSizeButton( reinterpret_cast<QWidget*>(videoWindow()) );
    }
    debug() << " update recent files list " << endl;

    /// set titles
    switch( state )
    {
        case Engine::Uninitialised:
        case Engine::Empty:
            m_titleLabel->setText( i18n("No media loaded") );
            break;
        case Engine::Paused:
            m_titleLabel->setText( i18n("Paused") );
            break;
        case Engine::Loaded:
        case Engine::Playing:
        case Engine::TrackEnded:
            m_titleLabel->setText( TheStream::prettyTitle() );
            break;
    }
    debug() << "set titles " << endl;

    /// set toolbar states
    QWidget *dvd_button = toolBar()->findChild< QWidget* >( "toolbutton_toggle_dvd_menu" );
    if (dvd_button)
        dvd_button->setVisible( state != Engine::Empty && url.protocol() == "dvd" );

    if( isFullScreen && !toolbar->testAttribute( Qt::WA_UnderMouse ) ) {
        switch( state ) {
        case Engine::TrackEnded:
            toolbar->show();

            if( videoWindow()->isActiveWindow() ) {
                //FIXME dual-screen this seems to still show
                QContextMenuEvent e( QContextMenuEvent::Other, QPoint() );
                QApplication::sendEvent( videoWindow(), &e );
            }
            break;
        case Engine::Empty:
        case Engine::Loaded:
        case Engine::Paused:
        case Engine::Uninitialised:
            toolBar()->show();
            break;
        case Engine::Playing:
            toolBar()->hide();
            break;
        }
    }
}

}
