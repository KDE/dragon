// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2025 Harald Sitter <sitter@kde.org>

#pragma once

#include <QtQmlIntegration>

class QVideoSink;

class BackendAttachedType : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS
    Q_PROPERTY(bool ffmpeg READ isFFmpeg CONSTANT)
public:
    explicit BackendAttachedType(QVideoSink *videoSink, QObject *parent = nullptr);

    [[nodiscard]] bool isFFmpeg() const;

private:
    QVideoSink *m_videoSink;
};

class Backend : public QObject
{
    Q_OBJECT
    QML_ATTACHED(BackendAttachedType)
    QML_ELEMENT
public:
    using QObject::QObject;

    static BackendAttachedType *qmlAttachedProperties(QObject *object);
};
