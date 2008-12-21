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
#include "timeLabel.h"

#include <QList>
#include <QPointer>
#include <QCheckBox>

#include <KXmlGuiWindow>

class KNotificationRestrictions;
class KUrl;
class QActionGroup;
class QLabel;
class QMenu;
class QSlider;
class QCheckBox;

class FullScreenAction;

namespace Codeine
{
   class PlayDialog;
   class MainWindow : public KXmlGuiWindow
   {
   Q_OBJECT

      MainWindow();
     ~MainWindow();

      static MainWindow *s_instance;

      friend int ::main( int, char** );
      friend QWidget* mainWindow();

   public:
      void openRecentFile( const KUrl& );
      bool open( const KUrl& );
      void showVolume( bool );

   signals:
      void dbusStatusChanged( int );
      void fileChanged( QString );

   public slots:
      void play();
      void playMedia( bool show_welcome_dialog = false );
      void toggleVideoSettings( bool );
      void toggleVolumeSlider( bool );
      void playDialogResult( int result );

   private slots:
      void setFullScreen( bool full );
      void engineMessage( const QString& );
      void engineStateChanged( Engine::State );
      void init();
      void aboutToShowMenu();
      void streamSettingChange();
      void subChannelsChanged( QList< QAction* > );
      void audioChannelsChanged( QList< QAction* > );
      void mutedChanged( bool );

   private:
      void playDisc();
      void setupActions();
      void updateSliders();

      bool load( const KUrl& );

      QMenu *menu( const char *name );

      virtual void dragEnterEvent( QDragEnterEvent* );
      virtual void dropEvent( QDropEvent* );
      virtual void keyPressEvent( QKeyEvent* );

//      virtual void saveProperties( KConfig* );
//      virtual void readProperties( KConfig* );

      QPointer<QDockWidget> m_leftDock;
      QPointer<QDockWidget> m_rightDock;
      QWidget     *m_positionSlider;
      QPointer<QWidget> m_volumeSlider;
      QCheckBox *m_muteCheckBox;
      TimeLabel   *m_timeLabel;
      QLabel      *m_titleLabel;
      QList<QSlider*> m_sliders;
      QPointer<PlayDialog> m_playDialog;
      FullScreenAction *m_fullScreenAction;
      KNotificationRestrictions *m_stopScreenSaver;
	  bool	m_toolbarIsHidden;
      bool  m_statusbarIsHidden;

      QActionGroup *m_aspectRatios;
      //undefined
      MainWindow( const MainWindow& );
      MainWindow &operator=( const MainWindow& );

   protected:
      void wheelEvent ( QWheelEvent * event );
   };

}



#endif
