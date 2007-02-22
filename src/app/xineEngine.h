// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINE_VIDEOWINDOW_H
#define CODEINE_VIDEOWINDOW_H

#include "codeine.h"
#include <qwidget.h>

#include <kurl.h>

namespace Codeine
{
   /** Functions declared here are defined in:
    *    xineEngine.cpp
    *    videoWindow.cpp
    */
   class VideoWindow : public QWidget
   {
   Q_OBJECT

      static VideoWindow *s_instance;

      VideoWindow( const VideoWindow& ); //disable
      VideoWindow &operator=( const VideoWindow& ); //disable
      
      KUrl m_url;

      friend class TheStream;
      friend VideoWindow* const engine();
      friend VideoWindow* const videoWindow();

   public:
      VideoWindow( QWidget *parent );
     ~VideoWindow();

      bool init();
      void exit();

      bool load( const KUrl &url );
      bool play( uint = 0 );

      uint position() const { return 0; }
      uint time() const { return 0; }
      uint length() const { return 0; }

      uint volume() const;

      Engine::State state() const;

   /// Stuff to do with video and the video window/widget
      static const uint CURSOR_HIDE_TIMEOUT = 2000;


      void becomePreferredSize();

      enum { ExposeEvent = 3000 };

      QString fileFilter() const;

   public slots:
      void pause();
      void record();
      void seek( uint );
      void stop();

      ///special slot, see implementation to facilitate understanding
      void setStreamParameter( int );

   Q_SIGNALS:
      void stateChanged( Engine::State );
      void statusMessage( const QString& );
      void titleChanged( const QString& );
      void channelsChanged( const QStringList& );
   
   public slots:
      void toggleDVDMenu();
      void showOSD( const QString& );

   };

   //global function for general use by Codeine
   //videoWindow() is const for Xlib-thread-safety reasons
   inline VideoWindow* const videoWindow() { return VideoWindow::s_instance; }
   inline VideoWindow* const engine() { return VideoWindow::s_instance; }
}

#endif
