// Author:    Max Howell <max.howell@methylblue.com>, (C) 2005
// Copyright: See COPYING file that comes with this distribution

#include "codeine.h"
#include "debug.h"
#include <kaboutdata.h>
#include <kparts/genericfactory.h>
#include "part.h"
#include <qtimer.h>
#include "toolbar.h"
#include "videoWindow.h"

#include <kaction.h>
#include <qslider.h>

namespace Codeine
{
   typedef KParts::GenericFactory<Codeine::Part> Factory;
}


K_EXPORT_COMPONENT_FACTORY( libcodeine, Codeine::Factory )


namespace Codeine
{
   Part::Part( QWidget *parentWidget, const char *widgetName, QObject *parent, const char *name, const QStringList& )
         : ReadOnlyPart( parent, name )
         , m_statusBarExtension( new KParts::StatusBarExtension( this ) )
   {
      setInstance( Factory::instance() );
      setWidget( new VideoWindow( parentWidget, widgetName ) );

      if( !videoWindow()->init() )
         //FIXME this will terminate the host, eg Konqueror
         Debug::fatal() << "Couldn't init xine!\n";

      KAction *play = new KToggleAction( i18n("Play"), "player_play", Qt::Key_Space, videoWindow(), SLOT(togglePlay()), actionCollection(), "play" );
      KAction *mute = new KToggleAction( i18n("Mute"), "player_mute", Qt::Key_M, videoWindow(), SLOT(toggleMute()), actionCollection(), "mute" );
      KToolBar *toolBar = new MouseOverToolBar( widget() );
      play->plug( toolBar );
      mute->plug( toolBar );
      m_slider = new QSlider( Qt::Horizontal, toolBar, "slider" );
      m_slider->setMaxValue( 65535 );
      toolBar->setStretchableWidget( m_slider );
      toolBar->addSeparator(); //FIXME ugly

      QObject *o = (QObject*)statusBar();
      connect( videoWindow(), SIGNAL(statusMessage( const QString& )), o, SLOT(message( const QString& )) );
      connect( videoWindow(), SIGNAL(titleChanged( const QString& )), o, SLOT(message( const QString& )) ); //FIXME
   }

   bool
   Part::openURL( const KURL &url )
   {
      //FIXME nasty, we'd rather not do this way
      killTimers();
      startTimer( 100 );

      return videoWindow()->play( m_url = url );
   }

   bool
   Part::closeURL()
   {
      m_url = KURL();
      videoWindow()->eject();
      return true;
   }

   KAboutData*
   Part::createAboutData()
   {
      // generic factory expects this on the heap
      return new KAboutData( APP_NAME, PRETTY_NAME, APP_VERSION );
   }

   void
   Part::timerEvent( QTimerEvent* )
   {
      m_slider->setValue( videoWindow()->position() );
   }
}
