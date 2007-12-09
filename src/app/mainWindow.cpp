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
#include <QTimerEvent>

#include "actions.h"
#include "debug.h"
#include "extern.h"         //dialog creation function definitions
#include "fullScreenAction.h"
#include "mainWindow.h"
#include "mxcl.library.h"
#include "playDialog.h"  //::play()
#include "playlistFile.h"
#include "theStream.h"
#include "videoWindow.h"

#ifndef NO_XTEST_EXTENSION
extern "C"
{
    #include <X11/extensions/XTest.h>
    #include <X11/keysym.h>
}
#endif

using namespace Qt;

namespace Codeine {

    MainWindow *MainWindow::s_instance = 0;
    /// @see codeine.h
    QWidget* mainWindow() { return MainWindow::s_instance; }



MainWindow::MainWindow()
        : KXmlGuiWindow ()
        , m_positionSlider( 0 )
        , m_timeLabel( new QLabel( " 0:00:00 ", this ) )
        , m_titleLabel( new KSqueezedTextLabel( this ) )
{
    DEBUG_BLOCK
    s_instance = this;
//PORTING?    setWindowState( windowState() ^ Qt::WDestructiveClose ); //we are allocated on the stack

    new VideoWindow( this );
    m_positionSlider = videoWindow()->newPositionSlider();

    setCentralWidget( videoWindow() );
    setFocusProxy( videoWindow() ); // essential! See VideoWindow::event(), QEvent::FocusOut

    // these have no affect beccause "KDE Knows Best" FFS
    //setDockEnabled( toolBar(), Qt::DockRight, false ); //doesn't make sense due to our large horizontal slider
    //setDockEnabled( toolBar(), Qt::DockLeft, false ); //as above

    m_titleLabel->setMargin( 2 );
    m_timeLabel->setFont( KGlobalSettings::fixedFont() );
    m_timeLabel->setAlignment( AlignCenter );
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
    //connect( m_positionSlider, SIGNAL(valueChanged( int )), this, SLOT(showTime( int )) );

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

    //don't do until videoWindow() is initialised!
    startTimer( 50 );
}

MainWindow::~MainWindow()
{
    DEBUG_FUNC_INFO

    hide(); //so we appear to have quit, and then sound fades out below

 //   delete videoWindow(); //fades out sound in dtor
}

bool
MainWindow::queryExit()
{
    if( toggleAction( "fullscreen" )->isChecked() ) {
        // there seems to be no other way to stop KMainWindow
        // saving the window state without any controls
        fullScreenToggled( false );
        showNormal();
        QApplication::sendPostedEvents( this, 0 );
        // otherwise KMainWindow saves the screensize as maximised
        Codeine::MessageBox::sorry(
                "This annoying messagebox is to get round a bug in either KDE or Qt. "
                "Just press OK and Codeine will quit." );
        //NOTE not actually needed
        saveAutoSaveSettings();
        hide();
    }

    return true;
}

void
MainWindow::setupActions()
{
    DEBUG_BLOCK

    KActionCollection * const ac = actionCollection();

    KStandardAction::quit( kapp, SLOT(quit()), ac );
    //was play_media, never used
    KStandardAction::open( this, SLOT(playMedia()), ac )->setText( i18n("Play &Media...") );
    //connect( new FullScreenAction( this, ac ), SIGNAL(toggled( bool )), SLOT(fullScreenToggled( bool )) );
    new FullScreenAction( this, ac );

    new PlayAction( this, SLOT(play()), ac );
    #define addToAc( X ) ac->addAction( X->objectName(), X );
    KAction* playerStop = new KAction( KIcon("player_stop"), i18n("Stop"), ac );
    playerStop->setObjectName( "stop" );
    playerStop->setShortcut( Qt::Key_S );
    connect( playerStop, SIGNAL( triggered() ), engine(), SLOT( stop() ) );
    addToAc( playerStop )

    KAction* recordAction = new KToggleAction( KIcon("player_record"), i18n("Record"), ac );
    recordAction->setObjectName( "record" );
    recordAction->setShortcut( Qt::CTRL + Qt::Key_R );
    connect( recordAction, SIGNAL( triggered() ), engine(), SLOT( record() ) );
    addToAc( recordAction )

    KAction* resetZoom = new KAction( KIcon("viewmag1"), i18n("Reset Video Scale"), ac );
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

/*
void
MainWindow::saveProperties( KConfig *config )
{
    config->writeEntry( "url", TheStream::url().url() );
    config->writeEntry( "time", engine()->currentTime() );
}

void
MainWindow::readProperties( KConfig *config )
{
    if( engine()->load( config->readPathEntry( "url" ) ) )
        engine()->play( config->readNumEntry( "time" ) );
}
*/

void
MainWindow::timerEvent( QTimerEvent* )
{
    static int counter = 0;

    if( engine()->state() == Engine::Playing ) {
        ++counter &= 1023;

        if( !m_positionSlider->isEnabled() && counter % 10 == 0 ) // 0.5 seconds
            // usually the slider emits a signal that updates the timeLabel
            // but not if the slider isn't moving because there is no length
            showTime();

    }
}

void
MainWindow::showTime( int pos )
{
    #define zeroPad( n ) n < 10 ? QString("0%1").arg( n ) : QString::number( n )

    const int ms = (pos == -1) ? engine()->currentTime() : int(engine()->length() * (pos / 65535.0));
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
    case Engine::Loaded:
        engine()->play();
        break;

    case Engine::Playing:
    case Engine::Paused:
        engine()->playPause();
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

class FullScreenToolBarHandler : QObject
{
    KToolBar *m_toolbar;
    int m_timer_id;
    bool m_stay_hidden_for_a_bit;
    QPoint m_home;

public:
    FullScreenToolBarHandler( KMainWindow *parent )
            : QObject( parent )
            , m_toolbar( parent->toolBar() )
            , m_timer_id( 0 )
            , m_stay_hidden_for_a_bit( false )
    {
        DEBUG_BLOCK

        parent->installEventFilter( this );
        m_toolbar->installEventFilter( this );
    }

    bool eventFilter( QObject *o, QEvent *e )
    {
        if (o == parent() && e->type() == QEvent::MouseMove) {
            killTimer( m_timer_id );

            QMouseEvent const * const me = (QMouseEvent*)e;
            if (m_stay_hidden_for_a_bit) {
                // wait for a small pause before showing the toolbar again
                // usage = user removes mouse from toolbar after using it
                // toolbar disappears (usage is over) but usually we show
                // toolbar immediately when mouse is moved.. so we need this hack

                // HACK if user thrusts mouse to top, we assume they really want the toolbar
                // back. Is hack as 80% of users have at top, but 20% at bottom, we don't cater
                // for the 20% as lots more code, for now.
                if (me->pos().y() < m_toolbar->height())
                    goto show_toolbar;

                m_timer_id = startTimer( 100 );
            }
            else {
                if (m_toolbar->isHidden()) {
                    if (m_home.isNull())
                        m_home = me->pos();
                    else if ((m_home - me->pos()).manhattanLength() > 6)
                        // then cursor has moved far enough to trigger show toolbar
show_toolbar:
                        m_toolbar->show(),
                        m_home = QPoint();
                    else
                        // cursor hasn't moved far enough yet
                        // don't reset timer below, return instead
                        return false;
                }

                // reset the hide timer
                m_timer_id = startTimer( VideoWindow::CURSOR_HIDE_TIMEOUT );
            }
        }

        if (o == parent() && e->type() == QEvent::Resize)
        {
            //we aren't managed by mainWindow when at FullScreen
            videoWindow()->move( 0, 0 );
            videoWindow()->resize( ((QWidget*)o)->size() );
            videoWindow()->lower();
        }

        if (o == m_toolbar)
            switch (e->type()) {
                case QEvent::Enter:
                    m_stay_hidden_for_a_bit = false;
                    killTimer( m_timer_id );
                break;

                case QEvent::Leave:
                    m_toolbar->hide();
                    m_stay_hidden_for_a_bit = true;
                    killTimer( m_timer_id );
                    m_timer_id = startTimer( 100 );
                break;

                default: break;
            }

        return false;
    }

    void timerEvent( QTimerEvent* )
    {
        if (m_stay_hidden_for_a_bit)
            ;

        else if ( !m_toolbar->testAttribute( Qt::WA_UnderMouse ) )
            m_toolbar->hide();

        m_stay_hidden_for_a_bit = false;
    }
};


void
MainWindow::fullScreenToggled( bool isFullScreen )
{
    DEBUG_BLOCK
    static FullScreenToolBarHandler *s_handler;

    //toolBar()->setMovingEnabled( !isFullScreen );
    toolBar()->setHidden( isFullScreen && engine()->state() == Engine::Playing );

    reinterpret_cast<QWidget*>(menuBar())->setHidden( isFullScreen );
    statusBar()->setHidden( isFullScreen );

    setMouseTracking( isFullScreen ); /// @see mouseMoveEvent()

    if (isFullScreen)
        s_handler = new FullScreenToolBarHandler( this );
    else
        delete s_handler;

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

    TheStream::aspectRatioAction()->setChecked( true );
    int subId = TheStream::subtitleChannel();
    QList< QAction* > subs = action("subtitle_channels_menu")->menu()->actions();
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
    #define seek( step ) { \
            const int new_pos = engine()->currentTime() + step; \
            engine()->seek( new_pos > 0 ? (uint)new_pos : 0 ); \
        }

    switch( e->key() )
    {
        case Qt::Key_Left:  seek( -500 ); break;
        case Qt::Key_Right: seek( +500 ); break;
        case Key_Escape:     KWindowSystem::clearState( winId(), NET::FullScreen );
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
    action("subtitle_channels_menu")->menu()->addActions( subActions );
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
