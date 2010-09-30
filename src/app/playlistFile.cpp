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
#include <KDebug>
#include <KIO/NetAccess>
#include <KLocale>

#include <QApplication>
#include <QFile>

PlaylistFile::PlaylistFile( const KUrl &url )
        : m_url( url )
        , m_isRemoteFile( !url.isLocalFile() )
        , m_isValid( false )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );

    QString &path = m_path = url.path();

    if( path.endsWith( QLatin1String(".pls"), Qt::CaseInsensitive ) )
        m_type = PLS; else
    if( path.endsWith( QLatin1String(".m3u"), Qt::CaseInsensitive ) )
        m_type = M3U;
    else {
        m_type = Unknown;
        m_error = i18n( "The file is not a playlist" );
        QApplication::restoreOverrideCursor();
        return;
    }

    if( m_isRemoteFile ) {
        path.clear();
        if( !KIO::NetAccess::download( url, path, Dragon::mainWindow() ) ) {
            m_error = i18n( "Dragon Player could not download the remote playlist: %1", url.prettyUrl() );
            QApplication::restoreOverrideCursor();
            return;
        }
    }

    QFile file( path );
    if( file.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &file );
        switch( m_type ) {
            case M3U: parseM3uFile( stream ); break;
            case PLS: parsePlsFile( stream ); break;
            default: ;
        }

        if( m_contents.isEmpty() )
            m_error = i18n( "<qt>The playlist, <i>'%1'</i>, could not be interpreted. Perhaps it is empty?</qt>", path ),
            m_isValid = false;
    }
    else
        m_error = i18n( "Dragon Player could not open the file: %1", path );

    QApplication::restoreOverrideCursor();
}


PlaylistFile::~PlaylistFile()
{
    if( m_isRemoteFile )
        KIO::NetAccess::removeTempFile( m_path );
}


void
PlaylistFile::parsePlsFile( QTextStream &stream )
{

    for( QString line = stream.readLine(); !line.isNull(); )
    {
        if( line.startsWith( QLatin1String("File") ) ) {
            const KUrl url = line.section( QLatin1Char(  '=' ), -1 );
            const QString title = stream.readLine().section( QLatin1Char(  '=' ), -1 );

            kDebug() << url << endl << title;

            m_contents += url;
            m_isValid = true;

            return; //TODO continue for all urls
        }
        line = stream.readLine();
    }
}


void
PlaylistFile::parseM3uFile( QTextStream &stream )
{

    for( QString line; !stream.atEnd(); )
    {
        line = stream.readLine();

        if( line.startsWith( QLatin1String("#EXTINF"), Qt::CaseInsensitive ) )
            continue;

        else if( !line.startsWith( QLatin1Char( '#' ) ) && !line.isEmpty() )
        {
            KUrl url;

            // KUrl::isRelativeUrl() expects absolute URLs to start with a protocol, so prepend it if missing
            if( line.startsWith( QLatin1Char( '/' ) ) )
                line.prepend( QLatin1String( "file://" ) );

            if( KUrl::isRelativeUrl( line ) )
                url.setPath( m_url.directory() + QLatin1Char( '/' ) + line );
            else
                url = KUrl( line );

            m_contents += url;
            m_isValid = true;

            return;
        }
    }
}
