// (c) 2004 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINEMAINWINDOW_H
#define CODEINEMAINWINDOW_H

#include "codeine.h"
#include <kmainwindow.h>
//Added by qt3to4:
#include <Q3PopupMenu>
#include <QTimerEvent>
#include <QKeyEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QLabel>

class KUrl;
class QLabel;
class Q3PopupMenu;
class QSlider;


namespace Codeine
{
   class MainWindow : public KMainWindow
   {
   Q_OBJECT

      MainWindow();
     ~MainWindow();

      friend int ::main( int, char** );

      enum { SubtitleChannelsMenuItemId = 2000, AudioChannelsMenuItemId, AspectRatioMenuItemId };

   public slots:
      void play();
      void playMedia( bool show_welcome_dialog = false );

      void configure();
      void streamInformation();
      void captureFrame();

   private slots:
      void engineMessage( const QString& );
      void engineStateChanged( Engine::State );
      void init();
      void showTime( int = -1 );
      void setChannels( const QStringList& );
      void aboutToShowMenu();
      void fullScreenToggled( bool );

   private:
      void setupActions();

      bool load( const KUrl& );
      bool open( const KUrl& );

      Q3PopupMenu *menu( const char *name );

      virtual void timerEvent( QTimerEvent* );
      virtual void dragEnterEvent( QDragEnterEvent* );
      virtual void dropEvent( QDropEvent* );
      virtual void keyPressEvent( QKeyEvent* );

      virtual void saveProperties( KConfig* );
      virtual void readProperties( KConfig* );

      virtual bool queryExit();

      QSlider *m_positionSlider;
      QLabel  *m_timeLabel;
      QLabel  *m_titleLabel;
      QWidget *m_analyzer;

      //undefined
      MainWindow( const MainWindow& );
      MainWindow &operator=( const MainWindow& );
   };
}

#endif
