// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#define CODEINE_DEBUG_PREFIX "engine"

#include "actions.h"      //::seek() FIXME unfortunate
#include <cmath>          //the fade out
#include "config.h"
#include "debug.h"
#include <limits>
#include <klocale.h>
#include "mxcl.library.h"
#include <qapplication.h> //::sendEvent()
#include <qdatetime.h>    //record()
#include <qdir.h>         //::exists()
#include "slider.h"
#include "theStream.h"
#include <xine.h>
#include "xineEngine.h"
#include "xineScope.h"


#define XINE_SAFE_MODE 1

extern "C" { void _debug( const char *string ) { debug() << string; } } //FIXME


namespace Codeine {


VideoWindow *VideoWindow::s_instance = 0;


VideoWindow::VideoWindow( QWidget *parent )
      : QWidget( parent, "VideoWindow" )
      , m_osd( 0 )
      , m_stream( 0 )
      , m_eventQueue( 0 )
      , m_videoPort( 0 )
      , m_audioPort( 0 )
      , m_scope( 0 )
      , m_xine( 0 )
      , m_current_vpts( 0 )
{
   DEBUG_BLOCK

   s_instance = this;

   setWFlags( Qt::WNoAutoErase );
   setMouseTracking( true );
   setAcceptDrops( true );
   setUpdatesEnabled( false ); //to stop Qt drawing over us
   setPaletteBackgroundColor( Qt::black );
   setFocusPolicy( ClickFocus );

   //TODO sucks
   //TODO namespace this?
   myList->next = myList; //init the buffer list
}

VideoWindow::~VideoWindow()
{
   DEBUG_BLOCK

   eject();

   // fade out volume on exit
   if( m_stream && xine_get_status( m_stream ) == XINE_STATUS_PLAY ) {
      int cum = 0;
      for( int v = 99; v >= 0; v-- ) {
         xine_set_param( m_stream, XINE_PARAM_AUDIO_AMP_LEVEL, v );
         int sleep = int(32000 * (-std::log10( double(v + 1) ) + 2));

         ::usleep( sleep );

         cum += sleep;
      }

      debug() << "Total sleep: " << cum << "x10^-6 s\n";

      xine_stop( m_stream );

      ::sleep( 1 );
   }

   //xine_set_param( m_stream, XINE_PARAM_IGNORE_VIDEO, 1 );

   if( m_osd )        xine_osd_free( m_osd );
   if( m_stream )     xine_close( m_stream );
   if( m_eventQueue ) xine_event_dispose_queue( m_eventQueue );
   if( m_stream )     xine_dispose( m_stream );
   if( m_audioPort )  xine_close_audio_driver( m_xine, m_audioPort );
   if( m_videoPort )  xine_close_video_driver( m_xine, m_videoPort );
   if( m_scope )      xine_post_dispose( m_xine, m_scope );
   if( m_xine )       xine_exit( m_xine );

   cleanUpVideo();
}

bool
VideoWindow::init()
{
   DEBUG_BLOCK

   initVideo();

   debug() << "xine_new()\n";
   m_xine = xine_new();
   if( !m_xine )
      return false;

   #ifdef XINE_SAFE_MODE
   xine_engine_set_param( m_xine, XINE_ENGINE_PARAM_VERBOSITY, 99 );
   #endif

   debug() << "xine_config_load()\n";
   xine_config_load( m_xine, QFile::encodeName( QDir::homeDirPath() + "/.xine/config" ) );

   debug() << "xine_init()\n";
   xine_init( m_xine );

   debug() << "xine_open_video_driver()\n";
   m_videoPort = xine_open_video_driver( m_xine, "auto", XINE_VISUAL_TYPE_X11, videoWindow()->x11Visual() );

   debug() << "xine_open_audio_driver()\n";
   m_audioPort = xine_open_audio_driver( m_xine, "auto", NULL );

   debug() << "xine_stream_new()\n";
   m_stream = xine_stream_new( m_xine, m_audioPort, m_videoPort );
   if( !m_stream )
      return false;

   // we do these after creating the stream as they are non-fatal
   // and the messagebox creates a modal event loop that allows
   // events that require a stream to have been created..
   if( !m_videoPort )
      MessageBox::error( i18n("xine was unable to initialize any video-drivers.") );
   if( !m_audioPort )
      MessageBox::error( i18n("xine was unable to initialize any audio-drivers.") );

   debug() << "xine_osd_new()\n";
   m_osd = xine_osd_new( m_stream, 10, 10, 1000, 18 * 6 + 10 );
   if( m_osd ) {
      xine_osd_set_font( m_osd, "sans", 18 );
      xine_osd_set_text_palette( m_osd, XINE_TEXTPALETTE_WHITE_BLACK_TRANSPARENT, XINE_OSD_TEXT1 );
   }

   #ifndef XINE_SAFE_MODE
   debug() << "scope_plugin_new()\n";
   m_scope = scope_plugin_new( m_xine, m_audioPort );

   //FIXME this one seems to make seeking unstable for Codeine, perhaps
   xine_set_param( m_stream, XINE_PARAM_METRONOM_PREBUFFER, 6000 ); //less buffering, faster seeking..

   // causes an abort currently
   //xine_trick_mode( m_stream, XINE_TRICK_MODE_SEEK_TO_TIME, 1 );
   #endif


   {
      typedef QValueList<int> List;
      List params( List()
            << XINE_PARAM_VO_HUE << XINE_PARAM_VO_SATURATION << XINE_PARAM_VO_CONTRAST << XINE_PARAM_VO_BRIGHTNESS
            << XINE_PARAM_SPU_CHANNEL << XINE_PARAM_AUDIO_CHANNEL_LOGICAL << XINE_PARAM_VO_ASPECT_RATIO );

      for( List::ConstIterator it = params.constBegin(), end = params.constEnd(); it != end; ++it )
         debug1( xine_get_param( m_stream, *it ) );
   }


   debug() << "xine_event_create_listener_thread()\n";
   xine_event_create_listener_thread( m_eventQueue = xine_event_new_queue( m_stream ), &VideoWindow::xineEventListener, (void*)this );

   //set the UI up to a default state
   announceStateChange();

   startTimer( 200 ); //prunes the scope

   return true;
}

void
VideoWindow::eject()
{
   //WARNING! don't xine_stop or that, buggers up dtor

   if( m_url.isEmpty() )
      return;

   KConfig *profile = TheStream::profile(); // the config profile for this video file

   #define writeParameter( param, default ) { \
         const int value = xine_get_param( m_stream, param ); \
         const QString key = QString::number( param ); \
         if( value != default ) \
            profile->writeEntry( key, value ); \
         else \
            profile->deleteEntry( key ); }

