/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

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
class UrlMimeData : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    QML_ELEMENT

    /*!
     * \qmlproperty QtObject target
     *
     * The target to monitor, default is the parent item
     */
    Q_PROPERTY(QObject *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QList<QUrl> urls READ urls NOTIFY urlsChanged)

public:
    explicit UrlMimeData(QObject *parent = nullptr);

    QObject *target() const;
    void setTarget(QObject *target);
    Q_SIGNAL void targetChanged(QObject *target);

    QList<QUrl> urls() const;
    Q_SIGNAL void urlsChanged(const QList<QUrl> &urls);

    void classBegin() override;
    void componentComplete() override;

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QPointer<QObject> m_target;
    QList<QUrl> m_urls;
};
