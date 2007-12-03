// (c) 2004 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINEMAINWINDOW_H
#define CODEINEMAINWINDOW_H

#include "codeine.h"


#include <kxmlguiwindow.h>

class KUrl;
class QActionGroup;
class QLabel;
class QMenu;
class QSlider;



namespace Codeine
{
   class MainWindow : public KXmlGuiWindow
   {
   Q_OBJECT

      MainWindow();
     ~MainWindow();

      friend int ::main( int, char** );

      enum { SubtitleChannelsMenuItemId = 2000, AudioChannelsMenuItemId, AspectRatioMenuItemId };

   public slots:
      void play();
      void playMedia( bool show_welcome_dialog = false );

      void streamInformation();

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

      QMenu *menu( const char *name );

      virtual void timerEvent( QTimerEvent* );
      virtual void dragEnterEvent( QDragEnterEvent* );
      virtual void dropEvent( QDropEvent* );
      virtual void keyPressEvent( QKeyEvent* );

//      virtual void saveProperties( KConfig* );
//      virtual void readProperties( KConfig* );

      virtual bool queryExit();

      QWidget *m_positionSlider;
      QLabel  *m_timeLabel;
      QLabel  *m_titleLabel;

      QActionGroup *m_aspectRatios;
      //undefined
      MainWindow( const MainWindow& );
      MainWindow &operator=( const MainWindow& );
   };
}

#endif
