/*
    SPDX-FileCopyrightText: 2012 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "mediaplayer2.h"
#include "mpris2.h"

#include <KAboutData>
#include <KProtocolInfo>
#include <KService>

using namespace Qt::StringLiterals;

MediaPlayer2::MediaPlayer2(ActionSet actionSet, QObject *parent)
    : QDBusAbstractAdaptor(parent)
    , m_actionSet(actionSet)
{
    connect(m_actionSet.fullscreen, &QAction::toggled, this, &MediaPlayer2::emitFullscreenChange);
}

MediaPlayer2::~MediaPlayer2()
{
}

bool MediaPlayer2::CanQuit() const
{
    return true;
}

void MediaPlayer2::Quit() const
{
    m_actionSet.quit->trigger();
}

bool MediaPlayer2::CanRaise() const
{
    return true;
}

void MediaPlayer2::Raise() const
{
    m_actionSet.raise->trigger();
}

bool MediaPlayer2::Fullscreen() const
{
    return m_actionSet.fullscreen->isChecked();
}

void MediaPlayer2::setFullscreen(bool fullscreen) const
{
    m_actionSet.fullscreen->setChecked(fullscreen);
}

void MediaPlayer2::emitFullscreenChange(bool fullscreen) const
{
    const QVariantMap properties{
        {QStringLiteral("Fullscreen"), fullscreen},
        {QStringLiteral("CanSetFullscreen"), CanSetFullscreen()},
    };
    Mpris2::signalPropertiesChange(this, properties);
}

bool MediaPlayer2::CanSetFullscreen() const
{
    return true;
}

bool MediaPlayer2::HasTrackList() const
{
    return false;
}

QString MediaPlayer2::Identity() const
{
    return KAboutData::applicationData().displayName();
}

QString MediaPlayer2::DesktopEntry() const
{
    return QStringLiteral("org.kde.dragonplayer");
}

QStringList MediaPlayer2::SupportedUriSchemes() const
{
    QStringList protocols;

    const auto allProtocols = KProtocolInfo::protocols();
    for (const QString &protocol : allProtocols) {
        if (!KProtocolInfo::isHelperProtocol(protocol))
            protocols << protocol;
    }

    return protocols;
}

QStringList MediaPlayer2::SupportedMimeTypes() const
{
    KService::Ptr app = KService::serviceByDesktopName(DesktopEntry());

    if (app)
        return app->mimeTypes();

    return QStringList();
}

#include "moc_mediaplayer2.cpp"
