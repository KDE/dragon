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
#include <KDebug>
#include <KToolBar>

#include "actions.h"
#include "theStream.h"
#include "videoWindow.h"
#include "audioView2.h"

#define QT_FATAL_ASSERT

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
        kDebug() << "Loading state";
        break;
      case Phonon::StoppedState:
        kDebug() << "Stopped state";
        break;
      case Phonon::PlayingState:
        kDebug() << "Playing state";
        break;
      case Phonon::BufferingState:
        kDebug() << "Buffering state";
        break;
      case Phonon::PausedState:
        kDebug() << "Paused state";
        break;
      case Phonon::ErrorState:
        kDebug() << "Error state";
        break;
    }

    bool enable = false;
    if(state == Phonon::PlayingState || state == Phonon::PausedState || state == Phonon::BufferingState)
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

    m_timeLabel->setVisible(enable);
    m_audioView->enableDemo(!enable);

    kDebug() << "updated actions";

    /// update menus
    {
        // the toolbar play button is always enabled, but the menu item
        // is disabled if we are empty, this looks more sensible
        PlayAction* playAction = static_cast<PlayAction*>( actionCollection()->action(QLatin1String( "play" )) );
        playAction->setEnabled( hasMedia );
        playAction->setPlaying( state == Phonon::PlayingState || state == Phonon::BufferingState );
        actionCollection()->action(QLatin1String( "aspect_ratio_menu" ))->setEnabled((enable) && TheStream::hasVideo() );

        // set correct aspect ratio
        if( state != Phonon::LoadingState )
            TheStream::aspectRatioAction()->setChecked( true );
    }
    kDebug() << "updated menus";

    /// turn off screensaver
    if( state == Phonon::PlayingState )
        inhibitPowerSave();
    else if( Phonon::StoppedState || !TheStream::hasMedia() )
        releasePowerSave();

    updateTitleBarText();

    // enable/disable DVD specific buttons
    QWidget *dvd_button = toolBar()->findChild< QWidget* >( QLatin1String( "toolbutton_toggle_dvd_menu" ));
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
}//engineStateChanged


void
MainWindow::engineMediaChanged(Phonon::MediaSource /*newSource*/)
{
#ifdef __GNUC__
#warning FIXME
#endif
//    m_audioView->updateText();

    // update recently played list
    kDebug() << " update recent files list ";

    emit fileChanged( engine()->urlOrDisc() );
    //TODO fetch this from the Media source
    KUrl const &url = TheStream::url();
    const QString url_string = url.url();

    #ifndef NO_SKIP_PR0N
    // ;-)
    if( !(url_string.contains( QLatin1String( "porn" ), Qt::CaseInsensitive ) || url_string.contains( QLatin1String(  "pr0n" ), Qt::CaseInsensitive )) )
    #endif
    if( url.protocol() != QLatin1String( "dvd" ) && url.protocol() != QLatin1String( "vcd" ) && !url.prettyUrl().isEmpty())
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
  kDebug() << "seekable changed to " << canSeek;
  m_positionSlider->setEnabled( canSeek );
  //TODO connect/disconnect the jump forward/back here.
}//engineSeekableChanged


void MainWindow::engineMetaDataChanged()
{
    kDebug() << "metaDataChanged";
    updateTitleBarText();
#ifdef __GNUC__
#warning FIXME
#endif
//    m_audioView->updateText();
}

void MainWindow::engineHasVideoChanged(bool hasVideo)
{
  kDebug() << "hasVideo changed";
  if( TheStream::hasVideo() )
  {
    if( m_mainView->indexOf(engine()) == -1 )
      m_mainView->addWidget(engine());
    m_mainView->setCurrentWidget(engine());
    m_currentWidget = engine();

    // Fake change of state to trigger a re-evaluation of enabled actions.
    // The video state might have changed *after* a state change (e.g. in Phonon-VLC)
    // in which case the video related menu actions will not be enabled until
    // a new state change occurs. By forcing a fake state change we can work around this.
    engineStateChanged(videoWindow()->state(), videoWindow()->state());
  }
  else
  {
    m_mainView->setCurrentWidget(m_audioView);
    m_currentWidget = m_audioView;
  }
}

}//namespace
