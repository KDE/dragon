/***********************************************************************
 * Copyright 2012 Harald Sitter <sitter@kde.org>
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

#include "audioView2.h"
#include "ui_audioView2.h"

#include <KGlobalSettings>
#include <KFontSizeAction>

#include "theStream.h"
#include "videoWindow.h"

namespace Dragon {

AudioView2::AudioView2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AudioView2)
{
    ui->setupUi(this);

    ui->m_analyzerFrame->setMaximumSize(ui->m_analyzer->maximumSize());
    ui->m_analyzerFrame->setMinimumSize(ui->m_analyzer->minimumSize());

    QFont boldFont = KGlobalSettings::generalFont();
    boldFont.setBold(true);
    ui->m_track->setFont(boldFont);
    ui->m_artist->setFont(KGlobalSettings::generalFont());
    ui->m_album->setFont(KGlobalSettings::generalFont());

    engine()->setupAnalyzer(ui->m_analyzer);
    connect(engine(), SIGNAL(metaDataChanged()), this, SLOT(update()));
}

AudioView2::~AudioView2()
{
    delete ui;
}

void AudioView2::enableDemo(bool enable)
{
    ui->m_analyzer->enableDemo(enable);
}

void AudioView2::update()
{
    ui->m_artist->setText( TheStream::metaData( Phonon::ArtistMetaData ) );
    ui->m_album->setText( TheStream::metaData( Phonon::AlbumMetaData ) );
    ui->m_track->setText( TheStream::metaData( Phonon::TitleMetaData ) );
//    { //somewhat of a longshot: try to find Amarok cover for the music
//        QString imagePath = checkForAmarokImage( artist, album );
//        if(imagePath.isNull())
//        {
//            delete m_image;
//            m_image = 0;
//            m_layout->setColumnFixedWidth(0, 0.0);
//        }
//        else
//        {
//            m_layout->setColumnFixedWidth(0, COVER_COLUMN_WIDTH);
//            if(!m_image)
//            {
//                m_image = new QGraphicsPixmapItem();
//                scene()->addItem( m_image );
//                m_image->setPos( 3.0, 3.0 );
//            }
//            QPixmap cover( imagePath );
//            m_image->setPixmap( cover );
//            qreal width = static_cast<qreal>( cover.width() );
//            qreal scale = COVER_WIDTH / width;
//            m_image->setScale( scale );
//        }
//    }
}


void AudioView2::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

} // namespace Dragon
