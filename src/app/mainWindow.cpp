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

#include "mainWindow.h"
#include "timeLabel.h"

#include <KApplication>
#include <KCmdLineArgs>
#include <KCursor>
#include <KFileDialog>      //::open()
#include <KGlobalSettings> //::timerEvent()
#include <KIO/NetAccess>
#include <KLocale>
#include <KMenuBar>
#include <KSqueezedTextLabel>
#include <KStatusBar>
#include <KToggleFullScreenAction>
#include <KToolBar>
#include <KWindowSystem>
#include <KXMLGUIFactory>

#include <Phonon/VideoWidget>

#include <Solid/Device>
#include <Solid/OpticalDisc>

#include <QActionGroup>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>        //::stateChanged()
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>      //ctor
#include <QMouseEvent>
#include <QObject>
#include <QTimer>
#include <QStackedWidget>

#include "actions.h"
#include "discSelectionDialog.h"
#include "dbus/playerDbusHandler.h"
#include "dbus/rootDbusHandler.h"
#include "dbus/trackListDbusHandler.h"
#include "debug.h"
#include "extern.h"         //dialog creation function definitions
#include "fullScreenToolBarHandler.h"
#include "messageBox.h"
#include "playDialog.h"  //::play()
#include "playlistFile.h"
#include "theStream.h"
#include "ui_videoSettingsWidget.h"
#include "videoWindow.h"
#include "audioView.h"
#include "loadView.h"

#include <phonon/backendcapabilities.h>

namespace Dragon {

    MainWindow *MainWindow::s_instance = 0;
    /// @see codeine.h
    QWidget* mainWindow() { return MainWindow::s_instance; }



MainWindow::MainWindow()
        : KXmlGuiWindow()
        , m_mainView( 0 )
        , m_audioView(new AudioView(this) )
        , m_loadView( new LoadView(this) )
        , m_currentWidget( new QWidget(this) )
        , m_leftDock( 0 )
        , m_positionSlider( 0 )
        , m_volumeSlider( 0 )
        , m_timeLabel( 0 )
        , m_titleLabel( new QLabel( this ) )
        , m_playDialog( 0 )
        , m_stopScreenSaver( 0 )
        , m_stopSleepCookie( -1 )
        , m_toolbarIsHidden(false)
        , m_statusbarIsHidden(false)
{
    DEBUG_BLOCK
    s_instance = this;
    setMouseTracking( true );

    m_mainView = new QStackedWidget(this);
    m_mainView->setMouseTracking( true );

    new VideoWindow( this );
    videoWindow()->setMouseTracking( true );

    m_positionSlider = videoWindow()->newPositionSlider();
    
    m_mainView->addWidget(m_loadView);
    m_mainView->addWidget(m_audioView);
    m_mainView->addWidget(videoWindow());
    m_mainView->setCurrentWidget(m_loadView);
    
    m_currentWidget = m_loadView;

    setCentralWidget( m_mainView );

    setFocusProxy( videoWindow() ); // essential! See VideoWindow::event(), QEvent::FocusOut

    m_titleLabel->setMargin( 2 );
    m_titleLabel->setSizePolicy(QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ));

