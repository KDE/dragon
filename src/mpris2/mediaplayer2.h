/*
    SPDX-FileCopyrightText: 2012 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYER_MEDIAPLAYER2_H
#define DRAGONPLAYER_MEDIAPLAYER2_H

#include <QAction>
#include <QDBusAbstractAdaptor>
#include <QQmlParserStatus>
#include <QStringList> // Needed for automoc'ed cpp to compile
#include <QWindow>
#include <QtQmlIntegration>

struct ActionSet {
    QAction *fullscreen;
    QAction *quit;
    QAction *raise;
};

class MediaPlayer2 : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2") // Docs: https://specifications.freedesktop.org/mpris-spec/latest/Media_Player.html

    Q_PROPERTY(bool CanQuit READ CanQuit)
    Q_PROPERTY(bool CanRaise READ CanRaise)

    Q_PROPERTY(bool Fullscreen READ Fullscreen WRITE setFullscreen)
    Q_PROPERTY(bool CanSetFullscreen READ CanSetFullscreen)

    Q_PROPERTY(bool HasTrackList READ HasTrackList)

    Q_PROPERTY(QString Identity READ Identity)
    Q_PROPERTY(QString DesktopEntry READ DesktopEntry)

    Q_PROPERTY(QStringList SupportedUriSchemes READ SupportedUriSchemes)
    Q_PROPERTY(QStringList SupportedMimeTypes READ SupportedMimeTypes)

public:
    explicit MediaPlayer2(ActionSet actionSet, QObject *parent);
    ~MediaPlayer2() override;

    bool CanQuit() const;
    bool CanRaise() const;

    bool Fullscreen() const;
    void setFullscreen(bool fullscreen) const;
    bool CanSetFullscreen() const;

    bool HasTrackList() const;

    QString Identity() const;
    QString DesktopEntry() const;

    QStringList SupportedUriSchemes() const;
    QStringList SupportedMimeTypes() const;

public Q_SLOTS:
    void Raise() const;
    void Quit() const;

private Q_SLOTS:
    void emitFullscreenChange(bool fullscreen) const;

private:
    ActionSet m_actionSet;
};

#endif
