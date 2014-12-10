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


//TODO error messages that vary depending on if the file is remote or not

#include "playlistFile.h"
#include "codeine.h"

#include <KIO/StoredTransferJob>
#include <KLocalizedString>
#include <KJobWidgets>

#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QtWidgets/qgraphicsitem.h>
#include <QtGui/qevent.h>

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
