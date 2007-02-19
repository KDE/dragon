// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINE_PLAYLIST_FILE_H
#define CODEINE_PLAYLIST_FILE_H

#include <kurl.h>

class PlaylistFile
{
public:
   PlaylistFile( const KURL &url );
  ~PlaylistFile();

   enum FileFormat { M3U, PLS, Unknown, NotPlaylistFile = Unknown };

   bool isPlaylist() const { return m_type != Unknown; }
   bool isValid() const { return m_isValid; }
   KURL firstUrl() const { return m_contents.isEmpty() ? KURL() : m_contents.first(); }
   QString error() const { return m_error; }

private:
   /// both only return first url currently
   void parsePlsFile( Q3TextStream& );
   void parseM3uFile( Q3TextStream& );

   KURL m_url;
   bool m_isRemoteFile;
   bool m_isValid;
   QString m_error;
   FileFormat m_type;
   QString m_path;
   KURL::List m_contents;
};

#endif
