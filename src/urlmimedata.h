/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef URLMIMEDATA_H
#define URLMIMEDATA_H

#include <QObject>
#include <QPointer>
#include <QQmlEngine>
#include <QUrl>

// TODO Put this in some Framework...
/*!
 * \qmltype UrlMimeData
 *
 * \brief Utility for using URLs in drag and drop
 *
 * \sa KUrlMimeData
 */
class UrlMimeData : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QObject *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QList<QUrl> urls READ urls NOTIFY urlsChanged)

public:
    explicit UrlMimeData(QObject *parent = nullptr);

    QObject *target() const;
    void setTarget(QObject *target);
    Q_SIGNAL void targetChanged(QObject *target);

    QList<QUrl> urls() const;
    Q_SIGNAL void urlsChanged(const QList<QUrl> &urls);

    bool enabled() const;
    void setEnabled(bool enabled);
    Q_SIGNAL void enabledChanged(bool enabled);

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QPointer<QObject> m_target;
    QList<QUrl> m_urls;
};

#endif // URLMIMEDATA_H
