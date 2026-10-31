/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "urlmimedata.h"

#include <KUrlMimeData>

#include <QDropEvent>

UrlMimeData::UrlMimeData(QObject *parent)
    : QObject(parent)
{
}

QObject *UrlMimeData::target() const
{
    return m_target.get();
}

void UrlMimeData::setTarget(QObject *target)
{
    if (m_target == target) {
        return;
    }

    if (m_target) {
        m_target->removeEventFilter(this);
    }

    m_target = target;

    if (m_target) {
        m_target->installEventFilter(this);
    }

    Q_EMIT targetChanged(m_target);
}

QList<QUrl> UrlMimeData::urls() const
{
    return m_urls;
}

bool UrlMimeData::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_target) {
        switch (event->type()) {
        case QEvent::DragEnter:
        // case QEvent::DragMove:
        case QEvent::Drop: {
            auto dropEvent = static_cast<QDropEvent *>(event);
            const auto urls = KUrlMimeData::urlsFromMimeData(dropEvent->mimeData());
            qCritical() << "IEVENT" << urls;
            if (m_urls != urls) {
                m_urls = urls;
                Q_EMIT urlsChanged(urls);
            }
            break;
        }
        default:
            break;
        }
    }

    return QObject::eventFilter(watched, event);
}

#include "moc_urlmimedata.cpp"