    // work around a bug in KStatusBar
    // sizeHint width of statusbar seems to get stupidly large quickly
    statusBar()->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Maximum );

    setupActions();
    
    //setStandardToolBarMenuEnabled( false ); //bah to setupGUI()!
    //toolBar()->show(); //it's possible it would be hidden, but we don't want that as no UI way to show it!

    {
        KActionCollection* ac = actionCollection();
        QMenu *menu = 0;
        QAction *menuAction = 0; 
        #define make_menu( name, text ) \
                menu = new QMenu( text ); \
                menuAction = menu->menuAction(); \
                menuAction->setObjectName( name ); \
                menuAction->setEnabled( false ); \
                connect( menu, SIGNAL(aboutToShow()), SLOT(aboutToShowMenu()) ); \
                ac->addAction( menuAction->objectName(), menuAction );
        make_menu( "aspect_ratio_menu", i18n( "Aspect &Ratio" ) );
        make_menu( "audio_channels_menu", i18n( "&Audio Channels" ) );
        make_menu( "subtitle_channels_menu", i18n( "&Subtitles" ) );
        #undef make_menu

        {
            m_aspectRatios = new QActionGroup( this );
            m_aspectRatios->setExclusive( true );
            #define make_ratio_action( text, objectname, aspectEnum ) \
            { \
                KAction* ratioAction = new KAction( this ); \
                ratioAction->setText( text ); \
                ratioAction->setCheckable( true ); \
                m_aspectRatios->addAction( ratioAction ); \
                TheStream::addRatio( aspectEnum, ratioAction ); \
                ac->addAction( objectname, ratioAction ); \
                connect( ratioAction, SIGNAL( triggered() ), this, SLOT( streamSettingChange() ) ); \
            }
            make_ratio_action( i18n( "Determine &Automatically" ), "ratio_auto",  Phonon::VideoWidget::AspectRatioAuto );
            make_ratio_action( i18n( "&4:3" ), "ratio_golden", Phonon::VideoWidget::AspectRatio4_3 );
            make_ratio_action( i18n( "Ana&morphic (16:9)" ), "ratio_anamorphic", Phonon::VideoWidget::AspectRatio16_9 );
            make_ratio_action( i18n( "&Window Size" ), "ratio_window", Phonon::VideoWidget::AspectRatioWidget );
            #undef make_ratio_action
            ac->action( "ratio_auto" )->setChecked( true );
            ac->action( "aspect_ratio_menu" )->menu()->addActions( m_aspectRatios->actions() );
        }
    }

    setupGUI(); //load xml dragonplayerui.rc file
    //must be done after setupGUI:
    {
        toolBar()->setAllowedAreas( Qt::TopToolBarArea | Qt::BottomToolBarArea );
        toolBar()->setFloatable( false );
    }
    KXMLGUIClient::stateChanged( "empty" );

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if( args->count() || args->isSet( "play-dvd" ) || kapp->isSessionRestored() )
        //we need to resize the window, so we can't show the window yet
        init();
    else 
    {
        //"faster" startup
        //TODO if we have a size stored for this video, do the "faster" route
        QTimer::singleShot( 0, this, SLOT(init()) );
        QApplication::setOverrideCursor( Qt::WaitCursor ); 
    }
}

void
MainWindow::init()
{
//     DEBUG_BLOCK
    //connect the stuff in loadView
    connect( m_loadView, SIGNAL(openDVDPressed()), this, SLOT(playDisc()) );
    connect( m_loadView, SIGNAL(openFilePressed()), this, SLOT(openFileDialog()) );
    connect( m_loadView, SIGNAL(loadUrl(KUrl)), this, SLOT(open(KUrl)) );

    //connect the video player
    connect( engine(), SIGNAL( stateUpdated( Phonon::State, Phonon::State ) ), this, SLOT( engineStateChanged( Phonon::State, Phonon::State ) ) );
    connect( engine(), SIGNAL( currentSourceChanged( Phonon::MediaSource ) ), this, SLOT( engineMediaChanged( Phonon::MediaSource ) ) );
    connect( engine(), SIGNAL( seekableChanged( bool ) ), this, SLOT( engineSeekableChanged( bool ) ) );
    connect( engine(), SIGNAL( metaDataChanged() ), this, SLOT( engineMetaDataChanged() ) );
    connect( engine(), SIGNAL( hasVideoChanged( bool ) ), this, SLOT( engineHasVideoChanged( bool ) ) );

    connect( engine(), SIGNAL( subChannelsChanged( QList< QAction* > ) ), this, SLOT( subChannelsChanged( QList< QAction* > ) ) );
    connect( engine(), SIGNAL( audioChannelsChanged( QList< QAction* > ) ), this, SLOT( audioChannelsChanged( QList< QAction* > ) ) );
    connect( engine(), SIGNAL( mutedChanged( bool ) ), this, SLOT( mutedChanged( bool ) ) );

    if( !engine()->init() ) {
        KMessageBox::error( this, i18n(
            "<qt>xine could not be successfully initialised. Dragon Player will now exit. "
            "You can try to identify what is wrong with your xine installation using the <b>xine-check</b> command at a command-prompt.</qt>") );
        QApplication::exit( 2 );
    }

    //would be dangerous for these to65535 happen before the videoWindow() is initialised
    setAcceptDrops( true );
    connect( statusBar(), SIGNAL(messageChanged( const QString& )), engine(), SLOT(showOSD( const QString& )) );
    //statusBar()->insertPermanentItem( "hello world", 0, 0 );
    m_timeLabel = new TimeLabel( statusBar() );
    connect( videoWindow(), SIGNAL( tick( qint64) ), m_timeLabel, SLOT( setCurrentTime( qint64 ) ) );
    connect( videoWindow(), SIGNAL( totalTimeChanged( qint64 ) ), m_timeLabel, SLOT( setTotalTime( qint64 ) ) );
    statusBar()->addPermanentWidget( m_titleLabel, 100 );
    statusBar()->addPermanentWidget( m_timeLabel );

    Mpris::registerTypes();
    new PlayerDbusHandler( this );
    new RootDbusHandler( this );
    new TrackListDbusHandler( this );

    QApplication::restoreOverrideCursor();
    engineStateChanged(Phonon::StoppedState);//set everything as it would be in stopped state
    engineSeekableChanged(false);

    if( !kapp->isSessionRestored() ) {
        KCmdLineArgs &args = *KCmdLineArgs::parsedArgs();
        if (args.isSet( "play-dvd" ))
            engine()->playDvd();
        else if (args.count() > 0 ) {
            this->open( args.url( 0 ) );
            args.clear();
            adjustSize(); //will resize us to reflect the videoWindow's sizeHint()
        }
    }
    else
        //session management must be done after the videoWindow() has been initialised
        restore( 1, false );
}

