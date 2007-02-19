// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINE_THESTREAM_H
#define CODEINE_THESTREAM_H

#include "config.h"  // needed for inline functions
#include <kurl.h>    // larger :( but no macros at least
#include <qsize.h>   // small header
#include <qstring.h> // small header

/// for purely static classes
#define CODEINE_NO_EXPORT( T ) \
   T(); \
  ~T(); \
   T( const T& ); \
   T &operator=( const T& ); \
   bool operator==( const T& ); \
   bool operator!=( const T& );

namespace Codeine
{
   class TheStream
   {
   CODEINE_NO_EXPORT( TheStream )

   public:
      static const KUrl &url();

      static bool canSeek();
      static bool hasAudio();
      static bool hasVideo();

      static QSize defaultVideoSize();

      static int aspectRatio();
      static int subtitleChannel();
      static int audioChannel();

      static QString prettyTitle();
      static QString information();

      static inline bool hasProfile()
            { return KGlobal::config()->hasGroup( url().prettyUrl() ); }

      static KConfig *profile();
   };
}

#endif
