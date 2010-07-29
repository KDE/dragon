/***********************************************************************
 * Copyright 2008  David Edmundson <kde@davidedmundson.co.uk>
 * Copyright 2010  Ian Monroe      <ian@monroe.nu>
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

#include "audioView.h"

#include "analyzer/blockAnalyzer.h"
#include "textItem.h"
#include "theStream.h"
#include "videoWindow.h"


#include <QFileInfo>
#include <QGraphicsItem>
#include <QGraphicsGridLayout>
#include <QGraphicsPixmapItem>
#include <QLabel>

#include <Plasma/Label>
#include <KMD5>
#include <KStandardDirs>

namespace Dragon
{

AudioView::AudioView( QWidget *parent) 
    : QGraphicsView( parent )
    , m_image(0)
{
/*
   |  0    |   1       |
 0 | Image | Artist    |
 1 | Image | Album     |
 2 | Image | # - Track |
 3 |    Analyzer       |
 */  
    m_widget = new QGraphicsWidget();
    m_layout = new QGraphicsGridLayout();
    m_layout->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    QGraphicsScene* scene = new QGraphicsScene();
    
    #define setup_label(l) \
      l = new ExpandingTextItem(m_widget); \
      l->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
      
      
    setup_label(m_artist);
    m_layout->addItem(m_artist, 0, 1, Qt::AlignVCenter);
    setup_label(m_album);
    m_layout->addItem(m_album, 1, 1, Qt::AlignVCenter);
    setup_label(m_track);
    m_layout->addItem(m_track, 2, 1,  Qt::AlignVCenter);
    
    m_analyzer = new BlockAnalyzer(0);
    QGraphicsProxyWidget* analyzerWidget = scene->addWidget(m_analyzer);
    analyzerWidget->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    analyzerWidget->setMinimumHeight(100.0);
    analyzerWidget->setMaximumWidth(450.0);
    analyzerWidget->setMaximumHeight(300.0);
    m_layout->addItem(analyzerWidget, 3, 0, 1, 2, Qt::AlignCenter);
    engine()->setupAnalyzer(m_analyzer);
    m_layout->setRowMinimumHeight(2, 100.0);
    
    m_widget->setLayout(m_layout);
    setScene(scene);
    scene->addItem(m_widget);
    #undef setup_label
}

AudioView::~AudioView()
{//scene deleted automatically
}

void AudioView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    QRectF newRect(QPointF(0.0,0.0), event->size());
    scene()->setSceneRect(newRect);
    m_widget->resize( event->size());
    kDebug() << scene()->sceneRect() << m_widget->rect();
}

#define COVER_WIDTH 100.0
#define COVER_COLUMN_WIDTH (COVER_WIDTH+3.0)

void
AudioView::updateText()
{
    QString artist = TheStream::metaData( Phonon::ArtistMetaData );
    kDebug() << "its being set: " << artist << scene()->sceneRect();
    m_artist->setPlainText(artist);
    QString album = TheStream::metaData( Phonon::AlbumMetaData );
    m_album->setPlainText(album);
    QString trackString;
    {
        QString trackName = TheStream::metaData( Phonon::TitleMetaData );
        QString trackNumber = TheStream::metaData( Phonon::TracknumberMetaData );
        bool okInt = false;
        if ( trackNumber.toInt(&okInt) > 0 && okInt)
        {
            trackString =  QString( trackNumber + QLatin1String(" - ") + trackName );
        } 
        else
            trackString = trackName;
    }
    m_track->setPlainText( trackString );
    { //somewhat of a longshot: try to find Amarok cover for the music
        QString imagePath = checkForAmarokImage( artist, album );
        if(imagePath.isNull())
        {
            delete m_image;
            m_image = 0;
            m_layout->setColumnFixedWidth(0, 0.0);
        }
        else
        {
            m_layout->setColumnFixedWidth(0, COVER_COLUMN_WIDTH);
            if(!m_image)
            {
                m_image = new QGraphicsPixmapItem();
                scene()->addItem( m_image );
                m_image->setPos( 3.0, 3.0 );
            }
            QPixmap cover( imagePath );
            m_image->setPixmap( cover );
            qreal width = static_cast<qreal>( cover.width() );
            qreal scale = COVER_WIDTH / width;
            m_image->setScale( scale );
        }
    }
}

QString 
AudioView::checkForAmarokImage(const QString& artist, const QString& album)
{
    KMD5 context( artist.toLower().toLocal8Bit() + album.toLower().toLocal8Bit() );
    const QByteArray md5sum = context.hexDigest();
    QString location  = KStandardDirs::locate("data", QLatin1String("amarok/albumcovers/large/") + md5sum);
    if(QFileInfo(location).exists())
        return location;
    else
        return QString();
}


}
#include "audioView.moc"
