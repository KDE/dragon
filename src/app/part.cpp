/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "part.h"

#include "actions.h"
#include "codeine.h"
#include "partToolBar.h"
#include "videoWindow.h"

#if KPARTS_VERSION >= QT_VERSION_CHECK(5, 77, 0)
#include <KPluginMetaData>
#else
#include <KAboutData>
#endif
#include <KActionCollection>
#include <KPluginFactory>
#include <KToggleAction>
#include <KLocalizedString>

#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QSlider>
#include <QTimer>
#include <QTimerEvent>
#include <QVBoxLayout>
#include <QWidgetAction>

K_PLUGIN_FACTORY_WITH_JSON(CodeineFactory, "dragonplayer_part.json",
                           registerPlugin<Dragon::Part>();)

namespace Dragon
{
#if KPARTS_VERSION >= QT_VERSION_CHECK(5, 77, 0)
    Part::Part(QWidget* parentWidget, QObject* parent, const KPluginMetaData& metaData, const QVariantList& /*args*/)
#else
    Part::Part(QWidget* parentWidget, QObject* parent, const QVariantList& /*args*/)
#endif
        : ReadOnlyPart( parent )
        , m_statusBarExtension( new KParts::StatusBarExtension( this ) )
        , m_playPause( nullptr )
    {
#if KPARTS_VERSION >= QT_VERSION_CHECK(5, 77, 0)
        setMetaData(metaData);
#else
        setComponentData(*createAboutData());
#endif
        KActionCollection * const ac = actionCollection();

        setWidget( new QWidget( parentWidget ) ); //, widgetName
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setContentsMargins( 0, 0, 0, 0 );

        QToolBar *toolBar = new MouseOverToolBar( widget() );
        layout->addWidget( toolBar );
        layout->addWidget( new VideoWindow( widget() ) );

        m_playPause = new Dragon::PlayAction(videoWindow(), &VideoWindow::playPause, ac);
        toolBar->addAction( m_playPause );
        {
            QWidget* slider = videoWindow()->newPositionSlider();
            QWidgetAction* sliderAction = new QWidgetAction( ac );
            sliderAction->setText(i18n("Position Slider"));
            sliderAction->setObjectName( QLatin1String( "position_slider" ) );
            sliderAction->setDefaultWidget( slider );
            ac->addAction( sliderAction->objectName(), sliderAction );
            toolBar->addAction( sliderAction );
        }
        connect(engine(), &VideoWindow::stateChanged, this, &Part::engineStateChanged);
        videoWindow()->setContextMenuPolicy( Qt::CustomContextMenu );
        connect(videoWindow(), &QWidget::customContextMenuRequested, this, &Part::videoContextMenu);

        widget()->setLayout( layout );
    }

    void Part::engineStateChanged( Phonon::State state )
    {
        m_playPause->setChecked( state == Phonon::PlayingState );
    }

    bool Part::openUrl( const QUrl &url )
    {
        qDebug() << "playing " << url;
        m_url = url;
        bool ret = videoWindow()->load( m_url );
        videoWindow()->play();
        return ret;
    }

    bool Part::closeUrl()
    {
        m_url = QUrl();
        videoWindow()->stop();
        return true;
    }

#if KPARTS_VERSION < QT_VERSION_CHECK(5, 77, 0)
    KAboutData* Part::createAboutData()
    {
        // generic factory expects this on the heap
        //return new KAboutData( APP_NAME, "Dragon Player", APP_VERSION );
        return new KAboutData( QStringLiteral(APP_NAME),
                               i18n("Dragon Player"), QStringLiteral(APP_VERSION),
                               i18n("A video player that has a usability focus"), KAboutLicense::GPL_V2,
                               i18n("Copyright 2006, Max Howell\nCopyright 2007, Ian Monroe"), QString(),
                               QStringLiteral("https://multimedia.kde.org"),
                               QStringLiteral("imonroe@kde.org") );
    }
#endif

    void Part::videoContextMenu( const QPoint & pos )
    {
        QMenu menu;
        menu.addAction( m_playPause );
        menu.exec( pos );
    }

    QAction *action( const char* /*actionName*/ ) { return nullptr; }
    ///fake mainWindow for VideoWindow
    QWidget *mainWindow() { return nullptr; }

}

#include "part.moc"
