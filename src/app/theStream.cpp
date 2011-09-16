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

#include "theStream.h"

#include <QHash>

#include <KDebug>
#include <KGlobal>
#include <KUrl>
#include <KLocale>
#include <Phonon/MediaController>
#include <Phonon/MediaObject>
#include <Phonon/MediaSource>
#include <Phonon/VideoWidget>
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
        if( current == Phonon::MediaSource::Disc )
        {
            QList< Solid::Device > deviceList = Solid::Device::listFromType( Solid::DeviceInterface::OpticalDisc );
            if( !deviceList.isEmpty() )
            {
                Solid::StorageVolume* disc = deviceList.first().as<Solid::StorageVolume>();
                if( disc )
                {
                    QString discLabel = QString::fromLatin1("disc:%1,%2").arg( disc->uuid(), disc->label() );
                    return KConfigGroup( KGlobal::config(), discLabel );
                }
                else
                    kDebug() << "profile: doesn't convert into Solid::StorageVolume";
            }
            else
                kDebug() << "profile: empty device list";
        }
        //if not a disc, or Solid fails
        return KConfigGroup( KGlobal::config(), url().prettyUrl() );
    }

    KUrl
    TheStream::url()
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
        const KUrl& url      = videoWindow()->m_media->currentSource().url();
        QString artist, title;

        QStringList artists = videoWindow()->m_media->metaData(QLatin1String( "ARTIST" ));
        if (!artists.isEmpty()) {
            artist = artists.first();
        }

        QStringList titles = videoWindow()->m_media->metaData(QLatin1String( "TITLE" ));
        if (!titles.isEmpty()) {
            title  = titles.first();
        }

        if (hasVideo() && !title.isEmpty())
            return title;
        else if (!title.isEmpty() && !artist.isEmpty())
            return artist + QLatin1String( " - " ) + title;
        else if (url.protocol() != QLatin1String( "http" ) && !url.fileName().isEmpty())
        {
            const QString n = url.fileName();
            //toLatin1 sense fromPercentEncoding takes a QByteArray
            //I'm not sure about this whole method though, should double check that titles make sense
            //using QString::toLatin1() will display "????" in titlelabel. Should be QString::toUtf8().   patched by nihui, Jul.6th, 2008
            return QUrl::fromPercentEncoding( n.left( n.lastIndexOf( QLatin1Char( '.' ) ) ).replace( QLatin1Char( '_' ), QLatin1Char( ' ' ) ).toUtf8() ); //krazy:exclude-qclasses

        }
        else
            return url.prettyUrl();
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
        return KGlobal::config()->hasGroup( url().prettyUrl() );
    }

    QString
    TheStream::metaData(Phonon::MetaData key)
    {
        QStringList values = videoWindow()->m_media->metaData(key);
        kDebug() << values;
        return (values.isEmpty()) ? QString() : values.join(QString(QLatin1Char( ' ' )));
    }


/*
    static inline QString
    entryHelper( const QString &plate, const QString &s1, const QString &s2 )
    {
        return s2.isEmpty() ? s2 : plate.arg( s1 ).arg( s2 );
    }

    static inline QString
    sectionHelper( const QString &sectionTitle, const QStringList &entries )
    {
	QString s;

        foreach( const QString& str, entries )
            if( !str.isEmpty() )
                s += str;

        return s.isEmpty() ? s : "<h2>" + sectionTitle + "</h2>" + s;
    }

    QString
    TheStream::information()
    {
        return QString();
//         #define meta( x ) xine_get_meta_info( VideoWindow::s_instance->m_stream, x )
//         #define info( x, y ) x.arg( xine_get_stream_info( VideoWindow::s_instance->m_stream, y ) )
//         #define simple( x ) QString::number( xine_get_stream_info( VideoWindow::s_instance->m_stream, x ) )
//
//         const QString plate = "<p><b>%1</b>: %2</p>";
//         QString s;
//
//         s += sectionHelper( i18n("Metadata"),
//             QStringList()
//                 << entryHelper( plate, i18n("Title"), meta( XINE_META_INFO_TITLE ) )
//                 << entryHelper( plate, i18n("Comment"), meta( XINE_META_INFO_COMMENT ) )
//                 << entryHelper( plate, i18n("Artist"), meta( XINE_META_INFO_ARTIST ) )
//                 << entryHelper( plate, i18n("Genre"), meta( XINE_META_INFO_GENRE ) )
//                 << entryHelper( plate, i18n("Album"), meta( XINE_META_INFO_ALBUM ) )
//                 << entryHelper( plate, i18n("Year"), meta( XINE_META_INFO_YEAR ) ) );
//
//         s += sectionHelper( i18n("Audio Properties"),
//             QStringList()
//                 << entryHelper( plate, i18n("Bitrate"), info( i18n("%1 bps"), XINE_STREAM_INFO_AUDIO_BITRATE ) )
//                 << entryHelper( plate, i18n("Sample-rate"), info( i18n("%1 Hz"),  XINE_STREAM_INFO_AUDIO_SAMPLERATE ) ) );
//
//         s += sectionHelper( i18n("Technical Information"),
//             QStringList()
//                 << entryHelper( plate, i18n("Video Codec"), meta( XINE_META_INFO_VIDEOCODEC ) )
//                 << entryHelper( plate, i18n("Audio Codec"), meta( XINE_META_INFO_AUDIOCODEC ) )
//                 << entryHelper( plate, i18n("System Layer"), meta( XINE_META_INFO_SYSTEMLAYER ) )
//                 << entryHelper( plate, i18n("Input Plugin"), meta( XINE_META_INFO_INPUT_PLUGIN  ))
//                 << entryHelper( plate, i18n("CDINDEX_DISCID"), meta( XINE_META_INFO_CDINDEX_DISCID ) ) );
//
//         QStringList texts;
//         texts << "BITRATE" << "SEEKABLE" << "VIDEO_WIDTH" << "VIDEO_HEIGHT" << "VIDEO_RATIO" << "VIDEO_CHANNELS" << "VIDEO_STREAMS" << "VIDEO_BITRATE" << "VIDEO_FOURCC" << "VIDEO_HANDLED" << "FRAME_DURATION" << "AUDIO_CHANNELS" << "AUDIO_BITS" << "-AUDIO_SAMPLERATE" << "-AUDIO_BITRATE" << "AUDIO_FOURCC" << "AUDIO_HANDLED" << "HAS_CHAPTERS" << "HAS_VIDEO" << "HAS_AUDIO" << "-IGNORE_VIDEO" << "-IGNORE_AUDIO" << "-IGNORE_SPU" << "VIDEO_HAS_STILL" << "MAX_AUDIO_CHANNEL" << "MAX_SPU_CHANNEL" << "AUDIO_MODE" << "SKIPPED_FRAMES" << "DISCARDED_FRAMES";
//
//         s += "<h2>Other</h2>";
//         for( uint x = 0; x <= 28; ++x )
//             s += entryHelper( plate, texts[x], simple( x ) );
//
//         #undef meta
//         #undef info
//         #undef simple
//
//         return s;
    }
*/
}