MainWindow::~MainWindow()
{
    DEBUG_BLOCK
    hide(); //so we appear to have quit, and then sound fades out below
    delete videoWindow(); //fades out sound in dtor
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    // Restore the state of these before closing
    mainWindow()->setWindowState( Qt::WindowNoState );
    statusBar()->setHidden( m_statusbarIsHidden );
    toolBar()->setHidden( m_toolbarIsHidden );
    menuBar()->setHidden( false );
    
    KMainWindow::closeEvent( event );
}

void MainWindow::wheelEvent (QWheelEvent *event)
 {
    if (event->delta() > 0) {
	engine()->tenPercentBack();
      } else {
	engine()->tenPercentForward();
    }
    event->accept();
 }

void
MainWindow::setupActions()
{
    DEBUG_BLOCK

    KActionCollection * const ac = actionCollection();

    KStandardAction::quit( kapp, SLOT( closeAllWindows() ), ac );

    KStandardAction::open( this, SLOT(toggleLoadView()), ac )->setText( i18n("Play &Media...") );

    #define addToAc( X ) ac->addAction( X->objectName(), X );

    KToggleFullScreenAction* toggleFullScreen = new KToggleFullScreenAction( this, ac );
    toggleFullScreen->setObjectName( "fullscreen" );
    toggleFullScreen->setShortcut( Qt::Key_F );
    toggleFullScreen->setAutoRepeat( false );
    connect( toggleFullScreen, SIGNAL( toggled( bool ) ), Dragon::mainWindow(), SLOT( setFullScreen( bool ) ) );
    addToAc( toggleFullScreen );

    new PlayAction( this, SLOT( play() ), ac );
    new VolumeAction( this, SLOT( toggleVolumeSlider( bool ) ), ac );

    KAction* playerStop = new KAction( KIcon("media-playback-stop"), i18n("Stop"), ac );
    playerStop->setObjectName( "stop" );
    playerStop->setShortcut( Qt::Key_S );
    connect( playerStop, SIGNAL( triggered() ), engine(), SLOT( stop() ) );
    addToAc( playerStop )

    KToggleAction* mute = new KToggleAction( KIcon("player-volume-muted"), i18nc( "Mute the sound output", "Mute"), ac );
    mute->setObjectName( "mute" );
    mute->setShortcut( Qt::Key_M );
    connect( mute, SIGNAL( toggled( bool ) ), videoWindow(), SLOT( mute( bool ) ) );
    addToAc( mute )

    KAction* resetZoom = new KAction( KIcon("zoom-fit-best"), i18n("Reset Video Scale"), ac );
    resetZoom->setObjectName( "reset_zoom" );
    resetZoom->setShortcut( Qt::Key_Equal );
    connect( resetZoom, SIGNAL( triggered() ), videoWindow(), SLOT( resetZoom() ) );
    addToAc( resetZoom )

    KAction* dvdMenu = new KAction( KIcon("media-optical-video"), i18n("Menu Toggle"), ac );
    dvdMenu->setObjectName( "toggle_dvd_menu" );
    dvdMenu->setShortcut( Qt::Key_R );
    connect( dvdMenu, SIGNAL( triggered() ), engine(), SLOT( toggleDVDMenu() ) );
    addToAc( dvdMenu )

    KAction* positionSlider = new KAction( i18n("Position Slider"), ac );
    positionSlider->setObjectName( "position_slider" );
    positionSlider->setDefaultWidget( m_positionSlider );
    addToAc( positionSlider )

    KAction* videoSettings = new KAction( i18n("Video Settings"), ac );
    videoSettings->setObjectName( "video_settings" );
    videoSettings->setCheckable( true );
    connect( videoSettings, SIGNAL( toggled( bool ) ), this, SLOT( toggleVideoSettings( bool ) ) );
    addToAc( videoSettings )

    KAction* prev_chapter = new KAction( KIcon("media-skip-backward"), i18n("Previous Chapter"), ac );
    prev_chapter->setObjectName( "prev_chapter" );
    prev_chapter->setShortcut( Qt::Key_Comma );
    connect( prev_chapter, SIGNAL( triggered() ), engine(), SLOT( prevChapter() ) );
    addToAc( prev_chapter )

    KAction* next_chapter = new KAction( KIcon("media-skip-forward"), i18n("Next Chapter"), ac );
    next_chapter->setObjectName( "next_chapter" );
    next_chapter->setShortcut( Qt::Key_Period );
    connect( next_chapter, SIGNAL( triggered() ), engine(), SLOT( nextChapter() ) );
    addToAc( next_chapter )

    // xgettext: no-c-format
    KAction* tenPercentBack = new KAction( KIcon("media-seek-backward"), i18n("Return 10% Back"), ac );
    tenPercentBack->setObjectName( "ten_percent_back" );
    tenPercentBack->setShortcut( Qt::Key_PageUp );
    connect( tenPercentBack, SIGNAL( triggered() ), engine(), SLOT( tenPercentBack() ) );
    addToAc( tenPercentBack )

    // xgettext: no-c-format
    KAction* tenPercentForward = new KAction( KIcon("media-seek-forward"), i18n("Go 10% Forward"), ac );
    tenPercentForward->setObjectName( "ten_percent_forward" );
    tenPercentForward->setShortcut( Qt::Key_PageDown );
    connect( tenPercentForward, SIGNAL( triggered() ), engine(), SLOT( tenPercentForward() ) );
    addToAc( tenPercentForward )

    KAction* tenSecondsBack = new KAction( KIcon("media-seek-backward"), i18n("Return 10 Seconds Back"), ac );
    tenSecondsBack->setObjectName( "ten_seconds_back" );
    tenSecondsBack->setShortcut( Qt::Key_Minus );
    connect( tenSecondsBack, SIGNAL( triggered() ), engine(), SLOT( tenSecondsBack() ) );
    addToAc( tenSecondsBack )

    KAction* tenSecondsForward = new KAction( KIcon("media-seek-forward"), i18n("Go 10 Seconds Forward"), ac );
    tenSecondsForward->setObjectName( "ten_seconds_forward" );
    tenSecondsForward->setShortcut( Qt::Key_Plus );
    connect( tenSecondsForward, SIGNAL( triggered() ), engine(), SLOT( tenSecondsForward() ) );
    addToAc( tenSecondsForward )
    #undef addToAc
}