   writeParameter( XINE_PARAM_VO_HUE, 32768 );
   writeParameter( XINE_PARAM_VO_SATURATION, 32772 );
   writeParameter( XINE_PARAM_VO_CONTRAST, 32772 );
   writeParameter( XINE_PARAM_VO_BRIGHTNESS, 32800 )
   writeParameter( XINE_PARAM_SPU_CHANNEL, -1 );
   writeParameter( XINE_PARAM_AUDIO_CHANNEL_LOGICAL, -1 );
   writeParameter( XINE_PARAM_VO_ASPECT_RATIO, 0 );

   #undef writeParameter


   if( xine_get_status( m_stream ) == XINE_STATUS_PLAY && //XINE_STATUS_PLAY = playing OR paused
            length() - time() > 5000 ) // if we are really close to the end, don't remember the position
      profile->writeEntry( "Position", position() );
   else
      profile->deleteEntry( "Position" );

   const QSize s = videoWindow()->size();
   const QSize defaultSize = TheStream::defaultVideoSize();
   if( s.width() == defaultSize.width() || s.height() == defaultSize.height() )
      profile->deleteEntry( "Preferred Size" );
   else
      profile->writeEntry( "Preferred Size", s );

   profile->sync();

   m_url = KURL();
}

bool
VideoWindow::load( const KURL &url )
{
   mxcl::WaitCursor allocateOnStack;

   eject(); //save profile for this video

   m_url = url;

   // only gets shown if there is an error generally, as no event processing
   // occurs, so no paint event. This is fine IMO, TODO although if xine_open hangs
   // due to something, it would be good to show the message...
   emit statusMessage( i18n("Loading media: %1" ).arg( url.fileName() ) );

   debug() << "xine_open()\n";
   if( xine_open( m_stream, url.url().local8Bit() ) )
   {
      KConfig *profile = TheStream::profile();
      #define setParameter( param, default ) xine_set_param( m_stream, param, profile->readNumEntry( QString::number( param ), default ) );
      setParameter( XINE_PARAM_VO_HUE, 32768 );
      setParameter( XINE_PARAM_VO_SATURATION, 32772 );
      setParameter( XINE_PARAM_VO_CONTRAST, 32772 );
      setParameter( XINE_PARAM_VO_BRIGHTNESS, 32800 )
      setParameter( XINE_PARAM_SPU_CHANNEL, -1 );
      setParameter( XINE_PARAM_AUDIO_CHANNEL_LOGICAL, -1 );
      setParameter( XINE_PARAM_VO_ASPECT_RATIO, 0 );
      setParameter( XINE_PARAM_AUDIO_AMP_LEVEL, 100 );
      #undef setParameter

      videoWindow()->setShown( xine_get_stream_info( m_stream, XINE_STREAM_INFO_HAS_VIDEO ) );

      //TODO popup message for no audio
      //TODO popup message for no video + no audio

      #ifndef XINE_SAFE_MODE
      // ensure old buffers are deleted
      // FIXME leaves one erroneous buffer
      timerEvent( 0 );

      if( m_scope ) {
         xine_post_out_t *source = xine_get_audio_source( m_stream );
         xine_post_in_t  *target = (xine_post_in_t*)xine_post_input( m_scope, const_cast<char*>("audio in") );
         xine_post_wire( source, target );
      }
      #endif

      announceStateChange();

      return true;
   }

   showErrorMessage();
   announceStateChange();
   m_url = KURL();
   return false;
}

