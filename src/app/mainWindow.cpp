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

#include <cstdlib>

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
#include <KToolBar>
#include <KWindowSystem>
#include <KXMLGUIFactory>

#include <Phonon/VideoWidget>

#include <QActionGroup>
#include <QDesktopWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>        //::stateChanged()
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>      //ctor
#include <QMouseEvent>
#include <QObject>
#include <QTimer>

#include "actions.h"
#include "debug.h"
#include "extern.h"         //dialog creation function definitions
#include "fullScreenAction.h"
#include "fullScreenToolBarHandler.h"
#include "mainWindow.h"
#include "mxcl.library.h"
#include "playDialog.h"  //::play()
#include "playlistFile.h"
#include "theStream.h"
#include "videoWindow.h"

namespace Codeine {

    MainWindow *MainWindow::s_instance = 0;
    /// @see codeine.h
    QWidget* mainWindow() { return MainWindow::s_instance; }



MainWindow::MainWindow()
        : KXmlGuiWindow()
        , m_positionSlider( 0 )
        , m_timeLabel( new QLabel( " 0:00:00 ", this ) )
        , m_titleLabel( new KSqueezedTextLabel( this ) )
{
    DEBUG_BLOCK
    s_instance = this;
    setMouseTracking( true );
    new VideoWindow( this );
    videoWindow()->setMouseTracking( true );
    m_positionSlider = videoWindow()->newPositionSlider();

    setCentralWidget( videoWindow() );
    setFocusProxy( videoWindow() ); // essential! See VideoWindow::event(), QEvent::FocusOut

    // these have no affect beccause "KDE Knows Best" FFS
    //setDockEnabled( toolBar(), Qt::DockRight, false ); //doesn't make sense due to our large horizontal slider
    //setDockEnabled( toolBar(), Qt::DockLeft, false ); //as above

    m_titleLabel->setMargin( 2 );
    m_timeLabel->setFont( KGlobalSettings::fixedFont() );
    m_timeLabel->setAlignment( Qt::AlignCenter );
    m_timeLabel->setMinimumSize( m_timeLabel->sizeHint() );

    // work around a bug in KStatusBar
    // sizeHint width of statusbar seems to get stupidly large quickly
    statusBar()->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Maximum );

    statusBar()->addWidget( m_titleLabel, 1 );
    statusBar()->addPermanentWidget( m_timeLabel, 0);
    setupActions();
    setupGUI();
    //setStandardToolBarMenuEnabled( false ); //bah to setupGUI()!
    //toolBar()->show(); //it's possible it would be hidden, but we don't want that as no UI way to show it!

