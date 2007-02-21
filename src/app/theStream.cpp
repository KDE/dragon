// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include <kurl.h>
#include <klocale.h>

#include "mxcl.library.h"
#include "theStream.h"
#include <xine.h>
#include "xineEngine.h"

namespace Codeine
{
    KConfigGroup
    TheStream::profile()
    {
//TODO a unique id for discs, and then even to also record chapters etc.
//         if( url().protocol() == "dvd" )
//             return Codeine::config( QString( "dvd:/" ) + prettyTitle() );
//         else
            return Codeine::config( url().prettyUrl() );
    }

    const KUrl&
    TheStream::url()
            { return VideoWindow::s_instance->m_url; }

    bool
    TheStream::canSeek()
            //FIXME!
            { return VideoWindow::s_instance->m_url.protocol() != "http"; }

    bool
    TheStream::hasAudio()
            { return xine_get_stream_info( VideoWindow::s_instance->m_stream, XINE_STREAM_INFO_HAS_AUDIO ); }

    bool
    TheStream::hasVideo()
            { return xine_get_stream_info( VideoWindow::s_instance->m_stream, XINE_STREAM_INFO_HAS_VIDEO ); }

    QSize
    TheStream::defaultVideoSize()
    {
        return !VideoWindow::s_instance->m_stream
                ? QSize()
                : QSize(
                        xine_get_stream_info( VideoWindow::s_instance->m_stream, XINE_STREAM_INFO_VIDEO_WIDTH ),
                        xine_get_stream_info( VideoWindow::s_instance->m_stream, XINE_STREAM_INFO_VIDEO_HEIGHT ) );
    }

    int TheStream::aspectRatio()
            { return xine_get_param( VideoWindow::s_instance->m_stream, XINE_PARAM_VO_ASPECT_RATIO ); }

    int TheStream::subtitleChannel()
            { return xine_get_param( VideoWindow::s_instance->m_stream, XINE_PARAM_SPU_CHANNEL ); }

    int TheStream::audioChannel()
            { return xine_get_param( VideoWindow::s_instance->m_stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL ); }

    QString
    TheStream::prettyTitle()
    {
        const KUrl &url        = VideoWindow::s_instance->m_url;
        const QString artist = QString::fromUtf8( xine_get_meta_info( VideoWindow::s_instance->m_stream, XINE_META_INFO_ARTIST ) );
        const QString title  = QString::fromUtf8( xine_get_meta_info( VideoWindow::s_instance->m_stream, XINE_META_INFO_TITLE ) );

        if (hasVideo() && !title.isEmpty())
            return title;
        else if (!title.isEmpty() && !artist.isEmpty())
            return artist + " - " + title;
        else if (url.protocol() != "http" && !url.fileName().isEmpty()) {
            const QString n = url.fileName();
            return KUrl::decode_string( n.left( n.findRev( '.' ) ).replace( '_', ' ' ) ); }
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
        #define meta( x ) xine_get_meta_info( VideoWindow::s_instance->m_stream, x )
        #define info( x, y ) x.arg( xine_get_stream_info( VideoWindow::s_instance->m_stream, y ) )
        #define simple( x ) QString::number( xine_get_stream_info( VideoWindow::s_instance->m_stream, x ) )

        const QString plate = "<p><b>%1</b>: %2</p>";
        QString s;

        s += sectionHelper( i18n("Metadata"),
            QStringList()
                << entryHelper( plate, i18n("Title"), meta( XINE_META_INFO_TITLE ) )
                << entryHelper( plate, i18n("Comment"), meta( XINE_META_INFO_COMMENT ) )
                << entryHelper( plate, i18n("Artist"), meta( XINE_META_INFO_ARTIST ) )
                << entryHelper( plate, i18n("Genre"), meta( XINE_META_INFO_GENRE ) )
                << entryHelper( plate, i18n("Album"), meta( XINE_META_INFO_ALBUM ) )
                << entryHelper( plate, i18n("Year"), meta( XINE_META_INFO_YEAR ) ) );

        s += sectionHelper( i18n("Audio Properties"),
            QStringList()
                << entryHelper( plate, i18n("Bitrate"), info( i18n("%1 bps"), XINE_STREAM_INFO_AUDIO_BITRATE ) )
                << entryHelper( plate, i18n("Sample-rate"), info( i18n("%1 Hz"),  XINE_STREAM_INFO_AUDIO_SAMPLERATE ) ) );

        s += sectionHelper( i18n("Technical Information"),
            QStringList()
                << entryHelper( plate, i18n("Video Codec"), meta( XINE_META_INFO_VIDEOCODEC ) )
                << entryHelper( plate, i18n("Audio Codec"), meta( XINE_META_INFO_AUDIOCODEC ) )
                << entryHelper( plate, i18n("System Layer"), meta( XINE_META_INFO_SYSTEMLAYER ) )
                << entryHelper( plate, i18n("Input Plugin"), meta( XINE_META_INFO_INPUT_PLUGIN  ))
                << entryHelper( plate, i18n("CDINDEX_DISCID"), meta( XINE_META_INFO_CDINDEX_DISCID ) ) );

        QStringList texts;
        texts << "BITRATE" << "SEEKABLE" << "VIDEO_WIDTH" << "VIDEO_HEIGHT" << "VIDEO_RATIO" << "VIDEO_CHANNELS" << "VIDEO_STREAMS" << "VIDEO_BITRATE" << "VIDEO_FOURCC" << "VIDEO_HANDLED" << "FRAME_DURATION" << "AUDIO_CHANNELS" << "AUDIO_BITS" << "-AUDIO_SAMPLERATE" << "-AUDIO_BITRATE" << "AUDIO_FOURCC" << "AUDIO_HANDLED" << "HAS_CHAPTERS" << "HAS_VIDEO" << "HAS_AUDIO" << "-IGNORE_VIDEO" << "-IGNORE_AUDIO" << "-IGNORE_SPU" << "VIDEO_HAS_STILL" << "MAX_AUDIO_CHANNEL" << "MAX_SPU_CHANNEL" << "AUDIO_MODE" << "SKIPPED_FRAMES" << "DISCARDED_FRAMES";

        s += "<h2>Other</h2>";
        for( uint x = 0; x <= 28; ++x )
            s += entryHelper( plate, texts[x], simple( x ) );

        #undef meta
        #undef info
        #undef simple

        return s;
    }
}
