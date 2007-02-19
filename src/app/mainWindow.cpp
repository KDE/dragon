// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include "actions.h"
#include "analyzer.h"
#include "config.h"
#include "configure.h"
#include <cstdlib>
#include "debug.h"
#include "extern.h"       //dialog creation function definitions
#include "fullScreenAction.h"
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kcursor.h>
#include <kfiledialog.h>     //::open()
#include <kglobalsettings.h> //::timerEvent()
#include <kio/netaccess.h>
#include <ksqueezedtextlabel.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <kurldrag.h>
#include <kwin.h>
#include "mainWindow.h"
#include "playDialog.h"  //::play()
#include "playlistFile.h"
#include "mxcl.library.h"
#include <q3cstring.h>
#include <qdesktopwidget.h>
#include <qevent.h>      //::stateChanged()
#include <qlayout.h>     //ctor
#include <q3popupmenu.h>  //because XMLGUI is poorly designed
#include <qobject.h>
//Added by qt3to4:
#include <QTimerEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include "slider.h"
#include "theStream.h"
#include "volumeAction.h"
#include "xineEngine.h"

#ifndef NO_XTEST_EXTENSION
extern "C"
{
   #include <X11/extensions/XTest.h>
   #include <X11/keysym.h>
}
#endif


namespace Codeine {


   /// @see codeine.h
   QWidget *mainWindow() { return kapp->mainWidget(); }


MainWindow::MainWindow()
      : KMainWindow()
      , m_positionSlider( new Slider( this, 65535 ) )
      , m_timeLabel( new QLabel( " 0:00:00 ", this ) )
      , m_titleLabel( new KSqueezedTextLabel( this ) )
{
   DEBUG_BLOCK

   clearWFlags( Qt::WDestructiveClose ); //we are allocated on the stack

   kapp->setMainWidget( this );

   new VideoWindow( this );
   setCentralWidget( videoWindow() );
   setFocusProxy( videoWindow() ); // essential! See VideoWindow::event(), QEvent::FocusOut

   // these have no affect beccause "KDE Knows Best" FFS
   setDockEnabled( toolBar(), Qt::DockRight, false ); //doesn't make sense due to our large horizontal slider
   setDockEnabled( toolBar(), Qt::DockLeft, false ); //as above

   m_titleLabel->setMargin( 2 );
   m_timeLabel->setFont( KGlobalSettings::fixedFont() );
   m_timeLabel->setAlignment( AlignCenter );
   m_timeLabel->setMinimumSize( m_timeLabel->sizeHint() );

   // work around a bug in KStatusBar
   // sizeHint width of statusbar seems to get stupidly large quickly
   statusBar()->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Maximum );

   statusBar()->addWidget( m_titleLabel, 1, false );
   statusBar()->addWidget( m_analyzer = new Analyzer::Block( this ), 0, true );
   statusBar()->addWidget( m_timeLabel, 0, true );
   setupActions();
   setupGUI();
   setStandardToolBarMenuEnabled( false ); //bah to setupGUI()!
   toolBar()->show(); //it's possible it would be hidden, but we don't want that as no UI way to show it!

   // only show dvd button when playing a dvd
   {
      struct KdeIsTehSuck : public QObject
      {
         virtual bool eventFilter( QObject*, QEvent *e )
         {
            if (e->type() != QEvent::LayoutHint)
               return false;

            // basically, KDE shows all tool-buttons, even if they are
            // hidden after it does any layout operation. Yay for KDE. Yay.
            QWidget *button = (QWidget*)((KMainWindow*)mainWindow())->toolBar()->child( "toolbutton_toggle_dvd_menu" );
            if (button)
               button->setShown( TheStream::url().protocol() == "dvd" );
            return false;
         }
      } *o;
      o = new KdeIsTehSuck;
      toolBar()->installEventFilter( o );
      insertChild( o );
   }

   {
      Q3PopupMenu *menu = 0, *settings = static_cast<Q3PopupMenu*>(factory()->container( "settings", this ));
      int id = SubtitleChannelsMenuItemId, index = 0;

      #define make_menu( name, text ) \
            menu = new Q3PopupMenu( this, name ); \
            menu->setCheckable( true ); \
            connect( menu, SIGNAL(activated( int )), engine(), SLOT(setStreamParameter( int )) ); \
            connect( menu, SIGNAL(aboutToShow()), SLOT(aboutToShowMenu()) ); \
            settings->insertItem( text, menu, id, index ); \
            settings->setItemEnabled( id, false ); \
            id++, index++;

      make_menu( "subtitle_channels_menu", i18n( "&Subtitles" ) );
      make_menu( "audio_channels_menu", i18n( "A&udio Channels" ) );
      make_menu( "aspect_ratio_menu", i18n( "Aspect &Ratio" ) );
      #undef make_menu

      Codeine::insertAspectRatioMenuItems( menu ); //so we don't have to include xine.h here

      settings->insertSeparator( index );
   }

