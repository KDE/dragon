/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYER_THESTREAM_H
#define DRAGONPLAYER_THESTREAM_H

#include <KConfigGroup>
#include <QUrl>    // larger :( but no macros at least
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
        static QUrl url();

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
    static QString discId();

    static bool hasProfile();

    static KConfigGroup profile();
private:
    static QHash<int, QAction*> s_aspectRatioActions;
};
}

#endif
