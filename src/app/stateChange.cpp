// Copyright 2004 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <ktoolbar.h>

#include <q3popupmenu.h>
#include <Q3MainWindow>
#include <qapplication.h>
#include <QContextMenuEvent>
#include <qevent.h>
#include <qlabel.h>
#include <qslider.h>

#include "actions.h"
#include "adjustSizeButton.h"
#include "debug.h"
#include "mainWindow.h"
#include "mxcl.library.h"
#include "theStream.h"
#include "videoSettings.h" //FIXME unfortunate
#include "xineEngine.h"


//TODO do in Sconstruct
#define QT_FATAL_ASSERT


//TODO make the XineEngine into xine::Stream and then make singleton and add functions like Stream::hasVideo() etc.
//TODO make convenience function to get fullscreen state


namespace Codeine {


void
MainWindow::engineStateChanged( Engine::State state )
{
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
        enableIf( "video_settings", (Playing | Paused) );
        enableIf( "volume", (Playing | Paused) );
        #undef enableIf

        toggleAction( "play" )->setChecked( state == Playing );

        //FIXME bad design to do this way
        QSlider *volume = (QSlider*)toolBar()->child( "volume" );
        if (volume)
            volume->setValue( engine()->volume() );
    }

    debug() << "updated actions" << endl;
    /// update VideoSettingsDialog instance
    VideoSettingsDialog::stateChanged( this, state );


    /// update menus
    {
        using namespace Engine;

        // the toolbar play button is always enabled, but the menu item
        // is disabled if we are empty, this looks more sensible
        QMenu * const file_menu = menu( "file" );
        QMenu * const settings_menu = menu( "settings" );
        const int play_id = file_menu->idAt( 2 );
        file_menu->setItemEnabled( play_id, state != Empty );

        // menus are clearer when handled differently to toolbars
        // KDE has a shit special action for this, but it stupidly changes
        // the toolbar icon too.
        // TODO do this from the playAction since we do it in context menu too
        const KGuiItem item = (state == Playing) ? KGuiItem( i18n("&Pause"), "player_pause" ) : KGuiItem( i18n("&Play"), "player_play" );
        file_menu->changeItem( play_id, item.iconSet(), item.text() );
        file_menu->setItemChecked( play_id, false );

        settings_menu->setItemEnabled( AspectRatioMenuItemId, state & (Playing | Paused) && TheStream::hasVideo() );

        // set correct aspect ratio
        if( state == Loaded )
            static_cast<QMenu*>(child( "aspect_ratio_menu" ))->setItemChecked( TheStream::aspectRatio(), true );
    }
    debug() << "updated menus" << endl;

    /// update statusBar
    {
        using namespace Engine;
        m_timeLabel->setShown( state & (Playing | Paused) );
    }
    debug() << "updated statusbar" << endl;

    /// update position slider
    switch( state )
    {
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
            break;
    }
    debug() << "update position slider" << endl;

    /// update recent files list if necessary
    if( state == Engine::Loaded ) {
        // update recently played list

        #ifndef NO_SKIP_PR0N
        // ;-)
        const QString url_string = url.url();
        if( !(url_string.contains( "porn", false ) || url_string.contains( "pr0n", false )) )
        #endif
            if( url.protocol() != "dvd" && url.protocol() != "vcd" ) {
                KConfigGroup config = Codeine::config( "General" );
                const QString prettyUrl = url.prettyUrl();

                QStringList urls = config.readPathListEntry( "Recent Urls" );
                urls.remove( prettyUrl );
                config.writePathEntry( "Recent Urls", urls << prettyUrl );
            }

        if( TheStream::hasVideo() && !isFullScreen )
            new AdjustSizeButton( reinterpret_cast<QWidget*>(videoWindow()) );
    }
    debug() << " update recent files list " << endl;

    /// set titles
    switch( state )
    {
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
    QWidget *dvd_button = (QWidget*)toolBar()->child( "toolbutton_toggle_dvd_menu" );
    if (dvd_button)
        dvd_button->setShown( state != Engine::Empty && url.protocol() == "dvd" );

    if( isFullScreen && !toolbar->hasMouse() ) {
        switch( state ) {
        case Engine::TrackEnded:
            toolbar->show();

            if( videoWindow()->isActiveWindow() ) {
                //FIXME dual-screen this seems to still show
                QContextMenuEvent e( QContextMenuEvent::Other, QPoint(), Qt::MetaModifier );
                QApplication::sendEvent( videoWindow(), &e );
            }
            break;
        case Engine::Empty:
        case Engine::Loaded:
        case Engine::Paused:
            toolBar()->show();
            break;
        case Engine::Playing:
            toolBar()->hide();
            break;
        }
    }
}

}
