/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "theStream.h"

#include <QHash>
#include <QDebug>

#include <Phonon/MediaController>
#include <Phonon/MediaObject>
#include <Phonon/MediaSource>
#include <Phonon/VideoWidget>

#include <KLocalizedString>
#include <KSharedConfig>

#include <Solid/Device>
#include <Solid/StorageVolume>

#include "videoWindow.h"

namespace Dragon
{

const char* TheStream::CHANNEL_PROPERTY = "channel";

QHash<int, QAction*> TheStream::s_aspectRatioActions;

KConfigGroup
TheStream::profile()
{
    Phonon::MediaSource::Type current = videoWindow()->m_media->currentSource().type();
    if( current == Phonon::MediaSource::Disc ) {
        QList< Solid::Device > deviceList = Solid::Device::listFromType( Solid::DeviceInterface::OpticalDisc );
        if( !deviceList.isEmpty() ) {
            Solid::StorageVolume* disc = deviceList.first().as<Solid::StorageVolume>();
            if( disc ) {
                QString discLabel = QStringLiteral("disc:%1,%2").arg(disc->uuid()).arg(disc->label() );
                return KConfigGroup( KSharedConfig::openConfig(), discLabel );
            } else
                qDebug() << "profile: doesn't convert into Solid::StorageVolume";
        } else
            qDebug() << "profile: empty device list";
    }
    //if not a disc, or Solid fails
    return KConfigGroup( KSharedConfig::openConfig(), url().toDisplayString() );
}

QUrl TheStream::url()
{
    return videoWindow()->m_media->currentSource().url();
}

bool
TheStream::canSeek()
{ return videoWindow()->m_media->isSeekable(); }

bool
TheStream::hasAudio()
{ return true; }

bool
TheStream::hasVideo()
{ return videoWindow()->m_media->hasVideo(); }

bool
TheStream::hasMedia()
{
    if(videoWindow()->m_media->currentSource().type() == Phonon::MediaSource::Invalid)
        return false;
    if(videoWindow()->m_media->currentSource().type() == Phonon::MediaSource::Empty)
        return false;
    //otherwise
    return true;
}

QSize
TheStream::defaultVideoSize()
{
    return videoWindow()->m_vWidget->sizeHint();
}

int
TheStream::aspectRatio()
{
    return engine()->m_vWidget->aspectRatio();
}

QAction*
TheStream::aspectRatioAction()
{
    return s_aspectRatioActions[ engine()->m_vWidget->aspectRatio() ];
}

void
TheStream::addRatio( int aspectEnum, QAction* ratioAction )
{
    s_aspectRatioActions[aspectEnum] = ratioAction;
}

int
TheStream::subtitleChannel()
{
    return engine()->m_controller->currentSubtitle().index();
}

int
TheStream::audioChannel()
{
    return engine()->m_controller->currentAudioChannel().index();
}

void
TheStream::setRatio( QAction* ratioAction )
{
    if( ratioAction )
        engine()->m_vWidget->setAspectRatio( (Phonon::VideoWidget::AspectRatio) s_aspectRatioActions.key( ratioAction ) );
}

QString
TheStream::prettyTitle()
{
    const QUrl url = videoWindow()->m_media->currentSource().url();
    QString artist, title;

    const QStringList artists = videoWindow()->m_media->metaData(Phonon::ArtistMetaData);
    if (!artists.isEmpty()) {
        artist = artists.first().trimmed();
    }

    const QStringList titles = videoWindow()->m_media->metaData(Phonon::TitleMetaData);
    if (!titles.isEmpty()) {
        title = titles.first().trimmed();
    }

    if (hasVideo() && !title.isEmpty())
        return title;
    else if (!title.isEmpty() && !artist.isEmpty())
        return artist + QLatin1String( " - " ) + title;
    else if (url.scheme() != QLatin1String( "http" ) && !url.fileName().isEmpty()) {
        const QString n = url.fileName();
        //toLatin1 sense fromPercentEncoding takes a QByteArray
        //I'm not sure about this whole method though, should double check that titles make sense
        //using QString::toLatin1() will display "????" in titlelabel. Should be QString::toUtf8().   patched by nihui, Jul.6th, 2008
        return QUrl::fromPercentEncoding( n.left( n.lastIndexOf( QLatin1Char( '.' ) ) ).replace( QLatin1Char( '_' ), QLatin1Char( ' ' ) ).toUtf8() ); //krazy:exclude-qclasses
    } else if (videoWindow()->m_media->currentSource().discType() == Phonon::Cd) {
        return i18n("Track %1/%2", videoWindow()->m_media->metaData().value(QStringLiteral("TRACK-NUMBER")),
                    videoWindow()->m_media->metaData().value(QStringLiteral("TRACK-COUNT")));
    } else {
        return url.toDisplayString();
    }
}

QString
TheStream::fullTitle()
{/*
      QString artist,album,title;
      QStringList artists = m_mediaObject->metaData(Phonon::ArtistMetaData);
      QStringList albums = m_mediaObject->metaData(Phonon::AlbumMetaData);
      QStringList titles = m_mediaObject->metaData(Phonon::TitleMetaData)
      title = (title ? titles.join( QLatin1String( " ") : "" ) );
      album = (albums ? albums.join( QLatin1String( " "): "" ) );
      artist = (artists ? artists.join( QLatin1String( " "): "" ) );

      return title + "\n" + album + "\n" + artist;*/
    return QString();
}

bool
TheStream::hasProfile()
{
    return KSharedConfig::openConfig()->hasGroup( url().toDisplayString() );
}

QString
TheStream::metaData(Phonon::MetaData key)
{
    QStringList values = videoWindow()->m_media->metaData(key);
    qDebug() << values;
    return (values.isEmpty()) ? QString() : values.join(QLatin1Char( ' ' ));
}

QString TheStream::discId()
{
    QStringList values = videoWindow()->m_media->metaData(Phonon::MusicBrainzDiscIdMetaData);
    if (!values.isEmpty())
        return values.first();

    return QString();
}

}
