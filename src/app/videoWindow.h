/***********************************************************************
 * Copyright 2005  Max Howell <max.howell@methylblue.com>
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

#ifndef DRAGONPLAYER_VIDEOWINDOW_H
#define DRAGONPLAYER_VIDEOWINDOW_H

#include "codeine.h"

#include <QMultiMap>
#include <QWidget>

#include <Phonon/Path>
#include <Phonon/MediaSource>
#include <Phonon/ObjectDescription>
#include <Solid/Device>
#include <KUrl>

class QActionGroup;
class QTimer;

namespace Phonon {
     class VideoWidget;
     class AudioOutput;
     class MediaObject;
     class MediaController;
     class AudioDataOutput;
}

#include "phonon/phononnamespace.h" //Phonon::State

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

        template<class ChannelDescription>
        void updateActionGroup( QActionGroup* channelActions, const QList<ChannelDescription>& availableChannels
            , const char* actionSlot );

    public:
        VideoWindow( QWidget *parent );
       ~VideoWindow();

        bool init();

        bool load( const KUrl &url );
        bool play( qint64 = 0 );
        bool resume();
        bool playDvd();
        bool playDisc( const Solid::Device& );
        bool isMuted();
        bool isPreview(const bool &v = 0);
        void relativeSeek( qint64 );

        qint64 length() const;
        bool isDVD() const;

        bool setupAnalyzer(QObject* analzyer); ///return whether setup was successful

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

    /// Stuff to do with video and the video window/widget
        static const uint CURSOR_HIDE_TIMEOUT = 2000;

        void becomePreferredSize();

        enum { ExposeEvent = 3000 };

        qint64 currentTime() const;
        int videoSetting( const QString& );

    public slots:
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

    protected:
        virtual bool event( QEvent* e );
        virtual void contextMenuEvent( QContextMenuEvent * event );
        virtual void mouseDoubleClickEvent( QMouseEvent* );
        virtual QSize sizeHint() const;
        Phonon::State state( Phonon::State state ) const;
        void setSubtitle( int channel );
        void setAudioChannel( int channel );
    private slots:
        void updateChannels();
        void hideCursor();
    signals:
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
    };

    //global function for general use by Dragon Player

    //rearranged from previous non-static functions due to compiler warning
    static inline VideoWindow* engine() {return VideoWindow::s_instance;}
    static inline VideoWindow* videoWindow() {return VideoWindow::s_instance; }
}

#endif
