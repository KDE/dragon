// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2025 Harald Sitter <sitter@kde.org>

#pragma once

#include <QtQmlIntegration>

#include <KWaylandExtras>

class Sandbox : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(bool inside READ isInside CONSTANT)
    Q_PROPERTY(bool ffmpegFull READ hasFfmpegFull CONSTANT)
public:
    using QObject::QObject;

    [[nodiscard]] bool isInside() const;
    [[nodiscard]] bool hasFfmpegFull() const;
};
