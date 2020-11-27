/*
    SPDX-FileCopyrightText: 2008 David Edmundson <kde@davidedmundson.co.uk>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "loadView.h"
#include "codeine.h"

#include <QLabel>
#include <QIcon>


namespace Dragon
{

LoadView::LoadView( QWidget *parent )
    : QWidget( parent )
{
    setupUi( this );
    setStyleSheet( QLatin1String( "QPushButton { text-align: center; }" ));

    connect(m_playDiskButton, &QAbstractButton::clicked, this, &LoadView::openDVDPressed);
    connect(m_playFileButton, &QAbstractButton::clicked, this, &LoadView::openFilePressed);
    connect(m_playStreamButton, &QAbstractButton::clicked, this, &LoadView::openStreamPressed);
    connect(m_recentlyPlayed, QOverload<const QUrl&>::of(&RecentlyPlayedList::itemDoubleClicked),
            this, &LoadView::loadUrl);
    connect(this, &LoadView::reloadRecentlyList, m_recentlyPlayed, &RecentlyPlayedList::loadEntries);
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