bool
VideoWindow::play( uint offset )
{
   mxcl::WaitCursor allocateOnStack;

   const bool resume = offset > 0 && /*FIXME*/ m_url.protocol() != "dvd";
   if( resume )
      //HACK because we have to do xine_play() the audio "stutters"
      //     so we mute it and then unmute it to make it sound better
      xine_set_param( m_stream, XINE_PARAM_AUDIO_AMP_MUTE, 1 );

   debug() << "xine_play()\n";
   if( xine_play( m_stream, offset, 0 ) )
   {
      if( resume ) {
         //we have to set this or it stays at 0
         Slider::instance()->setValue( offset );

         // we come up paused if we are resuming playback from a previous session
         pause();

         // see above from HACK
         xine_set_param( m_stream, XINE_PARAM_AUDIO_AMP_MUTE, 0 );
      }
      else
         announceStateChange();

      return true;
   }

   showErrorMessage();
   return false;
}

void
VideoWindow::record()
{
   xine_cfg_entry_t config;

   if( xine_config_lookup_entry( m_xine, "misc.save_dir", &config ) )
   {
      //TODO which fricking KDE function tells me this? Who can tell, stupid KDE API
      QDir d( QDir::home().filePath( "Desktop" ) );
      config.str_value = qstrdup( d.exists() //FIXME tiny-mem-leak, *shrug*
            ? d.path().utf8()
            : QDir::homeDirPath().utf8() );
      xine_config_update_entry( m_xine, &config );

      const QString fileName = m_url.filename();

      QString
      url  = m_url.url();
      url += "#save:";
      url += m_url.host();
      url += " [";
      url += QDate::currentDate().toString();
      url += ']';
      url += fileName.mid( fileName.findRev( '.' ) + 1 ).lower();

      xine_open( m_stream, url.local8Bit() );
      xine_play( m_stream, 0, 0 );

      emit statusMessage( i18n( "Recording to: %1" ).arg( url ) );

      debug() << url << endl;
   }
   else
      debug() << "unable to set misc.save_dir\n";
}

void
VideoWindow::stop()
{
   xine_stop( m_stream );

   announceStateChange();
}