void
MainWindow::toggleVideoSettings( bool show )
{
    if( show )
    {
        m_leftDock = new QDockWidget( this );
        m_leftDock->setObjectName("left_dock");
        m_leftDock->setFeatures( QDockWidget::NoDockWidgetFeatures );
        QWidget* videoSettingsWidget = new QWidget( m_leftDock );
        m_leftDock->setWidget( videoSettingsWidget );
        Ui::VideoSettingsWidget ui;
        ui.setupUi( videoSettingsWidget );
        videoSettingsWidget->adjustSize();
        addDockWidget( Qt::LeftDockWidgetArea, m_leftDock );
        m_sliders.clear();
        m_sliders << ui.brightnessSlider << ui.contrastSlider << ui.hueSlider <<  ui.saturationSlider;
        updateSliders();
        foreach( QSlider* slider, m_sliders )
             connect( slider, SIGNAL( valueChanged( int ) ), engine(), SLOT( settingChanged( int ) ) );
        
        connect( ui.defaultsButton, SIGNAL( clicked( bool ) ), this, SLOT( restoreDefaultVideoSettings() ) );
        connect( ui.closeButton, SIGNAL( clicked( bool ) ), action( "video_settings" ), SLOT( setChecked( bool ) ) );
        connect( ui.closeButton, SIGNAL( clicked( bool ) ), m_leftDock, SLOT( deleteLater() ) );
    }
    else
    {
        m_sliders.clear();
        delete m_leftDock;
    }
}

