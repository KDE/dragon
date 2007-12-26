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
 //  typedef KParts::GenericFactory<Codeine::Part> Factory;
}

//K_EXPORT_COMPONENT_FACTORY( libdragonpart, Codeine::Factory )
//K_EXPORT_PLUGIN( Codeine::Factory )
K_PLUGIN_FACTORY(CodeineFactory, registerPlugin<Codeine::Part>();)
K_EXPORT_PLUGIN(CodeineFactory("libdragon"))

namespace Codeine
{
    Part::Part( QWidget* parentWidget, QObject* parent, const QList<QVariant>& /*args*/ )
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
    Part::openUrl( const KUrl &url )
    {
        DEBUG_BLOCK
        debug() << "playing " << url;
        bool ret = videoWindow()->load( m_url = url );
        videoWindow()->play();
        return ret;
    }

    bool
    Part::closeUrl()
    {
        DEBUG_BLOCK
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

    bool 
    Part::openFile() //pure virtual in base class
    {
        DEBUG_BLOCK
        return false; 
    }

    QAction*
    action( const char* ) { return 0; }
    ///fake mainWindow for VideoWindow
    QWidget*
    mainWindow() { return 0;}

}

#include "part.moc"
