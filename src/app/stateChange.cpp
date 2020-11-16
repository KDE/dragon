/*
    SPDX-FileCopyrightText: 2004 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>
    SPDX-FileCopyrightText: 2008 David Edmundson <kde@davidedmundson.co.uk>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "mainWindow.h"

#include <KSharedConfig>
#include <QDebug>
#include <KToolBar>

#include "actions.h"
#include "theStream.h"
#include "videoWindow.h"
#include "audioView2.h"

#define QT_FATAL_ASSERT

namespace Dragon {

void MainWindow::engineStateChanged( Phonon::State state )
{
    bool const isFullScreen = toggleAction("fullscreen")->isChecked();
    bool const hasMedia = TheStream::hasMedia();

    switch(state)
    {
    case Phonon::LoadingState:
        qDebug() << "Loading state";
        break;
    case Phonon::StoppedState:
        qDebug() << "Stopped state";
        break;
    case Phonon::PlayingState:
        qDebug() << "Playing state";
        break;
    case Phonon::BufferingState:
        qDebug() << "Buffering state";
        break;
    case Phonon::PausedState:
        qDebug() << "Paused state";
        break;
    case Phonon::ErrorState:
        qDebug() << "Error state";
        break;
    }

    bool enable = engine()->isActiveState(state);
    action("stop")->setEnabled(enable);
    action("video_settings")->setEnabled(enable && TheStream::hasVideo());
    action("volume")->setEnabled(enable);
    if( m_volumeSlider )
        m_volumeSlider->setEnabled(enable);
    action("fullscreen")->setEnabled(enable || isFullScreen);
    action("reset_zoom")->setEnabled(hasMedia && !isFullScreen);
    action("prev_chapter")->setEnabled(engine()->canGoPrev());
    action("next_chapter")->setEnabled(engine()->canGoNext());

    m_timeLabel->setVisible(enable);
    m_audioView->enableDemo(!enable);

    if (!enable) {
        // Force out of full screen.
        if (isFullScreen) {
            setFullScreen(false);
        }
    }

    if (!m_currentWidget && state == Phonon::PlayingState) {
        if (TheStream::hasVideo()) {
            m_currentWidget = engine();
        } else {
            m_currentWidget = m_audioView;
            if (!isMaximized())
                resize(m_currentWidget->minimumSize());
        }
        toggleLoadView();
    }

    qDebug() << "updated actions";

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
    qDebug() << "updated menus";

    /// turn off screensaver
    if( state == Phonon::PlayingState )
        inhibitPowerSave();
    else if( state == Phonon::StoppedState || !TheStream::hasMedia() )
        releasePowerSave();

    updateTitleBarText();

    // enable/disable DVD specific buttons
    QWidget *dvd_button = toolBar()->findChild< QWidget* >( QLatin1String( "toolbutton_toggle_dvd_menu" ));
    if(videoWindow()->isDVD()) {
        if (dvd_button) {
            dvd_button->setVisible(true);
        }
        action("toggle_dvd_menu")->setEnabled( true );
    } else {
        if (dvd_button) {
            dvd_button->setVisible(false);
        }
        action("toggle_dvd_menu")->setEnabled( false );
    }
}//engineStateChanged


void MainWindow::engineMediaChanged(Phonon::MediaSource /*newSource*/)
{
    m_audioView->update();

    // update recently played list
    qDebug() << " update recent files list ";

    Q_EMIT fileChanged( engine()->urlOrDisc() );
    //TODO fetch this from the Media source
    QUrl const &url = TheStream::url();
    const QString url_string = url.url();

#ifndef NO_SKIP_PR0N
    // ;-)
    if( !(url_string.contains( QLatin1String( "porn" ), Qt::CaseInsensitive )
          || url_string.contains( QLatin1String( "pr0n" ), Qt::CaseInsensitive )) ) {
#endif
        if( url.scheme() != QLatin1String( "dvd" ) && url.scheme() != QLatin1String( "vcd" ) && !url.toDisplayString().isEmpty()) {
            KConfigGroup config = KConfigGroup( KSharedConfig::openConfig(), "General" );
            const auto list = config.readPathEntry( "Recent Urls", QStringList() );
            auto urls = QUrl::fromStringList(list);
            urls.removeAll(url);
            config.writePathEntry( "Recent Urls", QUrl::toStringList(urls << url) );
            Q_EMIT m_loadView->reloadRecentlyList();
        }
#ifndef NO_SKIP_PR0N
    }
#endif

}//engineMediaChanged

void MainWindow::engineSeekableChanged(bool canSeek)
{
    qDebug() << "seekable changed to " << canSeek;
    m_positionSlider->setEnabled( canSeek );
    action("ten_percent_back")->setEnabled( canSeek );
    action("ten_percent_forward")->setEnabled( canSeek );
    action("ten_seconds_back")->setEnabled( canSeek );
    action("ten_seconds_forward")->setEnabled( canSeek );
    //TODO connect/disconnect the jump forward/back here.
}//engineSeekableChanged


void MainWindow::engineMetaDataChanged()
{
    qDebug() << "metaDataChanged"; // FIXME Phonon emits metadataChanged() too often :/
    qDebug() << "Disc ID:" << TheStream::discId();
    updateTitleBarText();
    if (!TheStream::hasVideo())
        m_audioView->update();
}

void MainWindow::engineHasVideoChanged( bool hasVideo )
{
    Q_UNUSED(hasVideo);

    qDebug() << "hasVideo changed" << hasVideo;
    if( TheStream::hasVideo() ) {
        if( m_mainView->indexOf(engine()) == -1 )
            m_mainView->addWidget(engine());
        m_mainView->setCurrentWidget(engine());
        m_currentWidget = engine();

        // Fake change of state to trigger a re-evaluation of enabled actions.
        // The video state might have changed *after* a state change (e.g. in Phonon-VLC)
        // in which case the video related menu actions will not be enabled until
        // a new state change occurs. By forcing a fake state change we can work around this.
        engineStateChanged(videoWindow()->state());
    } else if (engine()->isActiveState()) {
        m_audioView->setupAnalyzer();
        m_mainView->setCurrentWidget(m_audioView);
        m_currentWidget = m_audioView;
    }

    if (TheStream::hasVideo()) {
        inhibitPowerSave();
        // Assumption: since we have no playlist the only way to release suppression
        // is through going into stopped state. This also means that should there
        // ever be a playlist playing video and then audio will possibly not
        // release video specific inhibitions.
    }
}

}//namespace