void
MainWindow::restoreDefaultVideoSettings()
{
    foreach( QSlider* slider, m_sliders )
        slider->setValue(0);
}

void
MainWindow::toggleLoadView()
{
  DEBUG_BLOCK;
  if( engine()->state() == Phonon::PlayingState && TheStream::hasVideo() )
  {
    engine()->playPause();
  }
    if( m_mainView->currentWidget() == m_loadView )
    {
      if( m_mainView->indexOf(m_currentWidget) == -1 )
      {
        m_mainView->addWidget(m_currentWidget);
        engine()->playPause();
      }
      m_mainView->setCurrentWidget(m_currentWidget);
      engine()->isPreview(false);
    }
    else if( m_currentWidget != m_audioView )
    {
      debug() << "setting Thumbnail for video Widget";
      m_mainView->setCurrentWidget(m_loadView);
      m_mainView->removeWidget(m_currentWidget);
      engine()->isPreview(true);
      m_loadView->setThumbnail(m_currentWidget);
    }
    else 
    {
      m_mainView->setCurrentWidget(m_loadView);
    }
}

void
MainWindow::toggleVolumeSlider( bool show )
{
    if( show )
    {
        m_volumeSlider = engine()->newVolumeSlider();
        m_volumeSlider->setDisabled ( engine()->isMuted() );

        m_muteCheckBox = new QCheckBox();
        m_muteCheckBox->setText( i18nc( "Mute the sound output", "Mute " ) );
        m_muteCheckBox->setChecked ( engine()->isMuted() );
        connect( m_muteCheckBox, SIGNAL( toggled( bool ) ), videoWindow(), SLOT( mute( bool ) ) );

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(m_volumeSlider);
        layout->addWidget(m_muteCheckBox);

        QWidget *dock = new QWidget;
        dock->setLayout(layout);

        m_rightDock = new QDockWidget( this );
        m_rightDock->setFeatures( QDockWidget::NoDockWidgetFeatures );
        dock->setParent( m_rightDock );
        m_rightDock->setWidget( dock );
        addDockWidget( Qt::RightDockWidgetArea, m_rightDock );
    }
    else
    {
        disconnect( m_muteCheckBox, SIGNAL( toggled( bool ) ), videoWindow(), SLOT( mute( bool ) ) );
        delete m_rightDock;
    }
}

void
MainWindow::mutedChanged( bool mute )
{
    if( m_rightDock )
      {
        m_volumeSlider->setDisabled ( mute );
        m_muteCheckBox->setChecked ( mute );
      }
}

void
MainWindow::updateSliders()
{
    foreach( QSlider* slider, m_sliders )
        slider->setValue( engine()->videoSetting( slider->objectName() ) );
}

void
MainWindow::engineMessage( const QString &message )
{
    DEBUG_BLOCK
    statusBar()->showMessage( message, 3500 );
}

bool
MainWindow::open( const KUrl &url )
{
    DEBUG_BLOCK
    debug() << url;

    if( load( url ) ) {
        const int offset = TheStream::hasProfile()
                // adjust offset if we have session history for this video
                ? TheStream::profile().readEntry<int>( "Position", 0 )
                : 0;
        debug() << "Initial offset is "<< offset;
        engine()->loadSettings();
        updateSliders();
        if( TheStream::hasVideo() )
          m_currentWidget = engine();
        else
          m_currentWidget = m_audioView;
        m_mainView->setCurrentWidget(m_currentWidget);
        return engine()->play( offset );
    }

    return false;
}

bool
MainWindow::load( const KUrl &url )
{
     //FileWatch the file that is opened

    if( url.isEmpty() ) {
        MessageBox::sorry( i18n( "Dragon Player was asked to open an empty URL; it cannot." ) );
        return false;
    }

    PlaylistFile playlist( url );
    if( playlist.isPlaylist() ) {
        //TODO: problem is we return out of the function
        //statusBar()->message( i18n("Parsing playlist file...") );

        if( playlist.isValid() )
            return engine()->load( playlist.firstUrl() );
        else {
            MessageBox::sorry( playlist.error() );
            return false;
        }
    }

    if (url.protocol() == "media") {
        //#define UDS_LOCAL_PATH (72 | KIO::UDS_STRING)
        KIO::UDSEntry e;
        if (!KIO::NetAccess::stat( url, e, 0 ))
            MessageBox::sorry( i18n("There was an internal error with the media slave...") );
        else {
            QString path = e.stringValue( KIO::UDSEntry::UDS_LOCAL_PATH );
            if( !path.isEmpty() )
                return engine()->load( KUrl( path ) );
        }
    }
    
    if( m_mainView->indexOf(engine()) == -1 )
      toggleLoadView();

    //let xine handle invalid, etc, KUrlS
    //TODO it handles non-existing files with bad error message
    return engine()->load( url );
}

