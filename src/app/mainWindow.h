/*
    SPDX-FileCopyrightText: 2004 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYERMAINWINDOW_H
#define DRAGONPLAYERMAINWINDOW_H

#include "codeine.h"
#include "loadView.h"
#include "timeLabel.h"

#include <KXmlGuiWindow>
#include <QCheckBox>
#include <QList>
#include <QPointer>
#include <QStackedWidget>
#include <phonon/MediaSource>

#include <optional>

class KToggleAction;
class QUrl;
class QActionGroup;
class QCloseEvent;
class QLabel;
class QMenu;
class QSlider;

class AudioView2;
#include "audioView2.h"

namespace Dragon
{
class FullScreenToolBarHandler;

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT

    MainWindow();
    ~MainWindow() override;

    static MainWindow *s_instance;

    friend class PlayerApplication;
    friend QWidget *mainWindow();

public:
    void openRecentFile(const QUrl &);
    void showVolume(bool);
    bool volumeContains(const QPoint &mousePos);

Q_SIGNALS:
    void fileChanged(QString);

public Q_SLOTS:
    bool open(const QUrl &);
    void playDisc();
    void openFileDialog();
    void openStreamDialog();
    void play();
    void toggleVideoSettings(bool);
    void toggleVolumeSlider(bool);
    void restoreDefaultVideoSettings();
    void toggleLoadView();

private Q_SLOTS:
    void setFullScreen(bool full);
    void engineMessage(const QString &);
    void init();
    void aboutToShowMenu();
    void streamSettingChange();
    void subChannelsChanged(QList<QAction *>);
    void audioChannelsChanged(QList<QAction *>);
    void mutedChanged(bool);
    void stop();
    // in stateChange.cpp
    void engineStateChanged(Phonon::State);
    void engineMediaChanged(Phonon::MediaSource);
    void engineSeekableChanged(bool);
    void engineMetaDataChanged();
    void engineHasVideoChanged(bool);
    void toggleUnique(bool);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    bool load(const QUrl &);
    void setupActions();
    void updateSliders();
    void updateTitleBarText();
    bool isFresh();

    QMenu *menu(const char *name);

    void dragEnterEvent(QDragEnterEvent *) override;
    void dropEvent(QDropEvent *) override;
    void keyPressEvent(QKeyEvent *) override;

    void inhibitPowerSave();
    void releasePowerSave();

    //      virtual void saveProperties( KConfig* );
    //      virtual void readProperties( KConfig* );

    QStackedWidget *m_mainView;
    AudioView2 *m_audioView;
    LoadView *m_loadView;
    QWidget *m_currentWidget;

    QPointer<QDockWidget> m_leftDock;
    QPointer<QDockWidget> m_rightDock;
    QWidget *m_positionSlider;
    QPointer<QWidget> m_volumeSlider;
    QCheckBox *m_muteCheckBox;
    TimeLabel *m_timeLabel;
    QLabel *m_titleLabel;
    QList<QSlider *> m_sliders;

    KToggleAction *m_menuToggleAction;

    std::optional<int> m_screensaverDisableCookie;
    int m_stopSleepCookie;
    int m_stopScreenPowerMgmtCookie;
    int m_profileMaxDays;

    bool m_toolbarIsHidden;
    bool m_statusbarIsHidden;
    bool m_menuBarIsHidden;
    FullScreenToolBarHandler *m_FullScreenHandler;

    QActionGroup *m_aspectRatios;
    Q_DISABLE_COPY(MainWindow)

protected:
    void closeEvent(QCloseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};

}

#endif
