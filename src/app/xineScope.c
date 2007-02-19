/* Author: Max Howell <max.howell@methylblue.com>, (C) 2004
   Copyright: See COPYING file that comes with this distribution */

/* gcc doesn't like inline for me */
#define inline
/* need access to port_ticket */
#define XINE_ENGINE_INTERNAL

#include "xineScope.h"
#include <xine/post.h>
#include <xine/xine_internal.h>


static MyNode theList;
static metronom_t theMetronom;
static int myChannels = 0;

MyNode* const myList = &theList;
metronom_t* const myMetronom = &theMetronom;


/* defined in xineEngine.cpp */
extern void _debug( const char * );


/*************************
* post plugin functions *
*************************/

static int
scope_port_open( xine_audio_port_t *port_gen, xine_stream_t *stream, uint32_t bits, uint32_t rate, int mode )
{
   _debug( "scope_port_open()\n" );

   #define port ((post_audio_port_t*)port_gen)

   _x_post_rewire( (post_plugin_t*)port->post );
   _x_post_inc_usage( port );

   port->stream = stream;
   port->bits = bits;
   port->rate = rate;
   port->mode = mode;

   myChannels = _x_ao_mode2channels( mode );

   return port->original_port->open( port->original_port, stream, bits, rate, mode );
}

static void
scope_port_close( xine_audio_port_t *port_gen, xine_stream_t *stream )
{
   _debug( "scope_port_close()\n" );

   port->stream = NULL;
   port->original_port->close( port->original_port, stream );

   _x_post_dec_usage( port );
}

static void
scope_port_put_buffer( xine_audio_port_t *port_gen, audio_buffer_t *buf, xine_stream_t *stream )
{
   MyNode *new_node;
   const int num_samples = buf->num_frames * myChannels;

   /* we are too simple to handle 8bit */
   /* what does it mean when stream == NULL? */
   if( port->bits == 8 ) {
      port->original_port->put_buffer( port->original_port, buf, stream ); return; }

   /* I keep my own metronom because xine wouldn't for some reason */
   memcpy( myMetronom, stream->metronom, sizeof(metronom_t) );

   new_node             = malloc( sizeof(MyNode) );
   new_node->vpts       = myMetronom->got_audio_samples( myMetronom, buf->vpts, buf->num_frames );
   new_node->num_frames = buf->num_frames;
   new_node->mem        = malloc( num_samples * 2 );
   memcpy( new_node->mem, buf->mem, num_samples * 2 );

   {
      int64_t
      K  = myMetronom->pts_per_smpls; /*smpls = 1<<16 samples*/
      K *= num_samples;
      K /= (1<<16);
      K += new_node->vpts;

      new_node->vpts_end = K;
   }

   /* pass data to original port */
   port->original_port->put_buffer( port->original_port, buf, stream );

   /* finally we should append the current buffer to the list
   * NOTE this is thread-safe due to the way we handle the list in the GUI thread */
   new_node->next = myList->next;
   myList->next   = new_node;

   #undef port
}

static void
scope_dispose( post_plugin_t *this )
{
   free( this );
}


/************************
* plugin init function *
************************/

xine_post_t*
scope_plugin_new( xine_t *xine, xine_audio_port_t *audio_target )
{
   if( audio_target == NULL )
      return NULL;

   post_plugin_t *post_plugin = xine_xmalloc( sizeof(post_plugin_t) );

   {
      post_plugin_t     *this = post_plugin;
      post_in_t         *input;
      post_out_t        *output;
      post_audio_port_t *port;

      _x_post_init( this, 1, 0 );

      port = _x_post_intercept_audio_port( this, audio_target, &input, &output );
      port->new_port.open       = scope_port_open;
      port->new_port.close      = scope_port_close;
      port->new_port.put_buffer = scope_port_put_buffer;

      this->xine_post.audio_input[0] = &port->new_port;
      this->xine_post.type = PLUGIN_POST;

      this->dispose = scope_dispose;
   }

   /* code is straight from xine_init_post()
      can't use that function as it only dlopens the plugins
      and our plugin is statically linked in */

   post_plugin->running_ticket = xine->port_ticket;
   post_plugin->xine = xine;

   return &post_plugin->xine_post;
}
