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

#ifndef DRAGONPLAYER_PLAYLIST_FILE_H
#define DRAGONPLAYER_PLAYLIST_FILE_H

#include <KUrl>
#include <QTextStream>

class PlaylistFile
{
public:
   PlaylistFile( const KUrl &url );
  ~PlaylistFile();

   enum FileFormat { M3U, PLS, Unknown, NotPlaylistFile = Unknown };

   bool isPlaylist() const { return m_type != Unknown; }
   bool isValid() const { return m_isValid; }
   KUrl firstUrl() const { return m_contents.isEmpty() ? KUrl() : m_contents.first(); }
   QString error() const { return m_error; }

private:
   /// both only return first url currently
   void parsePlsFile( QTextStream& );
   void parseM3uFile( QTextStream& );

   KUrl m_url;
   bool m_isRemoteFile;
   bool m_isValid;
   QString m_error;
   FileFormat m_type;
   QString m_path;
   KUrl::List m_contents;
};

#endif
