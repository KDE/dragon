// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#define CODEINE_DEBUG_PREFIX "engine"

#include "debug.h"
#include <kglobalsettings.h>
#include <klocale.h>
#include "mxcl.library.h"
#include <qapplication.h> //::sendEvent()
#include <qdatetime.h>    //::play()
#include <qdir.h>         //QDir::homeDir()
//Added by qt3to4:
#include <QCustomEvent>
#include <Q3CString>
#include <QEvent>
#include <xine.h>
#include "videoWindow.h"


namespace Codeine {


bool
VideoWindow::init()
{
   mxcl::WaitCursor allocateOnStack;

   debug() << "xine_new()\n";
   m_xine = xine_new();
   if( !m_xine )
      return false;

   debug() << "xine_config_load()\n";
   xine_config_load( m_xine, QFile::encodeName( QDir::homeDirPath() + "/.xine/config" ) );

   debug() << "xine_init()\n";
   xine_init( m_xine );

   debug() << "xine_open_video_driver()\n";
   m_videoPort = xine_open_video_driver( m_xine, "auto", XINE_VISUAL_TYPE_X11, x11Visual() );

   debug() << "xine_open_audio_driver()\n";
   m_audioPort = xine_open_audio_driver( m_xine, "auto", NULL );

   debug() << "xine_stream_new()\n";
   m_stream = xine_stream_new( m_xine, m_audioPort, m_videoPort );
   if( !m_stream )
      return false;

   if( !m_audioPort )
      MessageBox::error( i18n("xine was unable to initialize any audio-drivers.") );
   if( !m_videoPort )
      MessageBox::error( i18n("xine was unable to initialize any video-drivers.") );

   debug() << "xine_osd_new()\n";
   m_osd = xine_osd_new( m_stream, 10, 10, 1000, 18 * 6 + 10 );
   if( m_osd ) {
      xine_osd_set_font( m_osd, "sans", 18 );
      xine_osd_set_text_palette( m_osd, XINE_TEXTPALETTE_WHITE_BLACK_TRANSPARENT, XINE_OSD_TEXT1 );
   }

   debug() << "xine_event_create_listener_thread()\n";
   xine_event_create_listener_thread(
         m_eventQueue = xine_event_new_queue( m_stream ),
         &VideoWindow::xineEventListener, (void*)this );

   { /// set save directory
      xine_cfg_entry_t config;

      if( xine_config_lookup_entry( m_xine, "misc.save_dir", &config ) ) {
         const Q3CString dir = KGlobalSettings::desktopPath().local8Bit();
         config.str_value = qstrdup( dir );
         xine_config_update_entry( m_xine, &config );
      }
   }

   return true;
}

bool
VideoWindow::play( KURL url )
{
   DEBUG_BLOCK

   m_url = url;

   mxcl::WaitCursor allocateOnStack;

   //TODO make sensible
   if( url.protocol() == "http" ) {
      /// automatically save http streams to Desktop folder

      const QString fileName = url.filename();

      QString
      u  = url.url();
      u += "#save:";
      u += url.host();
      u += " [";
      u += QDate::currentDate().toString();
      u += ']';
      u += fileName.mid( fileName.findRev( '.' ) + 1 ).lower();

      url = u;
   }

   debug() << "About to load..\n";
   if( xine_open( m_stream, url.url().local8Bit() ) )
   {
      debug() << "About to play..\n";
      if( xine_play( m_stream, 0, 0 ) )
         return true;
   }

   showErrorMessage();
   return false;
}

void
VideoWindow::togglePlay()
{
   if( xine_get_param( m_stream, XINE_PARAM_SPEED ) ) {
      xine_set_param( m_stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE );
      xine_set_param( m_stream, XINE_PARAM_AUDIO_CLOSE_DEVICE, 1);
   }
   else
      xine_set_param( m_stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL );
}

void
VideoWindow::toggleMute()
{
   bool const muted = xine_get_param( m_stream, XINE_PARAM_AUDIO_MUTE );
   xine_set_param( m_stream, XINE_PARAM_AUDIO_MUTE, !muted );
}

void
VideoWindow::eject()
{
   m_url = KURL();

   xine_stop( m_stream );
}

int
VideoWindow::position()
{
   int pos = 0, time = 0, length = 0;
   xine_get_pos_length( m_stream, &pos, &time, &length );

   return pos;
}

void
VideoWindow::showErrorMessage()
{
   const QString filename = m_url.fileName();

   debug() << "xine_get_error()\n";

   // NOTE these error messages are somewhat customised
   // relative to the main application
   // This is because when embedded in some other application
   // the error messages have no context, so we must say that we are a video player!

   switch( xine_get_error( m_stream ) )
   {
   case XINE_ERROR_NO_INPUT_PLUGIN:
      MessageBox::sorry( i18n("The Codeine video player could not find an input plugin for '%1'.").arg( filename ) );
      break;
   case XINE_ERROR_NO_DEMUX_PLUGIN:
      MessageBox::sorry( i18n("The Codeine video player could not find a demux plugin for '%1'.").arg( filename ) );
      break;
   case XINE_ERROR_DEMUX_FAILED:
      MessageBox::sorry( i18n("The Codeine video player failed to demux '%1'; please check your xine installation.").arg( filename ) );
      break;
   case XINE_ERROR_INPUT_FAILED:
   case XINE_ERROR_MALFORMED_MRL:
   case XINE_ERROR_NONE:
      MessageBox::sorry( i18n("The Codeine video player reports an internal error; please check your xine installation.") );
      break;
   }
}

void
VideoWindow::customEvent( QCustomEvent *e )
{
   switch( e->type() - 2000 ) {
   case XINE_EVENT_UI_PLAYBACK_FINISHED:
//FIXME      emit stateChanged( Engine::TrackEnded );
      break;

   case 1000:
      #define message static_cast<QString*>(e->data())
      emit statusMessage( *message );
      delete message;
      break;

   case 1001:
      MessageBox::sorry( (*message).arg( "FIXME" ) ); //FIXME
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
   case XINE_EVENT_MRL_REFERENCE:
   {
      mxcl::WaitCursor allocateOnStack;
      const char *mrl = ((xine_mrl_reference_data_t*)xineEvent->data)->mrl;

      debug() << "XINE_EVENT_MRL_REFERENCE: " << mrl << endl;

      if( xine_open( engine->m_stream, mrl ) )
         xine_play( engine->m_stream, 0, 0 );

      break;
   }

   case XINE_EVENT_UI_NUM_BUTTONS: debug() << "XINE_EVENT_UI_NUM_BUTTONS\n"; break;
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
      debug() << "Message received from xine\n";

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

} //namespace Codeine