void
MainWindow::play()
{
    switch( engine()->state() ) {
    case Phonon::PlayingState:
        engine()->pause();
        break;
    case Phonon::PausedState:
        engine()->resume();
        if( m_mainView->currentWidget() == m_loadView )
          toggleLoadView();
        break;
    case Phonon::StoppedState:
        if( TheStream::hasVideo() )
          m_currentWidget = engine();
        else
          m_currentWidget = m_audioView;
        engine()->play();
        m_mainView->setCurrentWidget(engine());
        break;
    default:
        break;
    }
}

void
MainWindow::openFileDialog()
{
       QStringList mimeFilter=Phonon::BackendCapabilities::availableMimeTypes();
        //temporary fixes for MimeTypes that Xine does support but it doesn't return - this is a Xine bug.
        mimeFilter << "audio/x-flac";
        mimeFilter << "video/mp4";
        mimeFilter << "application/x-cd-image"; // added for *.iso images

        const KUrl url = KFileDialog::getOpenUrl( KUrl("kfiledialog:///dragonplayer"),mimeFilter.join(" "), this, i18n("Select File to Play") );
        if( url.isEmpty() )
        {
            debug() << "URL empty in MainWindow::playDialogResult()";
            return;
        }
        else
        {
            load( url );
        }
}

void
MainWindow::playDisc()
{
DEBUG_BLOCK
    QList< Solid::Device > playableDiscs;
    {
        QList< Solid::Device > deviceList = Solid::Device::listFromType( Solid::DeviceInterface::OpticalDisc );
        
        foreach( const Solid::Device &device, deviceList )
        {
            const Solid::OpticalDisc* disc = device.as<const Solid::OpticalDisc>();
            if( disc )
            {
                if( disc->availableContent() & ( Solid::OpticalDisc::VideoDvd | Solid::OpticalDisc::VideoCd | Solid::OpticalDisc::SuperVideoCd |  Solid::OpticalDisc::Audio ) )
                    playableDiscs << device;
            
            }
        }
    }
    if( !playableDiscs.isEmpty() )
    {
        if( playableDiscs.size() > 1 ) //more than one disc, show user a selection box
        {
            debug() << "> 1 possible discs, showing dialog";
            new DiscSelectionDialog( this, playableDiscs );
        }
        else //only one optical disc inserted, play whatever it is
        {
            debug() << "playing disc", engine()->playDisc( playableDiscs.first() );
        }
    }
    else
        engine()->playDvd(), debug() << "no disc in drive or Solid isn't working";

}

void
MainWindow::openRecentFile( const KUrl& url )
{
    m_playDialog->deleteLater();
    m_playDialog = 0;
    this->open( url );
}

void
MainWindow::setFullScreen( bool isFullScreen )
{
    DEBUG_BLOCK
    debug() << "Setting full screen to " << isFullScreen;
    mainWindow()->setWindowState( (isFullScreen ? Qt::WindowFullScreen : Qt::WindowNoState ));
    static FullScreenToolBarHandler *s_handler;

	if(isFullScreen)
	{
      m_statusbarIsHidden=statusBar()->isHidden();
	  m_toolbarIsHidden=toolBar()->isHidden();
	  toolBar()->setHidden( false );
      statusBar()->setHidden( true );
	}
	else
	{
      statusBar()->setHidden(m_statusbarIsHidden);
	  toolBar()->setHidden(m_toolbarIsHidden);
	}
    menuBar()->setHidden( isFullScreen );
    if( m_leftDock )
        m_leftDock->setHidden( isFullScreen );
    if( m_rightDock )
        m_rightDock->setHidden( isFullScreen );

    if( isFullScreen )
        s_handler = new FullScreenToolBarHandler( this );
    else
    {
        action( "fullscreen" )->setEnabled( videoWindow()->state() ==  Phonon::PlayingState || videoWindow()->state() ==  Phonon::PausedState);
        delete s_handler;
    }
}

