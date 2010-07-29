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

#ifndef DRAGONPLAYER_THESTREAM_H
#define DRAGONPLAYER_THESTREAM_H

#include <KConfigGroup>
#include <KUrl>    // larger :( but no macros at least
#include <QSize>   // small header
#include <QString> // small header
#include <Phonon/Global>

/// for purely static classes
#define DRAGONPLAYER_NO_EXPORT( T ) \
   T(); \
  ~T(); \
   T( const T& ); \
   T &operator=( const T& ); \
   bool operator==( const T& ); \
   bool operator!=( const T& );

class QAction;

namespace Dragon
{
   class TheStream
   {
   DRAGONPLAYER_NO_EXPORT( TheStream )

   public:
      static KUrl url();

      static bool canSeek();
      static bool hasAudio();
      static bool hasVideo();
      static bool hasMedia();

      static QSize defaultVideoSize();

      static int aspectRatio();
      static QAction* aspectRatioAction();
      static void setRatio( QAction* );
      static void addRatio( int, QAction* );

      static const char* CHANNEL_PROPERTY;
      static int subtitleChannel();
      static int audioChannel();

      static QString prettyTitle();
      static QString fullTitle();


      static QString metaData(Phonon::MetaData key); 
      
      static bool hasProfile();

      static KConfigGroup profile();
    private:
      static QHash<int, QAction*> s_aspectRatioActions;
   };
}

#endif