void
VideoWindow::pause()
{
   if( xine_get_status( m_stream ) == XINE_STATUS_STOP )
      play();

   else if( m_url.protocol() == "http" )
      // we are playing and it's an HTTP stream
      stop();

   else if( xine_get_param( m_stream, XINE_PARAM_SPEED ) ) {
      // do first because xine is slow to pause and is bad feedback otherwise
      emit stateChanged( Engine::Paused );
      xine_set_param( m_stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE );
      xine_set_param( m_stream, XINE_PARAM_AUDIO_CLOSE_DEVICE, 1);
      showOSD( i18n( "Playback paused" ) );
   }
   else {
      xine_set_param( m_stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL );
      announceStateChange();
      showOSD( i18n( "Playback resumed" ) );
   }
}

void
VideoWindow::showErrorMessage()
{
   const QString name = m_url.fileName();

   debug() << "xine_get_error()\n";
   switch( xine_get_error( m_stream ) )
   {
   case XINE_ERROR_NO_INPUT_PLUGIN:
      MessageBox::sorry( i18n("There is no input plugin that can read: %1.").arg( name ) );
      break;
   case XINE_ERROR_NO_DEMUX_PLUGIN:
      MessageBox::sorry( i18n("There is no demux plugin available for %1.").arg( name ) );
      break;
   case XINE_ERROR_DEMUX_FAILED:
      MessageBox::sorry( i18n("Demuxing failed for %1.").arg( name ) );
      break;
   case XINE_ERROR_INPUT_FAILED:
   case XINE_ERROR_MALFORMED_MRL:
   case XINE_ERROR_NONE:
      MessageBox::sorry( i18n("Internal error while attempting to play %1.").arg( name ) );
      break;
   }
}

Engine::State
VideoWindow::state() const
{
   //FIXME this is for the analyzer, but I don't like the analyzer being dodgy like this
   if( !m_xine || !m_stream )
      return Engine::Uninitialised;

   switch( xine_get_status( m_stream ) )
   {
   case XINE_STATUS_PLAY: return xine_get_param( m_stream, XINE_PARAM_SPEED ) ? Engine::Playing : Engine::Paused;
   case XINE_STATUS_IDLE: return Engine::Empty; //FIXME this route never used!
   case XINE_STATUS_STOP:
   default:               return m_url.isEmpty() ? Engine::Empty : Engine::Loaded;
   }
}

uint
VideoWindow::posTimeLength( PosTimeLength type ) const
{
   int pos = 0, time = 0, length = 0;
   xine_get_pos_length( m_stream, &pos, &time, &length );

   switch( type ) {
      case Pos:    return pos;
      case Time:   return time;
      case Length: return length;
   }

   return 0; //--warning
}

uint
VideoWindow::volume() const
{
   //TODO I don't like the design
   return xine_get_param( m_stream, XINE_PARAM_AUDIO_AMP_LEVEL );
}

void
VideoWindow::seek( uint pos )
{
   bool wasPaused = false;

   // If we seek to the end the track ended event is sent, but it is
   // delayed as it happens in xine-event loop and before that we are
   // already processing the next seek event (if user uses mouse wheel
   // or keyboard to seek) and this causes the ui to think video is
   // stopped but xine is actually playing the track. Tada!
   // TODO set state based on events from xine only
   if( pos > 65534 )
      pos = 65534;

   switch( state() ) {
   case Engine::Uninitialised:
      //NOTE should never happen
      Debug::warning() << "Seek attempt thwarted! xine not initialised!\n";
      return;
   case Engine::Empty:
      Debug::warning() << "Seek attempt thwarted! No media loaded!\n";
      return;
   case Engine::Loaded:
      // then the state is changing and we should announce it
      play( pos );
      return;
   case Engine::Paused:
      // xine_play unpauses stream if stream was paused
      // was broken at 1.0.1 still
      wasPaused = true;
      xine_set_param( m_stream, XINE_PARAM_AUDIO_AMP_MUTE, 1 );
      break;
   default:
      ;
   }

   if( !TheStream::canSeek() ) {
      // for http streaming it is not a good idea to seek as xine freezes
      // and/or just breaks, this is xine 1.0.1
      Debug::warning() << "We won't try to seek as the media is not seekable!\n";
      return;
   }

   //TODO depend on a version that CAN seek in flacs!
   if( m_url.path().endsWith( ".flac", false ) ) {
      emit statusMessage( i18n("xine cannot currently seek in flac media") );
      return;
   }

   //better feedback
   //NOTE doesn't work! I can't tell why..
   Slider::instance()->QSlider::setValue( pos );
   Slider::instance()->repaint( false );

   const bool fullscreen = toggleAction("fullscreen")->isChecked();
   if( fullscreen ) {
      //TODO don't use OSD (sucks) show slider widget instead
      QString osd = "[";
      QChar separator = '|';

      for( uint x = 0, y = int(pos / (65535.0/20.0)); x < 20; x++ ) {
         if( x > y )
            separator = '.';
         osd += separator;
      }
      osd += ']';

      xine_osd_clear( m_osd );
      xine_osd_draw_text( m_osd, 0, 0, osd.utf8(), XINE_OSD_TEXT1 );
      xine_osd_show( m_osd, 0 );
   }

   xine_play( m_stream, (int)pos, 0 );

   if( fullscreen )
      //after xine_play because the hide command uses stream position
      xine_osd_hide( m_osd, xine_get_current_vpts( m_stream ) + 180000  ); //2 seconds

   if( wasPaused )
      xine_set_param( m_stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE ),
      xine_set_param( m_stream, XINE_PARAM_AUDIO_AMP_MUTE, 0 );
}

