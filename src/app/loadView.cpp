/*
    SPDX-FileCopyrightText: 2008 David Edmundson <kde@davidedmundson.co.uk>
    SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "loadView.h"
#include "codeine.h"

#include <QIcon>
#include <QLabel>

namespace Dragon
{

LoadView::LoadView(QWidget *parent)
    : QWidget(parent)
    , Ui_LoadView()
{
    setupUi(this);
    setStyleSheet(QLatin1String("QPushButton { text-align: center; }"));

    connect(m_playDiskButton, &QAbstractButton::clicked, this, &LoadView::openDVDPressed);
    connect(m_playFileButton, &QAbstractButton::clicked, this, &LoadView::openFilePressed);
    connect(m_playStreamButton, &QAbstractButton::clicked, this, &LoadView::openStreamPressed);
    connect(m_recentlyPlayed, QOverload<const QUrl &>::of(&RecentlyPlayedList::itemDoubleClicked), this, &LoadView::loadUrl);
    connect(this, &LoadView::reloadRecentlyList, m_recentlyPlayed, &RecentlyPlayedList::loadEntries);
    connect(m_clearRecent, &QPushButton::clicked, m_recentlyPlayed, &RecentlyPlayedList::clearList);
    auto enableClearButton = [this] {
        m_clearRecent->setEnabled(m_recentlyPlayed->count() > 0);
    };
    connect(m_recentlyPlayed, &RecentlyPlayedList::changed, this, enableClearButton);
    enableClearButton();
}

void LoadView::setThumbnail(QWidget *object)
{
    if (!object) {
        m_vThumb->hide();
        return;
    }
    m_vThumb->show();
    object->setParent(m_vThumb);
    object->resize(m_vThumb->size());
    object->show();
}

}
