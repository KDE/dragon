/***********************************************************************
 * Copyright 2004  Max Howell <max.howell@methylblue.com>
 *           2007  Ian Monroe <ian@monroe.nu>
*            2008  David Edmundson <kde@davidedmundson.co.uk>
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

#include "mainWindow.h"

#include <KConfig>
#include <KLocale>
#include <KGlobal>
#include <KNotificationRestrictions>
#include <KToolBar>

#include <QContextMenuEvent>
#include <QToolButton>

#include "actions.h"
#include "adjustSizeButton.h"
#include "dbus/playerDbusHandler.h"
#include "debug.h"
#include "fullScreenAction.h"
#include "mxcl.library.h"
#include "theStream.h"
#include "videoWindow.h"


//TODO do in Sconstruct
#define QT_FATAL_ASSERT


//TODO make the XineEngine into xine::Stream and then make singleton and add functions like Stream::hasVideo() etc.
//TODO make convenience function to get fullscreen state


namespace Codeine {


void
MainWindow::engineStateChanged( Phonon::State state )
{
    bool const isFullScreen = toggleAction("fullscreen")->isChecked();
    bool const hasMedia = TheStream::hasMedia();
    QWidget *const toolbar = reinterpret_cast<QWidget*>(toolBar());

    switch(state)
    {
      case Phonon::LoadingState:
        debug() << "Loading state";
        break;
      case Phonon::StoppedState:
        debug() << "Stopped state";
        break;
      case Phonon::PlayingState:
        debug() << "Playing state";
        break;
      case Phonon::BufferingState:
        debug() << "Buffering state";
        break;
      case Phonon::PausedState:
        debug() << "Paused state";
        break;
      case Phonon::ErrorState:
        debug() << "Error state";
        break;
    }

//     using namespace Engine;

    bool enable = FALSE;
    if(state == Phonon::PlayingState || state == Phonon::PausedState)
    {
      enable=TRUE;
    }
    action("stop")->setEnabled(enable);
    action("video_settings")->setEnabled(enable);
    action("volume")->setEnabled(enable);
    if( m_volumeSlider )
      m_volumeSlider->setEnabled(enable); 
    action("fullscreen")->setEnabled(enable || isFullScreen);
    action("reset_zoom")->setEnabled(hasMedia && !isFullScreen);
    toggleAction( "play" )->setChecked(state == Phonon::PlayingState);

    m_timeLabel->setVisible(enable);


    debug() << "updated actions";

    /// update menus
    {
        // the toolbar play button is always enabled, but the menu item
        // is disabled if we are empty, this looks more sensible
        PlayAction* playAction = static_cast<PlayAction*>( actionCollection()->action("play") );
        playAction->setEnabled( hasMedia );
        playAction->setPlaying( state == Phonon::PlayingState );
        actionCollection()->action("aspect_ratio_menu")->setEnabled(( state == Phonon::PlayingState || state == Phonon::PausedState) && TheStream::hasVideo() );

        // set correct aspect ratio
        if( state != Phonon::LoadingState )
            TheStream::aspectRatioAction()->setChecked( true );
    }
    debug() << "updated menus";

    /// turn off screensaver
    if( state == Phonon::PlayingState )
    {
        if( !m_stopScreenSaver )
        {
            debug() << "screensaver off";
            m_stopScreenSaver = new KNotificationRestrictions( KNotificationRestrictions::NonCriticalServices, this );
        }
        else
            warning() << "m_stopScreenSaver not null";
    }
    else if( Phonon::StoppedState || !TheStream::hasMedia()  )
    {
        delete m_stopScreenSaver;
        m_stopScreenSaver = 0;
        debug() << "screensaver on";
    }

	//if there's something loaded
	if(! TheStream::hasMedia())
    {
        m_titleLabel->setText( i18n("No media loaded") );
    }
    else if( state == Phonon::PausedState)
    {
        m_titleLabel->setText( i18n("Paused") );
    }
    else
    {
        m_titleLabel->setText( TheStream::prettyTitle() );
    }
    debug() << "set titles ";


    // enable/disable DVD specific buttons
    QWidget *dvd_button = toolBar()->findChild< QWidget* >( "toolbutton_toggle_dvd_menu" );
    if(videoWindow()->isDVD())
    {
        if (dvd_button)
        {
            dvd_button->setVisible(true);
        }
        action("toggle_dvd_menu")->setEnabled( true );
    }
    else
    {
        if (dvd_button)
        {
            dvd_button->setVisible(false);
        }
        action("toggle_dvd_menu")->setEnabled( false );
    }
    if( isFullScreen && !toolbar->testAttribute( Qt::WA_UnderMouse ) ) 
    {/*
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
        case Engine::Paused:
        case Engine::Uninitialised:
            toolBar()->show();
            break;
        case Phonon::PlayingState:
            toolBar()->hide();
            break;
        case Engine::Loaded:
            break;
        }*/
    } 
    switch( state )
    {
        case Phonon::StoppedState:
            emit dbusStatusChanged( PlayerDbusHandler::Stopped ), debug() << "dbus: stopped";
            break;
        case Phonon::PausedState:
            emit dbusStatusChanged( PlayerDbusHandler::Paused ), debug() << "dbus: paused";
            break;
        case Phonon::PlayingState:
            emit dbusStatusChanged( PlayerDbusHandler::Playing ), debug() << "dbus: playing";
            break;
        default:/*If none of these states don't emit anything*/
            break;
    }
}//engineStateChanged


void
MainWindow::engineMediaChanged(Phonon::MediaSource /*newSource*/)
{
 // update recently played list
debug() << " update recent files list ";

emit fileChanged( engine()->urlOrDisc() );
//TODO fetch this from the Media source
KUrl const &url = TheStream::url();
const QString url_string = url.url();

#ifndef NO_SKIP_PR0N
// ;-)
if( !(url_string.contains( "porn", Qt::CaseInsensitive ) || url_string.contains( "pr0n", Qt::CaseInsensitive )) )
#endif
  if( url.protocol() != "dvd" && url.protocol() != "vcd" && url.prettyUrl()!="")
  {
    KConfigGroup config = KConfigGroup( KGlobal::config(), "General" );
    const QString prettyUrl = url.prettyUrl();
    QStringList urls = config.readPathEntry( "Recent Urls", QStringList() );
    urls.removeAll( prettyUrl );
    config.writePathEntry( "Recent Urls", urls << prettyUrl );
  }

}//engineMediaChanged

void MainWindow::engineSeekableChanged(bool canSeek)
{
  debug() << "seekable changed to " << canSeek;
  m_positionSlider->setEnabled( canSeek );
  //TODO connect/disconnect the jump forward/back here.
}//engineSeekableChanged

}//namespace
