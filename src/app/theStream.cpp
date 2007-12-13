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

#include <QHash>

#include <KUrl>
#include <KLocale>
#include <Phonon/MediaObject>
#include <Phonon/MediaSource>
#include <Phonon/VideoWidget>
#include <Solid/Device>
#include <Solid/StorageVolume>

#include <xine.h>

#include "debug.h"
#include "mxcl.library.h"
#include "theStream.h"
#include "videoWindow.h"


namespace Codeine
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
                    QString discLabel = "disc:";
                    QString uuid = disc->uuid();
                    QString label = disc->label();
                    if( !uuid.isEmpty() )
                        discLabel += uuid;
                    else if ( !label.isEmpty() )
                        discLabel += label;
                    else
                        discLabel = QString::null;
                    debug() << "Disc has UUID " << uuid << " and label " << disc->label() << " so writing " << discLabel;
                    return KConfigGroup( KGlobal::config(), QString("disc:") + uuid );
                }
                else
                    debug() << "profile: doesn't convert into Solid::StorageVolume";
            }
            else
                debug() << "profile: empty device list";
        }
        else
            debug() << "profile: Not a Phonon::MediaSource::Disc";
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
        if( engine()->m_xineStream )
            return xine_get_param( engine()->m_xineStream, XINE_PARAM_SPU_CHANNEL );
        else
            return -1;
    }

    int
    TheStream::audioChannel()
        { return 0; }

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
        const QString artist = QString();
        const QString title  = QString();

        if (hasVideo() && !title.isEmpty())
            return title;
        else if (!title.isEmpty() && !artist.isEmpty())
            return artist + " - " + title;
        else if (url.protocol() != "http" && !url.fileName().isEmpty()) 
        {
            const QString n = url.fileName();
            //toLatin1 sense fromPercentEncoding takes a QByteArray
            //I'm not sure about this whole method though, should double check that titles make sense
            return QUrl::fromPercentEncoding( n.left( n.lastIndexOf( '.' ) ).replace( '_', ' ' ).toLatin1() ); 
        }
        else
            return url.prettyUrl();
    }


    static inline QString
    entryHelper( const QString &plate, const QString &s1, const QString &s2 )
    {
        return s2.isEmpty() ? s2 : plate.arg( s1 ).arg( s2 );
    }

    static inline QString
    sectionHelper( const QString &sectionTitle, const QStringList &entries )
    {
	QString s;

        foreach( QString str, entries )
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
}
