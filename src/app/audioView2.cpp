/*
    SPDX-FileCopyrightText: 2012 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "audioView2.h"
#include "ui_audioView2.h"
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

    setupAnalyzer();
    connect(engine(), &VideoWindow::metaDataChanged, this, &AudioView2::update);
}

AudioView2::~AudioView2()
{
    delete ui;
}

void AudioView2::setupAnalyzer()
{
    engine()->setupAnalyzer(ui->m_analyzer);
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
