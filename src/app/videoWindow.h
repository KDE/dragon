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

#ifndef CODEINE_VIDEOWINDOW_H
#define CODEINE_VIDEOWINDOW_H

#include "codeine.h"
#include <QWidget>

#include <KUrl>
class QActionGroup;
namespace Phonon {
     class VideoWidget;
     class AudioOutput;
     class MediaObject;
}

typedef struct xine_stream_s xine_stream_t;

#include "phonon/phononnamespace.h" //Phonon::State

namespace Codeine
{
    class VideoWindow : public QWidget
    {
    Q_OBJECT

        static VideoWindow *s_instance;

        VideoWindow( const VideoWindow& ); //disable
        VideoWindow &operator=( const VideoWindow& ); //disable

        KUrl m_url;
        bool m_justLoaded;
        xine_stream_t* m_xineStream;
        QActionGroup* m_languages;
        Phonon::VideoWidget *m_vWidget;
        Phonon::AudioOutput *m_aOutput;
        Phonon::MediaObject *m_media;

        friend class TheStream;
        friend VideoWindow* const engine();
        friend VideoWindow* const videoWindow();

    public:
        VideoWindow( QWidget *parent );
       ~VideoWindow();

        bool init();
        void exit();

        bool load( const KUrl &url );
        bool play( qint64 = 0 );
        bool playDvd();

        uint length() const { return 0; }

        uint volume() const;
        QWidget* newPositionSlider();
        QWidget* newVolumeSlider();

        Engine::State state() const;

    /// Stuff to do with video and the video window/widget
        static const uint CURSOR_HIDE_TIMEOUT = 2000;

        void becomePreferredSize();

        enum { ExposeEvent = 3000 };

        qint64 currentTime() const;
        QString fileFilter() const;
        
        const xine_stream_t* xineStream() const { return m_xineStream; }
        
    public slots:
        void playPause();
        void record();
        void seek( qint64 );
        void stop();

        ///special slot, see implementation to facilitate understanding
        void setStreamParameter( int );
        void stateChanged(Phonon::State, Phonon::State);
        
        void toggleDVDMenu();
        void showOSD( const QString& );
        
        void setFullScreen( bool f );
    
    protected:
        virtual void contextMenuEvent( QContextMenuEvent * event );
        void refreshXineStream();
        Engine::State state( Phonon::State state ) const;
    signals:
        void stateChanged( Engine::State );
        void statusMessage( const QString& );
        void titleChanged( const QString& );
        void channelsChanged( QList< QAction* > );
    };

    //global function for general use by Codeine
    //videoWindow() is const for Xlib-thread-safety reasons
    inline VideoWindow* const videoWindow() { return VideoWindow::s_instance; }
    inline VideoWindow* const engine() { return VideoWindow::s_instance; }
}

#endif
