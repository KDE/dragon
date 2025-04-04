/*
    SPDX-FileCopyrightText: 2012 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "mpris2.h"
#include "mediaplayer2.h"
#include "mediaplayer2player.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QMetaClassInfo>
#include <QStringList>

#include <unistd.h>

using namespace Qt::StringLiterals;

Mpris2::Mpris2(QObject *parent)
    : QObject(parent)
    , m_quitAction([] {
        auto action = std::make_unique<QAction>();
        action->setObjectName(u"quitAction"_s);
        return action;
    }())
    , m_fullscreenAction([] {
        auto action = std::make_unique<QAction>();
        action->setObjectName(u"fullscreenAction"_s);
        return action;
    }())
    , m_raiseAction([] {
        auto action = std::make_unique<QAction>();
        action->setObjectName(u"raiseAction"_s);
        return action;
    }())
{
}

Mpris2::~Mpris2() = default;

void Mpris2::signalPropertiesChange(const QObject *adaptor, const QVariantMap &properties)
{
    QDBusMessage msg = QDBusMessage::createSignal(QStringLiteral("/org/mpris/MediaPlayer2"),
                                                  QStringLiteral("org.freedesktop.DBus.Properties"),
                                                  QStringLiteral("PropertiesChanged"));

    msg << QString::fromUtf8(adaptor->metaObject()->classInfo(0).value());
    msg << properties;
    msg << QStringList();

    QDBusConnection::sessionBus().send(msg);
}

void Mpris2::componentComplete()
{
    const QString mpris2Name = QStringLiteral("org.mpris.MediaPlayer2.dragonplayer");

    bool success = QDBusConnection::sessionBus().registerService(mpris2Name);

    // If the above failed, it's likely because we're not the first instance
    // and the name is already taken. In that event the MPRIS2 spec wants the
    // following:
    if (!success)
        success = QDBusConnection::sessionBus().registerService(mpris2Name + QLatin1String(".instance") + QString::number(getpid()));

    if (success) {
        new MediaPlayer2({.fullscreen = m_fullscreenAction.get(), .quit = m_quitAction.get(), .raise = m_raiseAction.get()}, this);
        new MediaPlayer2Player(m_player, this);
        QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/mpris/MediaPlayer2"), this, QDBusConnection::ExportAdaptors);
    } else {
        qWarning() << "Failed to register MPRIS2 service:" << QDBusConnection::sessionBus().lastError().message();
    }
}

void Mpris2::classBegin()
{
}

#include "moc_mpris2.cpp"
