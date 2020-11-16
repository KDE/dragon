/*
    SPDX-FileCopyrightText: 2012 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "mpris2.h"
#include "mediaplayer2.h"
#include "mediaplayer2player.h"
#include "codeine.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QMetaClassInfo>
#include <QStringList>

#include <unistd.h>

Mpris2::Mpris2(QObject* parent) : QObject(parent)
{
    QString mpris2Name("org.mpris.MediaPlayer2." + QLatin1String(APP_NAME));

    bool success = QDBusConnection::sessionBus().registerService(mpris2Name);

    // If the above failed, it's likely because we're not the first instance
    // and the name is already taken. In that event the MPRIS2 spec wants the
    // following:
    if (!success)
        success = QDBusConnection::sessionBus().registerService(mpris2Name + ".instance" + QString::number(getpid()));

    if (success)  {
        new MediaPlayer2(this);
        new MediaPlayer2Player(this);
        QDBusConnection::sessionBus().registerObject("/org/mpris/MediaPlayer2", this, QDBusConnection::ExportAdaptors);
    }
}

Mpris2::~Mpris2()
{
}

void Mpris2::signalPropertiesChange(const QObject* adaptor, const QVariantMap& properties)
{
    QDBusMessage msg = QDBusMessage::createSignal("/org/mpris/MediaPlayer2",
                                                  "org.freedesktop.DBus.Properties", "PropertiesChanged" );

    msg << adaptor->metaObject()->classInfo(0).value();
    msg << properties;
    msg << QStringList();

    QDBusConnection::sessionBus().send(msg);
}
