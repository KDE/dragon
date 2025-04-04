// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2025 Harald Sitter <sitter@kde.org>

#pragma once

#include <QtQmlIntegration>

#include <KWaylandExtras>

class QWindow;

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
    QString m_currentFolder;
};
