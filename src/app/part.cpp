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

#include "part.h"

#include "actions.h"
#include "codeine.h"
#include "partToolBar.h"
#include "videoWindow.h"

#include <KAction>
#include <KAboutData>
#include <KActionCollection>
#include <KDebug>
#include <KMenu>
#include <KPluginFactory>
#include <KToggleAction>

#include <QSlider>
#include <QTimer>
#include <QTimerEvent>
#include <QVBoxLayout>

K_PLUGIN_FACTORY(CodeineFactory, registerPlugin<Dragon::Part>();)
K_EXPORT_PLUGIN(CodeineFactory("libdragon"))

namespace Dragon
{
    Part::Part( QWidget* parentWidget, QObject* parent, const QList<QVariant>& /*args*/ )
            : ReadOnlyPart( parent )
            , m_statusBarExtension( new KParts::StatusBarExtension( this ) )
            , m_playPause( 0 )
    {
        KActionCollection * const ac = actionCollection();

        setWidget( new QWidget( parentWidget ) ); //, widgetName 
        QBoxLayout* layout = new QVBoxLayout();
        layout->setContentsMargins( 0, 0, 0, 0 );

        KToolBar *toolBar = new MouseOverToolBar( widget() );
        layout->addWidget( toolBar );
        layout->addWidget( new VideoWindow( widget() ) );

        m_playPause = new Dragon::PlayAction( videoWindow(), SLOT(playPause()), ac );
        toolBar->addAction( m_playPause );
        {
            QWidget* slider = videoWindow()->newPositionSlider();
            KAction* sliderAction = new KAction( i18n("Position Slider"), ac );
            sliderAction->setObjectName( QLatin1String( "position_slider" ) );
            sliderAction->setDefaultWidget( slider );
            ac->addAction( sliderAction->objectName(), sliderAction );
            toolBar->addAction( sliderAction );
        }
        connect( engine(), SIGNAL(stateChanged(Phonon::State)), this, SLOT(engineStateChanged(Phonon::State)) );
        videoWindow()->setContextMenuPolicy( Qt::CustomContextMenu );
        connect( videoWindow(), SIGNAL(customContextMenuRequested()), this, SLOT(videoContextMenu()) );

        widget()->setLayout( layout );
    }

    void
    Part::engineStateChanged( Phonon::State state )
    {
        m_playPause->setChecked( state == Phonon::PlayingState );
    }

    bool
    Part::openUrl( const KUrl &url )
    {
        kDebug() << "playing " << url;
        bool ret = videoWindow()->load( m_url = url );
        videoWindow()->play();
        return ret;
    }

    bool
    Part::closeUrl()
    {
        m_url = KUrl();
        videoWindow()->stop();
        return true;
    }

    KAboutData*
    Part::createAboutData()
    {
        // generic factory expects this on the heap
        //return new KAboutData( APP_NAME, "Dragon Player", APP_VERSION );
        return new KAboutData( APP_NAME, 0,
            ki18n("Dragon Player"), APP_VERSION,
            ki18n("A video player that has a usability focus"), KAboutData::License_GPL_V2,
            ki18n("Copyright 2006, Max Howell\nCopyright 2007, Ian Monroe"), KLocalizedString(),
            "http://multimedia.kde.org",
            "imonroe@kde.org" );
    }

    bool 
    Part::openFile() //pure virtual in base class
    {
        return false; 
    }
    
    void
    Part::videoContextMenu( const QPoint & pos )
    {
        KMenu menu;
        menu.addAction( m_playPause );
        menu.exec( pos );
    }

    QAction*
    action( const char* /*actionName*/ ) { return 0; }
    ///fake mainWindow for VideoWindow
    QWidget*
    mainWindow() { return 0;}

}

#include "part.moc"
