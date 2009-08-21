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
#include <QDBusInterface>
#include <QDBusReply>

#include <solid/powermanagement.h>

#include "actions.h"
#include "adjustSizeButton.h"
#include "dbus/playerDbusHandler.h"
#include "debug.h"
#include "theStream.h"
#include "videoWindow.h"


//TODO do in Sconstruct
#define QT_FATAL_ASSERT


//TODO make the XineEngine into xine::Stream and then make singleton and add functions like Stream::hasVideo() etc.
//TODO make convenience function to get fullscreen state


namespace Dragon {


void
MainWindow::engineStateChanged( Phonon::State state, Phonon::State oldstate )
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

    bool enable = false;
    if(state == Phonon::PlayingState || state == Phonon::PausedState)
    {
      enable = true;
    }
    action("stop")->setEnabled(enable);
    action("video_settings")->setEnabled(enable && TheStream::hasVideo());
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
      m_stopSleepCookie = Solid::PowerManagement::beginSuppressingSleep("DragonPlayer: watching a film");
        
      QDBusInterface screensaver("org.freedesktop.ScreenSaver", "/ScreenSaver", "org.freedesktop.ScreenSaver");
      QDBusReply<int> screensaverRc = screensaver.call("Inhibit","dragonplayer","Watching a film");
      if (screensaverRc.isValid())
      {
         m_screensaverDisableCookie = screensaverRc.value();
      }
    }
    else if( Phonon::StoppedState || !TheStream::hasMedia() )
    {
      //stop supressing sleep
      Solid::PowerManagement::stopSuppressingSleep(m_stopSleepCookie);

     //stop disabling screensaver
      if (m_screensaverDisableCookie != 0)
      {
        QDBusInterface screensaver("org.freedesktop.ScreenSaver", "/ScreenSaver", "org.freedesktop.ScreenSaver");
        screensaver.call("Uninhibit",m_screensaverDisableCookie);
        m_screensaverDisableCookie = 0;
      }
    }
    
    updateTitleBarText();

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
}//engineStateChanged


void
MainWindow::engineMediaChanged(Phonon::MediaSource /*newSource*/)
{
    m_audioView->updateText();

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
    if( url.protocol() != "dvd" && url.protocol() != "vcd" && !url.prettyUrl().isEmpty())
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


void MainWindow::engineMetaDataChanged()
{
    debug() << "metaDataChanged";
    updateTitleBarText();
    m_audioView->updateText();
}

void MainWindow::engineHasVideoChanged(bool hasVideo)
{
  debug() << "hasVideo changed";
  if( TheStream::hasVideo() )
  {
    if( m_mainView->indexOf(engine()) == -1 )
      m_mainView->addWidget(engine());
    m_mainView->setCurrentWidget(engine());
    m_currentWidget = engine();
  }
  else
  {
    m_mainView->setCurrentWidget(m_audioView);
    m_currentWidget = m_audioView;
  }
}

}//namespace
