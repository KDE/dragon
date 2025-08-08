// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2025 Harald Sitter <sitter@kde.org>

#pragma once

#include <QtQmlIntegration>

#include <KWaylandExtras>

class QWindow;

/*!
    This class is used to retrieve a file open url.
    The url is always either local or http/https (as per our .desktop file).
    It uses the portal directly to avoid having to wire up KIO to QtMultimedia.
    Instead we rely on the portal stack to turn remote urls the user may choose
    into local file urls we can consume (i.e. to employ kio-fuse for remote files).
    This allows us to not have divergent code paths between sandbox and !sandbox
    uses, and ensures we transparently can access everything KIO can access without
    having to talk to KIO directly.
*/
class FileOpen : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString currentFolder MEMBER m_currentFolder NOTIFY currentFolderChanged)
    Q_PROPERTY(QUrl selectedUrl MEMBER m_selectedUrl NOTIFY accepted)
public:
    using QObject::QObject;

public Q_SLOTS:
    void open(QWindow *window);

private Q_SLOTS:
    void gotResponse(uint response, const QVariantMap &results);

Q_SIGNALS:
    void accepted();
    void rejected();
    void currentFolderChanged();

private:
    void openInternal(const QString &windowHandle);
    QUrl m_selectedUrl;
    QString m_currentFolder = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).at(0);
};
