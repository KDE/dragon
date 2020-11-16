/*
    SPDX-FileCopyrightText: 2012 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYER_MPRIS2_H
#define DRAGONPLAYER_MPRIS2_H

#include <QObject>
#include <QVariantMap>

class Mpris2 : public QObject
{
    Q_OBJECT

public:
    explicit Mpris2(QObject* parent);
    ~Mpris2() override;

    static void signalPropertiesChange(const QObject* adaptor, const QVariantMap& properties);
};

#endif