void
MainWindow::showVolume( bool visible)
{
    if( m_rightDock )
        m_rightDock->setVisible( visible );
}

void
MainWindow::aboutToShowMenu()
{
    DEBUG_BLOCK
    TheStream::aspectRatioAction()->setChecked( true );
    {
        int subId = TheStream::subtitleChannel();
        QList< QAction* > subs = action("subtitle_channels_menu")->menu()->actions();
        debug() << "subtitle #" << subId << " is going to be checked";
        foreach( QAction* subAction, subs )
        {
            if( subAction->property( TheStream::CHANNEL_PROPERTY ).toInt() == subId )
            {
                subAction->setChecked( true );
                break;
            }
            debug() << subAction->property( TheStream::CHANNEL_PROPERTY ).toInt() << " not checked.";
        }
    }
    {
        int audioId = TheStream::audioChannel();
        QList< QAction* > audios = action("audio_channels_menu")->menu()->actions();
        debug() << "audio #" << audioId << " is going to be checked";
        foreach( QAction* audioAction, audios )
        {
            if( audioAction->property( TheStream::CHANNEL_PROPERTY ).toInt() == audioId )
            {
                audioAction->setChecked( true );
                break;
            }
        }
    }
}

void
MainWindow::dragEnterEvent( QDragEnterEvent *e )
{
    KUrl::List uriList = KUrl::List::fromMimeData( e->mimeData() );
    e->setAccepted( !uriList.isEmpty() );
}

void
MainWindow::dropEvent( QDropEvent *e )
{
    KUrl::List uriList = KUrl::List::fromMimeData( e->mimeData() );
    if( !uriList.isEmpty() )
        this->open( uriList.first() );
    else
        engineMessage( i18n("Sorry, no media was found in the drop") );
}

void
MainWindow::keyPressEvent( QKeyEvent *e )
{
    switch( e->key() )
    {
        case Qt::Key_Left:  engine()->relativeSeek( -5000 ); break;
        case Qt::Key_Right: engine()->relativeSeek( 5000 ); break;
        case Qt::Key_Escape:    action("fullscreen")->setChecked( false );
        default: ;
    }

    #undef seek
}

QMenu*
MainWindow::menu( const char *name )
{
    // KXMLGUI is "really good".
    return static_cast<QMenu*>(factory()->container( name, this ));
}

void 
MainWindow::streamSettingChange()
{
    if( sender()->objectName().left( 5 ) == "ratio" )
    {
        TheStream::setRatio( dynamic_cast< QAction* > ( sender() ) );
    }
}

void
MainWindow::updateTitleBarText()
{
    if( !TheStream::hasMedia() )
    {
        m_titleLabel->setText( i18n("No media loaded") );
    }
    else if( engine()->state() == Phonon::PausedState )
    {
        m_titleLabel->setText( i18n("Paused") );
    }
    else
    {
        m_titleLabel->setText( TheStream::prettyTitle() );
    }
    debug() << "set titles ";
}

#define CHANNELS_CHANGED( function, actionName ) \
void \
MainWindow::function( QList< QAction* > subActions ) \
{ \
DEBUG_BLOCK \
    if( subActions.size() <= 2 ) \
          action( actionName )->setEnabled( false ); \
    else \
    { \
        action( actionName )->menu()->addActions( subActions ); \
        action( actionName )->setEnabled( true ); \
    } \
}

CHANNELS_CHANGED( subChannelsChanged  , "subtitle_channels_menu" )
CHANNELS_CHANGED( audioChannelsChanged, "audio_channels_menu" )
#undef CHANNELS_CHANGED

/// Convenience class for other classes that need access to the actionCollection
KActionCollection*
actionCollection()
{
    return static_cast<MainWindow*>(mainWindow())->actionCollection();
}

/// Convenience class for other classes that need access to the actions
QAction*
action( const char *name )
{
    KActionCollection *actionCollection = 0;
    QAction *action = 0;

    if( mainWindow() )
        if( ( actionCollection = ((MainWindow*)mainWindow() )->actionCollection() ) )
            action = actionCollection->action( name );
    if( !action )
        debug() << name;
    Q_ASSERT( mainWindow() );
    Q_ASSERT( actionCollection );
    Q_ASSERT( action );

    return action;
}


} //namespace Dragon

#include "mainWindow.moc"