void
VideoWindow::setStreamParameter( int value )
{
   QCString sender = this->sender()->name();
   int parameter;

   if( sender == "hue" )
      parameter = XINE_PARAM_VO_HUE;
   else if( sender == "saturation" )
      parameter = XINE_PARAM_VO_SATURATION;
   else if( sender == "contrast" )
      parameter = XINE_PARAM_VO_CONTRAST;
   else if( sender == "brightness" )
      parameter = XINE_PARAM_VO_BRIGHTNESS;
   else if( sender == "subtitle_channels_menu" )
      parameter = XINE_PARAM_SPU_CHANNEL,
      value -= 2;
   else if( sender == "audio_channels_menu" )
      parameter = XINE_PARAM_AUDIO_CHANNEL_LOGICAL,
      value -= 2;
   else if( sender == "aspect_ratio_menu" )
      parameter = XINE_PARAM_VO_ASPECT_RATIO;
   else if( sender == "volume" )
      parameter = XINE_PARAM_AUDIO_AMP_LEVEL;
   else
      return;

   xine_set_param( m_stream, parameter, value );
}

const Engine::Scope&
VideoWindow::scope()
{
   using Analyzer::SCOPE_SIZE;

   static Engine::Scope scope( SCOPE_SIZE );

   if( xine_get_status( m_stream ) != XINE_STATUS_PLAY )
      return scope;

   //prune the buffer list and update the m_current_vpts timestamp
   timerEvent( 0 );

   for( int channels = xine_get_stream_info( m_stream, XINE_STREAM_INFO_AUDIO_CHANNELS ), frame = 0; frame < SCOPE_SIZE; )
   {
      MyNode *best_node = 0;

      for( MyNode *node = myList->next; node != myList; node = node->next )
         if( node->vpts <= m_current_vpts && (!best_node || node->vpts > best_node->vpts) )
            best_node = node;

      if( !best_node || best_node->vpts_end < m_current_vpts )
         break;

      int64_t
      diff  = m_current_vpts;
      diff -= best_node->vpts;
      diff *= 1<<16;
      diff /= myMetronom->pts_per_smpls;

      const int16_t*
      data16  = best_node->mem;
      data16 += diff;

      diff += diff % channels; //important correction to ensure we don't overflow the buffer
      diff /= channels;

      int
      n  = best_node->num_frames;
      n -= diff;
      n += frame; //clipping for # of frames we need

      if( n > SCOPE_SIZE )
         n = SCOPE_SIZE; //bounds limiting

      for( int a, c; frame < n; ++frame, data16 += channels ) {
         for( a = c = 0; c < channels; ++c )
            a += data16[c];

         a /= channels;
         scope[frame] = a;
      }

      m_current_vpts = best_node->vpts_end;
      m_current_vpts++; //FIXME needs to be done for some reason, or you get situations where it uses same buffer again and again
   }

   return scope;
}

