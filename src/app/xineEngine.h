// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINE_VIDEOWINDOW_H
#define CODEINE_VIDEOWINDOW_H

#include "codeine.h"
#include <qtimer.h>
#include <qwidget.h>
#include <kurl.h>
#include <vector>

typedef struct xine_s xine_t;
typedef struct xine_stream_s xine_stream_t;
typedef struct xine_video_port_s xine_video_port_t;
typedef struct xine_audio_port_s xine_audio_port_t;
typedef struct xine_event_queue_s xine_event_queue_t;
typedef struct xine_post_s xine_post_t;
typedef struct xine_osd_s xine_osd_t;

namespace Engine {
   typedef std::vector<int16_t> Scope;
}


namespace Codeine
{
   /** Functions declared here are defined in:
    *    xineEngine.cpp
    *    videoWindow.cpp
    */
   class VideoWindow : public QWidget
   {
   Q_OBJECT

      enum PosTimeLength { Pos, Time, Length };

      static VideoWindow *s_instance;

      VideoWindow( const VideoWindow& ); //disable
      VideoWindow &operator=( const VideoWindow& ); //disable

      friend class TheStream;
      friend VideoWindow* const engine();
      friend VideoWindow* const videoWindow();

   public:
      VideoWindow( QWidget *parent );
     ~VideoWindow();

      bool init();
      void exit();

      bool load( const KURL &url );
      bool play( uint = 0 );

      uint position() const { return posTimeLength( Pos ); }
      uint time() const { return posTimeLength( Time ); }
      uint length() const { return posTimeLength( Length ); }

      uint volume() const;

      const Engine::Scope &scope();
      Engine::State state() const;

      operator xine_t*() const { return m_xine; }
      operator xine_stream_t*() const { return m_stream; }

   public slots:
      void pause();
      void record();
      void seek( uint );
      void stop();

      ///special slot, see implementation to facilitate understanding
      void setStreamParameter( int );

   signals:
      void stateChanged( Engine::State );
      void statusMessage( const QString& );
      void titleChanged( const QString& );
      void channelsChanged( const QStringList& );

   private:
      #ifdef HAVE_XINE_H
      static void xineEventListener( void*, const xine_event_t* );
      #endif

      uint posTimeLength( PosTimeLength ) const;
      void showErrorMessage();

      virtual void customEvent( QCustomEvent* );
      virtual void timerEvent( QTimerEvent* );

      void eject();

      void announceStateChange() { emit stateChanged( state() ); }

      xine_osd_t         *m_osd;
      xine_stream_t      *m_stream;
      xine_event_queue_t *m_eventQueue;
      xine_video_port_t  *m_videoPort;
      xine_audio_port_t  *m_audioPort;
      xine_post_t        *m_scope;
      xine_t             *m_xine;

      int64_t m_current_vpts;

      KURL m_url;

   public:
      QString fileFilter() const;

   public slots:
      void toggleDVDMenu();
      void showOSD( const QString& );

   /// Stuff to do with video and the video window/widget
   private:
      static void destSizeCallBack( void*, int, int, double, int*, int*, double* );
      static void frameOutputCallBack( void*, int, int, double, int*, int*, int*, int*, double*, int*, int* );

      void initVideo();
      void cleanUpVideo();

   public:
      static const uint CURSOR_HIDE_TIMEOUT = 2000;

      virtual QSize sizeHint() const;
      virtual QSize minimumSizeHint() const;

      void *x11Visual() const;
      void becomePreferredSize();
      QImage captureFrame() const;

      enum { ExposeEvent = 3000 };

   public slots:
      void resetZoom();

   private slots:
      void hideCursor();

   private:
      virtual void contextMenuEvent( QContextMenuEvent* );
      virtual bool event( QEvent* );
      virtual bool x11Event( XEvent* );

      double m_displayRatio;
      QTimer m_timer;
   };

   //global function for general use by Codeine
   //videoWindow() is const for Xlib-thread-safety reasons
   inline VideoWindow* const videoWindow() { return VideoWindow::s_instance; }
   inline VideoWindow* const engine() { return VideoWindow::s_instance; }
}

#endif
