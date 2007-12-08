/***********************************************************************
 * Copyright 2004  Max Howell <max.howell@methylblue.com>
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

#ifndef CODEINEMAINWINDOW_H
#define CODEINEMAINWINDOW_H

#include "codeine.h"


#include <KXmlGuiWindow>

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

      static MainWindow *s_instance;

      friend int ::main( int, char** );
      friend QWidget* mainWindow();

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
      void aboutToShowMenu();
      void fullScreenToggled( bool );
      void streamSettingChange();

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