   QObjectList *list = toolBar()->queryList( "KToolBarButton" );
   if (list->isEmpty()) {
      MessageBox::error( i18n(
            "<qt>" PRETTY_NAME " could not load its interface, this probably means that " PRETTY_NAME " is not "
            "installed to the correct prefix. If you installed from packages please contact the packager, if "
            "you installed from source please try running the <b>configure</b> script again like this: "
            "<pre> % ./configure --prefix=`kde-config --prefix`</pre>" ) );

      std::exit( 1 );
   }
   delete list;

   KXMLGUIClient::stateChanged( "empty" );

   KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
   if( args->count() || args->isSet( "play-dvd" ) || kapp->isSessionRestored() )
      //we need to resize the window, so we can't show the window yet
      init();
   else {
      //"faster" startup
      //TODO if we have a size stored for this video, do the "faster" route
      QTimer::singleShot( 0, this, SLOT(init()) );
      QApplication::setOverrideCursor( KCursor::waitCursor() ); }
}

void
MainWindow::init()
{
   DEBUG_BLOCK

   connect( engine(), SIGNAL(statusMessage( const QString& )), this, SLOT(engineMessage( const QString& )) );
   connect( engine(), SIGNAL(stateChanged( Engine::State )), this, SLOT(engineStateChanged( Engine::State )) );
   connect( engine(), SIGNAL(channelsChanged( const QStringList& )), this, SLOT(setChannels( const QStringList& )) );
   connect( engine(), SIGNAL(titleChanged( const QString& )), m_titleLabel, SLOT(setText( const QString& )) );
   connect( m_positionSlider, SIGNAL(valueChanged( int )), this, SLOT(showTime( int )) );

   if( !engine()->init() ) {
      KMessageBox::error( this, i18n(
         "<qt>xine could not be successfully initialised. " PRETTY_NAME " will now exit. "
         "You can try to identify what is wrong with your xine installation using the <b>xine-check</b> command at a command-prompt.") );
      std::exit( 2 );
   }

   //would be dangerous for these to65535 happen before the videoWindow() is initialised
   setAcceptDrops( true );
   connect( m_positionSlider, SIGNAL(sliderReleased( uint )), engine(), SLOT(seek( uint )) );
   connect( statusBar(), SIGNAL(messageChanged( const QString& )), engine(), SLOT(showOSD( const QString& )) );

   QApplication::restoreOverrideCursor();

   if( !kapp->isSessionRestored() ) {
      KCmdLineArgs &args = *KCmdLineArgs::parsedArgs();
      if (args.isSet( "play-dvd" ))
         open( "dvd:/" );
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

   delete videoWindow(); //fades out sound in dtor
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
   KStandardAction::open( this, SLOT(playMedia()), ac, "play_media" )->setText( i18n("Play &Media...") );
   connect( new FullScreenAction( this, ac ), SIGNAL(toggled( bool )), SLOT(fullScreenToggled( bool )) );

   new PlayAction( this, SLOT(play()), ac );
   new KAction( i18n("Stop"), "player_stop", Key_S, engine(), SLOT(stop()), ac, "stop" );

   new KToggleAction( i18n("Record"), "player_record", CTRL + Key_R, engine(), SLOT(record()), ac, "record" );

   new KAction( i18n("Reset Video Scale"), "viewmag1", Key_Equal, videoWindow(), SLOT(resetZoom()), ac, "reset_zoom" );
   new KAction( i18n("Media Information"), "messagebox_info", Key_I, this, SLOT(streamInformation()), ac, "information" );
   new KAction( i18n("Menu Toggle"), "dvd_unmount", Key_R, engine(), SLOT(toggleDVDMenu()), ac, "toggle_dvd_menu" );
   new KAction( i18n("&Capture Frame"), "frame_image", Key_C, this, SLOT(captureFrame()), ac, "capture_frame" );

   new KAction( i18n("Video Settings..."), "configure", Key_V, this, SLOT(configure()), ac, "video_settings" );
   new KAction( i18n("Configure xine..."), "configure", 0, this, SLOT(configure()), ac, "xine_settings" );

   (new K3WidgetAction( m_positionSlider, i18n("Position Slider"), 0, 0, 0, ac, "position_slider" ))->setAutoSized( true );

   new VolumeAction( toolBar(), ac );
}

void
MainWindow::saveProperties( KConfig *config )
{
   config->writeEntry( "url", TheStream::url().url() );
   config->writeEntry( "time", engine()->time() );
}

void
MainWindow::readProperties( KConfig *config )
{
   if( engine()->load( config->readPathEntry( "url" ) ) )
      engine()->play( config->readNumEntry( "time" ) );
}

void
MainWindow::timerEvent( QTimerEvent* )
{
   static int counter = 0;

   if( engine()->state() == Engine::Playing ) {
      ++counter &= 1023;

      m_positionSlider->setValue( engine()->position() );
      if( !m_positionSlider->isEnabled() && counter % 10 == 0 ) // 0.5 seconds
         // usually the slider emits a signal that updates the timeLabel
         // but not if the slider isn't moving because there is no length
         showTime();

      #ifndef NO_XTEST_EXTENSION
      if( counter == 0 /*1020*/ ) { // 51 seconds //do at 0 to ensure screensaver doesn't happen before 51 seconds is up (somehow)
         const bool isOnThisDesktop = KWin::windowInfo( winId() ).isOnDesktop( KWin::currentDesktop() );

         if( videoWindow()->isVisible() && isOnThisDesktop ) {
            int key = XKeysymToKeycode( x11Display(), XK_Shift_R );

            XTestFakeKeyEvent( x11Display(), key, true, CurrentTime );
            XTestFakeKeyEvent( x11Display(), key, false, CurrentTime );
            XSync( x11Display(), false );
         }
      }
      #endif
   }
}

void
MainWindow::showTime( int pos )
{
   #define zeroPad( n ) n < 10 ? QString("0%1").arg( n ) : QString::number( n )

   const int ms = (pos == -1) ? engine()->time() : int(engine()->length() * (pos / 65535.0));
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
   statusBar()->message( message, 3500 );
}

bool
MainWindow::open( const KUrl &url )
{
   DEBUG_BLOCK
   debug() << url << endl;

   if( load( url ) ) {
      const int offset = TheStream::hasProfile()
            // adjust offset if we have session history for this video
            ? TheStream::profile()->readNumEntry( "Position", 0 )
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
      #define UDS_LOCAL_PATH (72 | KIO::UDS_STRING)
      KIO::UDSEntry e;
      if (!KIO::NetAccess::stat( url, e, 0 ))
         MessageBox::sorry( "There was an internal error with the media slave..." );
      else {
         KIO::UDSEntry::ConstIterator end = e.end();
         for (KIO::UDSEntry::ConstIterator it = e.begin(); it != end; ++it)
            if ((*it).m_uds == UDS_LOCAL_PATH && !(*it).m_str.isEmpty())
               return engine()->load( KUrl::fromPathOrUrl( (*it).m_str ) );
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
      engine()->pause();
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
      const KUrl url = KFileDialog::getOpenURL( ":default", filter, this, i18n("Select A File To Play") );
      open( url );
      } break;
   case PlayDialog::RECENT_FILE:
      open( dialog.url() );
      break;
   case PlayDialog::CDDA:
      open( "cdda:/1" );
      break;
   case PlayDialog::VCD:
      open( "vcd://" ); // one / is not enough
      break;
   case PlayDialog::DVD:
      open( "dvd:/" );
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

      else if (!m_toolbar->hasMouse())
         m_toolbar->hide();

      m_stay_hidden_for_a_bit = false;
   }
};


void
MainWindow::fullScreenToggled( bool isFullScreen )
{
   static FullScreenToolBarHandler *s_handler;

   DEBUG_FUNC_INFO

   if( isFullScreen )
      toolBar()->setPalette( palette() ), // due to 2px spacing in QMainWindow :(
      setPaletteBackgroundColor( Qt::black ); // due to 2px spacing
   else
      toolBar()->unsetPalette(),
      unsetPalette();

   toolBar()->setMovingEnabled( !isFullScreen );
   toolBar()->setHidden( isFullScreen && engine()->state() == Engine::Playing );

   reinterpret_cast<QWidget*>(menuBar())->setHidden( isFullScreen );
   statusBar()->setHidden( isFullScreen );

   setMouseTracking( isFullScreen ); /// @see mouseMoveEvent()

   if (isFullScreen)
      s_handler = new FullScreenToolBarHandler( this );
   else
      delete s_handler;

   // prevent videoWindow() moving around when mouse moves
   setCentralWidget( isFullScreen ? 0 : videoWindow() );
}

void
MainWindow::configure()
{
   const Q3CString sender = this->sender()->name();

   if( sender == "video_settings" )
      Codeine::showVideoSettingsDialog( this );

   else if( sender == "xine_settings" )
      Codeine::showXineConfigurationDialog( this, *engine() );
}

void
MainWindow::streamInformation()
{
   MessageBox::information( TheStream::information(), i18n("Media Information") );
}

void
MainWindow::setChannels( const QStringList &channels )
{
   DEBUG_FUNC_INFO

   //TODO -1 = auto

   QStringList::ConstIterator it = channels.begin();

   Q3PopupMenu *menu = (Q3PopupMenu*)child( (*it).latin1() );
   menu->clear();

   menu->insertItem( i18n("&Determine Automatically"), 1 );
   menu->insertSeparator();

   //the id is crucial, since the slot this menu is connected to requires
   //that information to set the correct channel
   //NOTE we subtract 2 in xineEngine because QMenuData doesn't allow negative id
   int id = 2;
   ++it;
   for( QStringList::ConstIterator const end = channels.end(); it != end; ++it, ++id )
      menu->insertItem( *it, id );

   menu->insertSeparator();
   menu->insertItem( i18n("&Off"), 0 );

   id = channels.first() == "subtitle_channels_menu" ? SubtitleChannelsMenuItemId : AudioChannelsMenuItemId;
   MainWindow::menu( "settings" )->setItemEnabled( id, channels.count() > 1 );
}

void
MainWindow::aboutToShowMenu()
{
   Q3PopupMenu *menu = (Q3PopupMenu*)sender();
   Q3CString name( sender() ? sender()->name() : 0 );

   // uncheck all items first
   for( uint x = 0; x < menu->count(); ++x )
      menu->setItemChecked( menu->idAt( x ), false );

   int id;
   if( name == "subtitle_channels_menu" )
      id = TheStream::subtitleChannel() + 2;
   else if( name == "audio_channels_menu" )
      id = TheStream::audioChannel() + 2;
   else
      id = TheStream::aspectRatio();

   menu->setItemChecked( id, true );
}

void
MainWindow::dragEnterEvent( QDragEnterEvent *e )
{
   e->accept( KUrlDrag::canDecode( e ) );
}

void
MainWindow::dropEvent( QDropEvent *e )
{
   KUrl::List list;
   KUrlDrag::decode( e, list );

   if( !list.isEmpty() )
      open( list.first() );
   else
      engineMessage( i18n("Sorry, no media was found in the drop") );
}

void
MainWindow::keyPressEvent( QKeyEvent *e )
{
   #define seek( step ) { \
         const int new_pos = m_positionSlider->value() step; \
         engine()->seek( new_pos > 0 ? (uint)new_pos : 0 ); \
      }

   switch( e->key() )
   {
      case Qt::Key_Left:  seek( -500 ); break;
      case Qt::Key_Right: seek( +500 ); break;
      case Key_Escape:    KWin::clearState( winId(), NET::FullScreen );
      default: ;
   }

   #undef seek
}

Q3PopupMenu*
MainWindow::menu( const char *name )
{
   // KXMLGUI is "really good".
   return static_cast<Q3PopupMenu*>(factory()->container( name, this ));
}


/// Convenience class for other classes that need access to the actionCollection
KActionCollection*
actionCollection()
{
   return static_cast<MainWindow*>(kapp->mainWidget())->actionCollection();
}

/// Convenience class for other classes that need access to the actions
KAction*
action( const char *name )
{
   #define QT_FATAL_ASSERT

   MainWindow *mainWindow = 0;
   KActionCollection *actionCollection = 0;
   KAction *action = 0;

   if( mainWindow = (MainWindow*)kapp->mainWidget() )
      if( actionCollection = mainWindow->actionCollection() )
         action = actionCollection->action( name );

   Q_ASSERT( mainWindow );
   Q_ASSERT( actionCollection );
   Q_ASSERT( action );

   return action;
}

} //namespace Codeine