void
VideoWindow::timerEvent( QTimerEvent* )
{
   /// here we prune the buffer list regularly
   #ifndef XINE_SAFE_MODE
   MyNode * const first_node = myList->next;
   MyNode const * const list_end = myList;

   m_current_vpts = (xine_get_status( m_stream ) == XINE_STATUS_PLAY)
         ? xine_get_current_vpts( m_stream )
         : std::numeric_limits<int64_t>::max();

   for( MyNode *prev = first_node, *node = first_node->next; node != list_end; node = node->next )
   {
      // we never delete first_node
      // this maintains thread-safety
      if( node->vpts_end < m_current_vpts ) {
         prev->next = node->next;

         free( node->mem );
         free( node );

         node = prev;
      }

      prev = node;
   }
   #endif
}

void
VideoWindow::customEvent( QCustomEvent *e )
{
   switch( e->type() - 2000 ) {
   case XINE_EVENT_UI_PLAYBACK_FINISHED:
      emit stateChanged( Engine::TrackEnded );
      break;

   case XINE_EVENT_FRAME_FORMAT_CHANGE:
      //TODO not ideal really
      debug() << "XINE_EVENT_FRAME_FORMAT_CHANGE\n";
      break;

   case XINE_EVENT_UI_CHANNELS_CHANGED:
   {
      char s[128]; //apparently sufficient

      {
         QStringList languages( "subtitle_channels_menu" );
         int channels = xine_get_stream_info( m_stream, XINE_STREAM_INFO_MAX_SPU_CHANNEL );
         for( int j = 0; j < channels; j++ )
            languages += xine_get_spu_lang( m_stream, j, s ) ? s : i18n("Channel %1").arg( j+1 );
         emit channelsChanged( languages );
      }

      {
         QStringList languages( "audio_channels_menu" );
         int channels = xine_get_stream_info( m_stream, XINE_STREAM_INFO_MAX_AUDIO_CHANNEL );
         for( int j = 0; j < channels; j++ )
            languages += xine_get_audio_lang( m_stream, j, s ) ? s : i18n("Channel %1").arg( j+1 );
         emit channelsChanged( languages );
      }
      break;
   }

   case 1000:
      #define message static_cast<QString*>(e->data())
      emit statusMessage( *message );
      delete message;
      break;

   case 1001:
      MessageBox::sorry( (*message).arg( m_url.prettyURL() ) );
      delete message;
      break;

   case 1002:
      emit titleChanged( *message );
      delete message;
      break;
      #undef message

   default:
      ;
   }
}

