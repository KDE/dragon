/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

//TODO error messages that vary depending on if the file is remote or not

#include "playlistFile.h"
#include "codeine.h"

#include <KIO/StoredTransferJob>
#include <KLocalizedString>
#include <KJobWidgets>

#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QGraphicsItem>
#include <QEvent>

PlaylistFile::PlaylistFile(const QUrl &url )
    : m_url( url )
    , m_isValid( false )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );

    const QString filename = m_url.fileName();

    if( filename.endsWith( QLatin1String(".pls"), Qt::CaseInsensitive ) )
        m_type = PLS;
    else if( filename.endsWith( QLatin1String(".m3u"), Qt::CaseInsensitive ) )
        m_type = M3U;
    else {
        m_type = Unknown;
        m_error = i18n( "The file is not a playlist" );
        QApplication::restoreOverrideCursor();
        return;
    }

    KIO::StoredTransferJob * job = KIO::storedGet(url, KIO::NoReload, KIO::HideProgressInfo|KIO::Overwrite);
    KJobWidgets::setWindow(job, Dragon::mainWindow());
    if (!job->exec()) {
        m_error = i18n( "Dragon Player could not download the remote playlist: %1", url.toDisplayString() );
        QApplication::restoreOverrideCursor();
        job->deleteLater();
        return;
    }

    QByteArray data = job->data();

    if (!data.isEmpty()) {
        QTextStream stream( &data );
        switch( m_type ) {
        case M3U: parseM3uFile( stream ); break;
        case PLS: parsePlsFile( stream ); break;
        default: ;
        }

        if( m_contents.isEmpty() ) {
            m_error = i18n( "<qt>The playlist, <i>'%1'</i>, could not be interpreted. Perhaps it is empty?</qt>", filename);
            m_isValid = false;
        }
    } else
        m_error = i18n( "Dragon Player could not open the file: %1", filename );

    QApplication::restoreOverrideCursor();
}


PlaylistFile::~PlaylistFile()
{
}

void PlaylistFile::parsePlsFile( QTextStream &stream )
{
    QString line;
    while (!stream.atEnd()) {
        line = stream.readLine();
        if( line.startsWith( QLatin1String("File") ) ) {
            const QString tmp = line.section( QLatin1Char( '=' ), -1 );
            addToPlaylist(tmp);
        }
    }
    m_isValid = !m_contents.isEmpty();
}


void PlaylistFile::parseM3uFile( QTextStream &stream )
{
    QString line;
    while (!stream.atEnd()) {
        line = stream.readLine();

        if( line.startsWith( QLatin1String("#EXTINF"), Qt::CaseInsensitive ) ) {
            continue;
        } else if( !line.startsWith( QLatin1Char( '#' ) ) && !line.isEmpty() ) {
            addToPlaylist(line);
        }
    }
    m_isValid = !m_contents.isEmpty();
}

void PlaylistFile::addToPlaylist(const QString &line) {
    QUrl url;

    if( line.startsWith( QLatin1Char( '/' ) ) ) { // absolute local file
        url = QUrl::fromLocalFile(line);
    } else { // relative file or other protocol
        const QUrl tmp = QUrl(line);
        if (tmp.scheme().isEmpty()) {
            url = QUrl::fromLocalFile(m_url.adjusted(QUrl::RemoveFilename).path() + QLatin1Char( '/' ) + line);
        } else {
            url = tmp;
        }
    }
    m_contents += url;
}
