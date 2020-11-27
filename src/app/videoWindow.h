/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYER_VIDEOWINDOW_H
#define DRAGONPLAYER_VIDEOWINDOW_H

#include "codeine.h"

#include <QMultiMap>
#include <QWidget>
#include <QUrl>

#include <Phonon/Path>
#include <Phonon/MediaSource>
#include <Phonon/ObjectDescription>
#include <Phonon/Global>

#include <Solid/Device>

class QActionGroup;
class QTimer;

namespace Phonon {
class VideoWidget;
class AudioOutput;
class MediaObject;
class MediaController;
class AudioDataOutput;
}

namespace Dragon
{
class VideoWindow : public QWidget
{
    Q_OBJECT

public:
    static VideoWindow *s_instance;

private:
    VideoWindow( const VideoWindow& ); //disable
    VideoWindow &operator=( const VideoWindow& ); //disable
    void eject();

    QTimer* m_cursorTimer;
    bool m_justLoaded;
    bool m_adjustedSize;
    QActionGroup* m_subLanguages;
    QActionGroup* m_audioLanguages;
    QWidget* m_logo;
    bool m_isPreview;
    quint64 m_initialOffset;

    Phonon::VideoWidget *m_vWidget;
    Phonon::AudioOutput *m_aOutput;
    Phonon::MediaObject *m_media;
    Phonon::MediaController *m_controller;
    Phonon::AudioDataOutput* m_aDataOutput;
    Phonon::Path m_audioPath;
    Phonon::Path m_audioDataPath;

    friend class TheStream;

    template<class ChannelDescription, class Func>
    void updateActionGroup( QActionGroup* channelActions, const QList<ChannelDescription>& availableChannels
                            , Func actionSlot );

public:
    explicit VideoWindow( QWidget *parent );
    ~VideoWindow() override;

    bool init();

    bool load( const QUrl &url );
    bool load( const QList<QUrl> &urls );
    bool play( qint64 = 0 );
    bool resume();
    bool playDvd();
    bool playDisc( const Solid::Device& );
    bool isMuted();
    bool isPreview(const bool &v = 0);
    void relativeSeek( qint64 );

    qint64 length() const;
    bool isDVD() const;

    bool setupAnalyzer(QObject* analyzer); ///return whether setup was successful

    ///stuff for dbus:
    qreal volume() const;
    void setVolume( qreal );
    QString urlOrDisc() const;
    QMultiMap<QString, QString> metaData() const;
    Phonon::MediaSource::Type mediaSourceType() const;
    bool isSeekable() const;
    qint32 tickInterval() const;
    //}

    QWidget* newPositionSlider();
    QWidget* newVolumeSlider();
    void loadSettings();

    Phonon::State state() const;
    bool isActiveState() const;
    bool isActiveState(Phonon::State s) const;

    /// Stuff to do with video and the video window/widget
    static const uint CURSOR_HIDE_TIMEOUT = 2000;

    qint64 currentTime() const;
    int videoSetting( const QString& );

public Q_SLOTS:
    void pause();
    void playPause();
    void seek( qint64 );
    void stop();
    void stateChanged( Phonon::State, Phonon::State );
    void settingChanged( int );
    void mute( bool );

    void toggleDVDMenu();
    void showOSD( const QString& );
    void slotSetSubtitle();
    void slotSetAudio();
    void resetZoom();

    void prevChapter();
    void nextChapter();
    void tenPercentBack();
    void tenPercentForward();
    void tenSecondsBack();
    void tenSecondsForward();

    void increaseVolume();
    void decreaseVolume();

    bool canGoPrev() const;
    bool canGoNext() const;

protected:
    bool event( QEvent* e ) override;
    void contextMenuEvent( QContextMenuEvent * event ) override;
    void mouseDoubleClickEvent( QMouseEvent* ) override;
    QSize sizeHint() const override;
    Phonon::State state( Phonon::State state ) const;
    void setSubtitle( int channel );
    void setAudioChannel( int channel );
private Q_SLOTS:
    void updateChannels();
    void hideCursor();
Q_SIGNALS:
    void stateUpdated( const Phonon::State, const Phonon::State );
    void subChannelsChanged( QList< QAction* > );
    void audioChannelsChanged( QList< QAction* > );
    void tick( qint64 );
    void currentSourceChanged( const Phonon::MediaSource);
    void totalTimeChanged( qint64 );
    void mutedChanged( bool );
    void seekableChanged( bool );
    void metaDataChanged();
    void hasVideoChanged( bool );
    void volumeChanged( qreal );
    void finished();
};

//global function for general use by Dragon Player

//rearranged from previous non-static functions due to compiler warning
static inline VideoWindow* engine() {return VideoWindow::s_instance;}
static inline VideoWindow* videoWindow() {return VideoWindow::s_instance; }
}

#endif
