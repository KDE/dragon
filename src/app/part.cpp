// Author:    Max Howell <max.howell@methylblue.com>, (C) 2005
// Copyright: See COPYING file that comes with this distribution

#include "codeine.h"
#include "debug.h"
#include "part.h"

#include <KAction>
#include <KAboutData>
#include <KActionCollection>
#include <KParts/GenericFactory>
#include <KToggleAction>

#include <QSlider>
#include <QTimer>
#include <QTimerEvent>

#include "videoWindow.h"

namespace Codeine
{
   typedef KParts::GenericFactory<Codeine::Part> Factory;
}


K_EXPORT_COMPONENT_FACTORY( libdragonpart, Codeine::Factory )


namespace Codeine
{
    Part::Part( QWidget* parentWidget, QObject* parent, const QStringList& /*args*/ )
            : ReadOnlyPart( parent )
            , m_statusBarExtension( new KParts::StatusBarExtension( this ) )
    {
        KActionCollection * const ac = actionCollection();

        setWidget( new VideoWindow( parentWidget ) ); //, widgetName 

        if( !videoWindow()->init() )
            //FIXME this will terminate the host, eg Konqueror
            Debug::fatal() << "Couldn't init xine!\n";

        KAction *play = new KToggleAction( KIcon("play"), i18n("Play"), ac );
        play->setObjectName( "player_play" );
        play->setShortcut( Qt::Key_Space );
        connect( play, SIGNAL( triggered() ), videoWindow(), SLOT( playPause() ) );
        ac->addAction( play->objectName(), play );
    //      KAction *mute = new KToggleAction( i18n("Mute"), "player_mute", Qt::Key_M, videoWindow(), SLOT(toggleMute()), actionCollection(), "mute" );
    //      KToolBar *toolBar = new MouseOverToolBar( widget() );
    //      toolBar->addAction( play );
    //      mute->plug( toolBar );

    /*      m_slider = new QSlider( Qt::Horizontal, toolBar, "slider" );
        m_slider->setMaxValue( 65535 );
        toolBar->setStretchableWidget( m_slider );
        toolBar->addSeparator(); //FIXME ugly */
        QObject *o = (QObject*)statusBar();
        connect( videoWindow(), SIGNAL(statusMessage( const QString& )), o, SLOT(message( const QString& )) );
        connect( videoWindow(), SIGNAL(titleChanged( const QString& )), o, SLOT(message( const QString& )) ); //FIXME
    }

    bool
    Part::openURL( const KUrl &url )
    {
        return videoWindow()->load( m_url = url );
    }

    bool
    Part::closeURL()
    {
        m_url = KUrl();
        videoWindow()->stop();
        return true;
    }

    KAboutData*
    Part::createAboutData()
    {
        // generic factory expects this on the heap
        //return new KAboutData( APP_NAME, PRETTY_NAME, APP_VERSION );
        return new KAboutData( APP_NAME, 0,
            ki18n(PRETTY_NAME), APP_VERSION,
            ki18n("A video player that has a usability focus"), KAboutData::License_GPL_V2,
            ki18n("Copyright 2006, Max Howell\nCopyright 2007, Ian Monroe"), KLocalizedString(),
            "http://dragonplayer.org",
            "imonroe@dragonplayer.org" );
    }

    QAction*
    action( const char* ) { return 0; }
    ///fake mainWindow for VideoWindow
    QWidget*
    mainWindow() { return 0;}

}
