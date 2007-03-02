// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINE_VIDEOWINDOW_H
#define CODEINE_VIDEOWINDOW_H

#include "codeine.h"
#include <qwidget.h>

#include <kurl.h>

namespace Phonon {
     class VideoWidget;
     class VideoPath;
     class AudioOutput;
     class AudioPath;
     class MediaObject;
}

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
        Phonon::VideoWidget *m_vWidget;
        Phonon::VideoPath    *m_vPath;
        Phonon::AudioOutput *m_aOutput;
        Phonon::AudioPath    *m_aPath;
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

    public slots:
        void pause();
        void record();
        void seek( qint64 );
        void stop();

        ///special slot, see implementation to facilitate understanding
        void setStreamParameter( int );
        void stateChanged(Phonon::State,Phonon::State) { emit stateChanged( state() ); }
        
        void toggleDVDMenu();
        void showOSD( const QString& );
        
        void setFullScreen( bool f );
    
    protected:
        virtual void contextMenuEvent( QContextMenuEvent * event );

    Q_SIGNALS:
        void stateChanged( Engine::State );
        void statusMessage( const QString& );
        void titleChanged( const QString& );
        void channelsChanged( const QStringList& );
    
    };

    //global function for general use by Codeine
    //videoWindow() is const for Xlib-thread-safety reasons
    inline VideoWindow* const videoWindow() { return VideoWindow::s_instance; }
    inline VideoWindow* const engine() { return VideoWindow::s_instance; }
}

#endif
