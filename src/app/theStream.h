// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINE_THESTREAM_H
#define CODEINE_THESTREAM_H

#include <KConfigGroup>
#include <KGlobal>
#include <KUrl>    // larger :( but no macros at least
#include <QSize>   // small header
#include <QString> // small header

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
      static QAction* aspectRatioAction();
      static void addRatio( int, QAction* );

      static int subtitleChannel();
      static int audioChannel();

      static QString prettyTitle();
      static QString information();

      static inline bool hasProfile()
            { return KGlobal::config()->hasGroup( url().prettyUrl() ); }

      static    KConfigGroup profile();
    private:
      static QHash<int, QAction*> s_aspectRatioActions;
   };
}

#endif
