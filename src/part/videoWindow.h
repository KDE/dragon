// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINE_VIDEO_WINDOW_H
#define CODEINE_VIDEO_WINDOW_H

#include "../codeine.h"
#include <qtimer.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QEvent>
#include <QCustomEvent>
#include <kurl.h>

typedef struct xine_s xine_t;
typedef struct xine_stream_s xine_stream_t;
typedef struct xine_video_port_s xine_video_port_t;
typedef struct xine_audio_port_s xine_audio_port_t;
typedef struct xine_event_queue_s xine_event_queue_t;
typedef struct xine_post_s xine_post_t;
typedef struct xine_osd_s xine_osd_t;


namespace Codeine
{
   class VideoWindow : public QWidget
   {
      Q_OBJECT

      static VideoWindow *s_instance;
      static const uint CURSOR_HIDE_TIMEOUT = 2000;

      friend VideoWindow* const videoWindow();

   public:
      VideoWindow( QWidget *parent, const char *name );
     ~VideoWindow();

      bool init();

      bool play( KURL );
      void eject();

      int position();

   signals:
      void statusMessage( const QString& );
      void titleChanged( const QString& );

   private:
      /// @see xineEngine.cpp
      #ifdef HAVE_XINE_H
      static void xineEventListener( void*, const xine_event_t* );
      #endif

      void showErrorMessage(); //TODO don't use this, just show delayed message

      virtual void customEvent( QCustomEvent* );
      virtual bool x11Event( XEvent* );
      virtual bool event( QEvent* );

      xine_osd_t         *m_osd;
      xine_stream_t      *m_stream;
      xine_event_queue_t *m_eventQueue;
      xine_video_port_t  *m_videoPort;
      xine_audio_port_t  *m_audioPort;
      xine_t             *m_xine;

      KURL m_url;

   private:
      void *x11Visual() const;

      static void destSizeCallBack( void*, int, int, double, int*, int*, double* );
      static void frameOutputCallBack( void*, int, int, double, int*, int*, int*, int*, double*, int*, int* );

      double m_displayRatio;
      QTimer m_timer;

   public slots:
      void togglePlay();
      void toggleMute();

   private slots:
      void hideCursor();

   private:
      /// prevent compiler generated functions
      VideoWindow( const VideoWindow& );
      VideoWindow &operator=( const VideoWindow& );
      bool operator==( const VideoWindow& );
   };

   inline VideoWindow* const videoWindow() { return VideoWindow::s_instance; }
}

#endif
