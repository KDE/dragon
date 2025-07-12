// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2025 Harald Sitter <sitter@kde.org>

#include "backend.h"

#include <QtMultimedia/QVideoSink>
#include <QtMultimedia/private/qplatformvideosink_p.h>

using namespace Qt::StringLiterals;

BackendAttachedType::BackendAttachedType(QVideoSink *videoSink, QObject *parent)
    : QObject(parent)
    , m_videoSink(videoSink)
{
}

bool BackendAttachedType::isFFmpeg() const
{
    if (!m_videoSink) {
        qWarning() << "No videoSink available";
        return false;
    }
    if (!m_videoSink->platformVideoSink()) {
        qWarning() << "No platform video sink available";
        return false;
    }
    auto platform = m_videoSink->platformVideoSink();
    constexpr auto ffmpegName = "QFFmpegVideoSink"_L1;
    if (auto className = QLatin1StringView(platform->metaObject()->className()); className != ffmpegName) {
        qWarning() << "Expected" << ffmpegName << ", got" << className;
        return false;
    }

    return true;
}

BackendAttachedType *Backend::qmlAttachedProperties(QObject *object)
{
    auto videoSink = qobject_cast<QVideoSink *>(object);
    Q_ASSERT(videoSink);
    if (!videoSink) {
        qWarning() << "BackendAttachedType can only be attached to QVideoSink objects, got" << object->metaObject()->className();
        // BackendAttachedType will handle this properly, no need to return without attaching
    }
    return new BackendAttachedType(videoSink, object);
}