    {
        KActionCollection* ac = actionCollection();
        QMenu *menu = 0;
        QAction *menuAction = 0; 
        QMenu *settings = static_cast<QMenu*>(factory()->container( "settings", this ));
        #define make_menu( name, text ) \
                menu = settings->addMenu( text ); \
                menuAction = menu->menuAction(); \
                menuAction->setObjectName( name ); \
                menuAction->setEnabled( false ); \
                connect( menu, SIGNAL(aboutToShow()), SLOT(aboutToShowMenu()) ); \
                ac->addAction( name, menuAction );

        make_menu( "subtitle_channels_menu", i18n( "&Subtitles" ) );
 //       make_menu( "audio_channels_menu", i18n( "A&udio Channels" ) );
        make_menu( "aspect_ratio_menu", i18n( "Aspect &Ratio" ) );
        #undef make_menu

        {
            m_aspectRatios = new QActionGroup( this );
            m_aspectRatios->setExclusive( true );
            #define make_ratio_action( text, objectname, aspectEnum ) \
            { \
                QAction* ratioAction = new QAction( this ); \
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
            #undef make_ratio_action
            ac->action( "ratio_auto" )->setChecked( true );
            ac->action( "aspect_ratio_menu" )->menu()->addActions( m_aspectRatios->actions() );
        }
        settings->addSeparator();
    }
    {
        QObjectList list = toolBar()->findChildren<QObject*>( "KToolBarButton" );
/*        if (list.isEmpty()) {
                MessageBox::error( i18n(
                    "<qt>" PRETTY_NAME " could not load its interface, this probably means that " PRETTY_NAME " is not "
                    "installed to the correct prefix. If you installed from packages please contact the packager, if "
                    "you installed from source please try running the <b>configure</b> script again like this: "
                    "<pre> % ./configure --prefix=`kde-config --prefix`</pre>" ) );

                std::exit( 1 );
        }
                */
    }
    KXMLGUIClient::stateChanged( "empty" );

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if( args->count() || args->isSet( "play-dvd" ) || kapp->isSessionRestored() )
        //we need to resize the window, so we can't show the window yet
        init();
    else {
        //"faster" startup
        //TODO if we have a size stored for this video, do the "faster" route
        QTimer::singleShot( 0, this, SLOT(init()) );
        QApplication::setOverrideCursor( Qt::WaitCursor ); }
}

void
MainWindow::init()
{
    DEBUG_BLOCK

    connect( engine(), SIGNAL( statusMessage( const QString& ) ), this, SLOT( engineMessage( const QString&   ) ) );
    connect( engine(), SIGNAL( stateChanged( Engine::State ) ), this, SLOT( engineStateChanged( Engine::State ) ) );
    connect( engine(), SIGNAL( titleChanged( const QString& ) ), m_titleLabel, SLOT( setText( const QString&  ) ) );
    connect( engine(), SIGNAL( channelsChanged( QList< QAction* > ) ), this, SLOT( channelsChanged( QList< QAction* > ) ) );

    if( !engine()->init() ) {
        KMessageBox::error( this, i18n(
            "<qt>xine could not be successfully initialised. " PRETTY_NAME " will now exit. "
            "You can try to identify what is wrong with your xine installation using the <b>xine-check</b> command at a command-prompt.") );
        std::exit( 2 );
    }

    //would be dangerous for these to65535 happen before the videoWindow() is initialised
    setAcceptDrops( true );
    connect( statusBar(), SIGNAL(messageChanged( const QString& )), engine(), SLOT(showOSD( const QString& )) );

    QApplication::restoreOverrideCursor();

    if( !kapp->isSessionRestored() ) {
        KCmdLineArgs &args = *KCmdLineArgs::parsedArgs();
        if (args.isSet( "play-dvd" ))
            open( KUrl( "dvd:/" ) );
        else if (args.count() > 0 ) {
            open( args.url( 0 ) );
            args.clear();
            adjustSize(); //will resize us to reflect the videoWindow's sizeHint()
        }
        else
            //show the welcome dialog
            playMedia( true ); // true = show in style of welcome dialog
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

void
MainWindow::setupActions()
{
    DEBUG_BLOCK

    KActionCollection * const ac = actionCollection();

    KStandardAction::quit( kapp, SLOT( closeAllWindows() ), ac );
    //was play_media, never used
    KStandardAction::open( this, SLOT(playMedia()), ac )->setText( i18n("Play &Media...") );
    new FullScreenAction( this, ac );

    new PlayAction( this, SLOT(play()), ac );
    #define addToAc( X ) ac->addAction( X->objectName(), X );
    KAction* playerStop = new KAction( KIcon("media-playback-stop"), i18n("Stop"), ac );
    playerStop->setObjectName( "stop" );
    playerStop->setShortcut( Qt::Key_S );
    connect( playerStop, SIGNAL( triggered() ), engine(), SLOT( stop() ) );
    addToAc( playerStop )

    KAction* recordAction = new KToggleAction( KIcon("media-record"), i18n("Record"), ac );
    recordAction->setObjectName( "record" );
    recordAction->setShortcut( Qt::CTRL + Qt::Key_R );
    connect( recordAction, SIGNAL( triggered() ), engine(), SLOT( record() ) );
    addToAc( recordAction )

    KAction* resetZoom = new KAction( KIcon("zoom-best-fit"), i18n("Reset Video Scale"), ac );
    resetZoom->setObjectName( "reset_zoom" );
    resetZoom->setShortcut( Qt::Key_Equal );
    connect( resetZoom, SIGNAL( triggered() ), videoWindow(), SLOT( resetZoom() ) );
    addToAc( resetZoom )

    KAction* mediaInfo = new KAction( KIcon("messagebox_info"), i18n("Media Information"), ac );
    mediaInfo->setObjectName( "information" );
    mediaInfo->setShortcut( Qt::Key_I );
    connect( mediaInfo, SIGNAL( triggered() ), this, SLOT( streamInformation() ) );
    addToAc( mediaInfo )

    KAction* dvdMenu = new KAction( KIcon("dvd_unmount"), i18n("Menu Toggle"), ac );
    dvdMenu->setObjectName( "toggle_dvd_menu" );
    dvdMenu->setShortcut( Qt::Key_R );
    connect( dvdMenu, SIGNAL( triggered() ), engine(), SLOT( toggleDVDMenu() ) );
    addToAc( dvdMenu )

 //   new KAction( i18n("&Capture Frame"), "frame_image", Key_C, this, SLOT(captureFrame()), ac, "capture_frame" );

    KAction* positionSlider = new KAction( i18n("Position Slider"), ac );
    positionSlider->setObjectName( "position_slider" );
    positionSlider->setDefaultWidget( m_positionSlider );
    addToAc( positionSlider )
   // positionSlider->setAutoSized( true ); PORTING, whats the replacement for this?

    #undef addToAc
}

void
MainWindow::showTime( qint64 ms )
{
    #define zeroPad( n ) n < 10 ? QString("0%1").arg( n ) : QString::number( n )

    const int s  = ms / 1000;
    const int m  =  s / 60;
    const int h  =  m / 60;

    QString time = zeroPad( s % 60 ); //seconds
    time.prepend( ':' );
    time.prepend( zeroPad( m % 60 ) ); //minutes
    time.prepend( ':' );
    time.prepend( QString::number( h ) ); //hours

    m_timeLabel->setText( time );
}

void
MainWindow::engineMessage( const QString &message )
{
    statusBar()->showMessage( message, 3500 );
}

bool
MainWindow::open( const KUrl &url )
{
    DEBUG_BLOCK
    debug() << url << endl;

    if( load( url ) ) {
        const int offset = TheStream::hasProfile()
                // adjust offset if we have session history for this video
                ? TheStream::profile().readEntry<int>( "Position", 0 )
                : 0;

        return engine()->play( offset );
    }

    return false;
}

bool
MainWindow::load( const KUrl &url )
{
     //FileWatch the file that is opened

    if( url.isEmpty() ) {
        MessageBox::sorry( i18n( "Codeine was asked to open an empty URL; it cannot." ) );
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
            MessageBox::sorry( "There was an internal error with the media slave..." );
        else {
            QString path = e.stringValue( KIO::UDSEntry::UDS_LOCAL_PATH );
            if( !path.isEmpty() )
                return engine()->load( KUrl( path ) );
        }
    }

    //let xine handle invalid, etc, KUrlS
    //TODO it handles non-existant files with bad error message
    return engine()->load( url );
}

void
MainWindow::play()
{
    switch( engine()->state() ) {
    case Engine::Playing:
    case Engine::Paused:
        engine()->playPause();
        break;
    case Engine::Loaded:
        break;
    case Engine::Empty:
    default:
        playMedia();
        break;
    }
}

void
MainWindow::playMedia( bool show_welcome_dialog )
{
    PlayDialog dialog( this, show_welcome_dialog );

    switch( dialog.exec() ) {
    case PlayDialog::FILE: {
        const QString filter = engine()->fileFilter() + '|' + i18n("Supported Media Formats") + "\n*|" + i18n("All Files");
        const KUrl url = KFileDialog::getOpenUrl( KUrl(":default"), filter, this, i18n("Select A File To Play") );
        open( url );
        } break;
    case PlayDialog::RECENT_FILE:
        open( dialog.url() );
        break;
    case PlayDialog::CDDA:
        open( KUrl( "cdda:/1" ) );
        break;
    case PlayDialog::VCD:
        open( KUrl( "vcd://" ) ); // one / is not enough
        break;
    case PlayDialog::DVD:
        engine()->playDvd();
        break;
    }
}

void
MainWindow::setFullScreen( bool isFullScreen )
{
    DEBUG_BLOCK
    debug() << "Setting full screen to " << isFullScreen;
    mainWindow()->setWindowState( mainWindow()->windowState() ^ Qt::WindowFullScreen );
    //setWindowState( windowState() & ( full ? Qt::WindowFullScreen : ~Qt::WindowFullScreen ) );
    static FullScreenToolBarHandler *s_handler;

    //toolBar()->setMovingEnabled( !isFullScreen );
    toolBar()->setHidden( isFullScreen && engine()->state() == Engine::Playing );
    menuBar()->setHidden( isFullScreen );
    statusBar()->setHidden( isFullScreen );

    if( isFullScreen )
        s_handler = new FullScreenToolBarHandler( this );
    else
    {
        action( "fullscreen" )->setEnabled( videoWindow()->state() & ( Engine::Playing | Engine::Paused) );
        delete s_handler;
    }
    // prevent videoWindow() moving around when mouse moves 
    //setCentralWidget( isFullScreen ? 0 : videoWindow() ); //deletes videoWindow() when cleared
}

void
MainWindow::streamInformation()
{
    MessageBox::information( TheStream::information(), i18n("Media Information") );
}

void
MainWindow::aboutToShowMenu()
{
//     QMenu *menu = (QMenu*)sender();
//     QByteArray name( sender() ? sender()->objectName() : 0 );
// 
//     // uncheck all items first
//     for( uint x = 0; x < menu->actions()->count(); ++x )
//         menu->actions()->at( x )->setChecked( false );
// 
//     int id;
//     if( name == "subtitle_channels_menu" )
//         id = TheStream::subtitleChannel() + 2;
//     else if( name == "audio_channels_menu" )
//         id = TheStream::audioChannel() + 2;
//     else
//         id = TheStream::aspectRatio();
    DEBUG_BLOCK
    TheStream::aspectRatioAction()->setChecked( true );
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
        open( uriList.first() );
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
MainWindow::channelsChanged( QList< QAction* > subActions )
{
DEBUG_BLOCK
    if( subActions.isEmpty() )
          action("subtitle_channels_menu")->setEnabled( false );
    else
    {
        action("subtitle_channels_menu")->menu()->addActions( subActions );
        action("subtitle_channels_menu")->setEnabled( true );
    }
}

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
        debug() << name << endl;
    Q_ASSERT( mainWindow() );
    Q_ASSERT( actionCollection );
    Q_ASSERT( action );

    return action;
}

} //namespace Codeine

#include "mainWindow.moc"