void
VideoWindow::xineEventListener( void *p, const xine_event_t* xineEvent )
{
   if( !p )
      return;

   #define engine static_cast<VideoWindow*>(p)

   switch( xineEvent->type ) {
   case XINE_EVENT_UI_NUM_BUTTONS: debug() << "XINE_EVENT_UI_NUM_BUTTONS\n"; break;
   case XINE_EVENT_MRL_REFERENCE: {
      //FIXME this is not the right way, it will have bugs
      debug() << "XINE_EVENT_MRL_REFERENCE\n";
      engine->m_url = QString::fromUtf8( ((xine_mrl_reference_data_t*)xineEvent->data)->mrl );
      QTimer::singleShot( 0, engine, SLOT(play()) );
      break;
   }
   case XINE_EVENT_DROPPED_FRAMES: debug() << "XINE_EVENT_DROPPED_FRAMES\n"; break;

   case XINE_EVENT_UI_PLAYBACK_FINISHED:
   case XINE_EVENT_FRAME_FORMAT_CHANGE:
   case XINE_EVENT_UI_CHANNELS_CHANGED:
   {
      QCustomEvent *ce;
      ce = new QCustomEvent( 2000 + xineEvent->type );
      ce->setData( const_cast<xine_event_t*>(xineEvent) );
      QApplication::postEvent( engine, ce );
      break;
   }

   case XINE_EVENT_UI_SET_TITLE:
      QApplication::postEvent( engine, new QCustomEvent(
            QEvent::Type(3002),
            new QString( QString::fromUtf8( static_cast<xine_ui_data_t*>(xineEvent->data)->str ) ) ) );
      break;

   case XINE_EVENT_PROGRESS:
   {
      xine_progress_data_t* pd = (xine_progress_data_t*)xineEvent->data;

      QString
      msg = "%1 %2%";
      msg = msg.arg( QString::fromUtf8( pd->description ) )
               .arg( KGlobal::locale()->formatNumber( pd->percent, 0 ) );

      QApplication::postEvent( engine, new QCustomEvent( QEvent::Type(3000), new QString( msg ) ) );
      break;
   }
   case XINE_EVENT_UI_MESSAGE:
   {
      debug() << "message received from xine\n";

      xine_ui_message_data_t *data = (xine_ui_message_data_t *)xineEvent->data;
      QString message;

      switch( data->type ) {
      case XINE_MSG_NO_ERROR:
      {
         //series of \0 separated strings, terminated with a \0\0
         char str[2000];
         char *p = str;
         for( char *msg = data->messages; !(*msg == '\0' && *(msg+1) == '\0'); ++msg, ++p )
            *p = *msg == '\0' ? '\n' : *msg;
         *p = '\0';

         debug() << str << endl;

         break;
      }

      case XINE_MSG_ENCRYPTED_SOURCE:
         message = i18n("The source is encrypted and can not be decrypted."); goto param;
      case XINE_MSG_UNKNOWN_HOST:
         message = i18n("The host is unknown for the URL: <i>%1</i>"); goto param;
      case XINE_MSG_UNKNOWN_DEVICE:
         message = i18n("The device name you specified seems invalid."); goto param;
      case XINE_MSG_NETWORK_UNREACHABLE:
         message = i18n("The network appears unreachable."); goto param;
      case XINE_MSG_AUDIO_OUT_UNAVAILABLE:
         message = i18n("Audio output unavailable; the device is busy."); goto param;
      case XINE_MSG_CONNECTION_REFUSED:
         message = i18n("The connection was refused for the URL: <i>%1</i>"); goto param;
      case XINE_MSG_FILE_NOT_FOUND:
         message = i18n("xine could not find the URL: <i>%1</i>"); goto param;
      case XINE_MSG_PERMISSION_ERROR:
         message = i18n("Access was denied for the URL: <i>%1</i>"); goto param;
      case XINE_MSG_READ_ERROR:
         message = i18n("The source cannot be read for the URL: <i>%1</i>"); goto param;
      case XINE_MSG_LIBRARY_LOAD_ERROR:
         message = i18n("A problem occurred while loading a library or decoder."); goto param;

      case XINE_MSG_GENERAL_WARNING:
      case XINE_MSG_SECURITY:
      default:

            if(data->explanation)
            {
               message += "<b>";
               message += QString::fromUtf8( (char*) data + data->explanation );
               message += "</b>";
            }
            else break; //if no explanation then why bother!

            //FALL THROUGH

      param:

            message.prepend( "<p>" );
            message += "<p>";

            if(data->parameters)
            {
               message += "xine says: <i>";
               message += QString::fromUtf8( (char*) data + data->parameters);
               message += "</i>";
            }
            else message += i18n("Sorry, no additional information is available.");

            QApplication::postEvent( engine, new QCustomEvent(QEvent::Type(3001), new QString(message)) );
      }

   } //case
   } //switch

   #undef engine
}

void
VideoWindow::toggleDVDMenu()
{
   xine_event_t e;
   e.type = XINE_EVENT_INPUT_MENU1;
   e.data = NULL;
   e.data_length = 0;

   xine_event_send( m_stream, &e );
}

void
VideoWindow::showOSD( const QString &message )
{
   if( m_osd ) {
      xine_osd_clear( m_osd );
      xine_osd_draw_text( m_osd, 0, 0, message.utf8(), XINE_OSD_TEXT1 );
      xine_osd_show( m_osd, 0 );
      xine_osd_hide( m_osd, xine_get_current_vpts( m_stream ) + 180000  ); //2 seconds
   }
}

QString
VideoWindow::fileFilter() const
{
   char *supportedExtensions = xine_get_file_extensions( m_xine );

   QString filter( "*." );
   filter.append( supportedExtensions );
   filter.remove( "txt" );
   filter.remove( "png" );
   filter.replace( ' ', " *." );

   std::free( supportedExtensions );

   return filter;
}

} //namespace Codeine
