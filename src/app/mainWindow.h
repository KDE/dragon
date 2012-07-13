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

#ifndef DRAGONPLAYERMAINWINDOW_H
#define DRAGONPLAYERMAINWINDOW_H

#include "codeine.h"
#include "timeLabel.h"
#include "loadView.h"

#include <QList>
#include <QPointer>
#include <QStackedWidget>
#include <QCheckBox>
#include <Phonon/MediaSource>
#include <KXmlGuiWindow>

class KNotificationRestrictions;
class KToggleAction;
class KUrl;
class QActionGroup;
class QCloseEvent;
class QLabel;
class QMenu;
class QSlider;
class QCheckBox;

class AudioView2;
#include "audioView2.h"

namespace Dragon
{
   class PlayDialog;
   class FullScreenToolBarHandler;

   class MainWindow : public KXmlGuiWindow
   {
   Q_OBJECT

      MainWindow();
     ~MainWindow();

      static MainWindow *s_instance;

      friend class PlayerApplication;
      friend QWidget* mainWindow();

   public:
      void openRecentFile( const KUrl& );
      void showVolume( bool );
      bool volumeContains( QPoint mousePos );

   signals:
      void fileChanged( QString );

   public slots:
      bool open( const KUrl& );
      void playDisc();
      void openFileDialog();
      void play();
      void toggleVideoSettings( bool );
      void toggleVolumeSlider( bool );
      void restoreDefaultVideoSettings();
      void toggleLoadView();
      void parseArgs();

   private slots:
      void setFullScreen( bool full );
      void engineMessage( const QString& );
      void init();
      void aboutToShowMenu();
      void streamSettingChange();
      void subChannelsChanged( QList< QAction* > );
      void audioChannelsChanged( QList< QAction* > );
      void mutedChanged( bool );
      void stop();
      //in stateChange.cpp
      void engineStateChanged( Phonon::State, Phonon::State = Phonon::StoppedState);
      void engineMediaChanged( Phonon::MediaSource );
      void engineSeekableChanged( bool );
      void engineMetaDataChanged();
      void engineHasVideoChanged( bool );
      void toggleUnique( bool );

   private:
      bool load( const KUrl& );
      void setupActions();
      void updateSliders();
      void updateTitleBarText();


      QMenu *menu( const char *name );

      virtual void dragEnterEvent( QDragEnterEvent* );
      virtual void dropEvent( QDropEvent* );
      virtual void keyPressEvent( QKeyEvent* );

      void inhibitPowerSave();
      void releasePowerSave();

//      virtual void saveProperties( KConfig* );
//      virtual void readProperties( KConfig* );

      QStackedWidget *m_mainView;
      AudioView2 *m_audioView;
      LoadView *m_loadView;
      QWidget *m_currentWidget;

      QPointer<QDockWidget> m_leftDock;
      QPointer<QDockWidget> m_rightDock;
      QWidget *m_positionSlider;
      QPointer<QWidget> m_volumeSlider;
      QCheckBox *m_muteCheckBox;
      TimeLabel *m_timeLabel;
      QLabel *m_titleLabel;
      QList<QSlider*> m_sliders;
      QPointer<PlayDialog> m_playDialog;

      KToggleAction *m_menuToggleAction;

      KNotificationRestrictions *m_stopScreenSaver;
      int m_screensaverDisableCookie;
      int m_stopSleepCookie;
      int m_stopScreenPowerMgmtCookie;

      bool m_toolbarIsHidden;
      bool m_statusbarIsHidden;
      bool m_menuBarIsHidden;
      FullScreenToolBarHandler *m_FullScreenHandler;

      QActionGroup *m_aspectRatios;
      Q_DISABLE_COPY(MainWindow)

   protected:
      void closeEvent( QCloseEvent * event );
      void wheelEvent ( QWheelEvent * event );
   };

}



#endif
