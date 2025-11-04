// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2025 Harald Sitter <sitter@kde.org>

#include "fileopen.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QGuiApplication>
#include <QWindow>

#include <KLocalizedString>

using namespace Qt::StringLiterals;

void FileOpen::open(QWindow *window)
{
    if (QGuiApplication::platformName() == "xcb"_L1) {
        constexpr auto hex = 16;
        openInternal(u"x11:"_s.append(QString::number(window->winId(), hex)));
        return;
    }

    auto wayland = KWaylandExtras::self();
    connect(
        wayland,
        &KWaylandExtras::windowExported,
        this,
        [this, window](QWindow *exportedWindow, const QString &handle) {
            if (exportedWindow != window) {
                // not our event
                return;
            }
            openInternal(u"wayland:"_s.append(handle));
        },
        Qt::SingleShotConnection);
    wayland->exportWindow(window);
}

void FileOpen::gotResponse(uint response, const QVariantMap &results)
{
    if (response != 0) {
        qWarning() << "Failed to open portal dialog:" << response;
        Q_EMIT rejected();
        return;
    }

    QStringList uris = results.value("uris"_L1).toStringList();

    if (uris.count() > 0) {
        m_selectedUrl = QUrl(uris.at(0));
        // Per documentation the uris are always file://
        auto dir = QFileInfo(m_selectedUrl.path()).path();
        if (!dir.isEmpty()) {
            m_currentFolder = dir;
            Q_EMIT currentFolderChanged();
        }
    } else {
        qWarning() << "Failed to open portal dialog: no uris";
    }

    Q_EMIT accepted();
}

void FileOpen::openInternal(const QString &windowHandle)
{
    QDBusMessage message = QDBusMessage::createMethodCall(u"org.freedesktop.portal.Desktop"_s,
                                                          u"/org/freedesktop/portal/desktop"_s,
                                                          u"org.freedesktop.portal.FileChooser"_s,
                                                          u"OpenFile"_s);
    QVariantMap options;

    options.insert("modal"_L1, true);
    options.insert("multiple"_L1, false);
    options.insert("directory"_L1, false);
    options.insert("handle_token"_L1, QStringLiteral("dragon%1").arg(QRandomGenerator::global()->generate()));
    if (!m_currentFolder.isEmpty()) {
        options.insert("current_folder"_L1, QFile::encodeName(m_currentFolder).append('\0'));
    }

    message << windowHandle;
    message << /* title = */ i18nc("@title", "Open Video File or Network Stream") << options;

    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message);
    auto watcher = new QDBusPendingCallWatcher(pendingCall);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
        watcher->deleteLater();
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;

        if (reply.isError()) {
            qWarning() << "Failed to open portal dialog:" << reply.error().message();
            Q_EMIT rejected();
            return;
        }

        QDBusConnection::sessionBus()
            .connect(QString(), reply.value().path(), "org.freedesktop.portal.Request"_L1, "Response"_L1, this, SLOT(gotResponse(uint, QVariantMap)));
    });
}
