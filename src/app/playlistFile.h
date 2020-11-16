/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYER_PLAYLIST_FILE_H
#define DRAGONPLAYER_PLAYLIST_FILE_H

#include <QUrl>
#include <QTextStream>
#include <QList>

class PlaylistFile
{
public:
    explicit PlaylistFile( const QUrl &url );
    ~PlaylistFile();

    enum FileFormat { M3U, PLS, Unknown, NotPlaylistFile = Unknown };

    bool isPlaylist() const { return m_type != Unknown; }
    bool isValid() const { return m_isValid; }
    QUrl firstUrl() const { return m_contents.isEmpty() ? QUrl() : m_contents.first(); }
    QList<QUrl> contents() const { return m_contents; }
    QString error() const { return m_error; }

private:
    void parsePlsFile( QTextStream& );
    void parseM3uFile( QTextStream& );

    void addToPlaylist(const QString &line);

    QUrl m_url;
    bool m_isValid;
    QString m_error;
    FileFormat m_type;
    QList<QUrl> m_contents;
};

#endif
