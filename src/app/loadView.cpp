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

    connect( m_playDiskButton, SIGNAL(clicked()), this, SIGNAL(openDVDPressed()) );
    connect( m_playFileButton, SIGNAL(clicked()), this, SIGNAL(openFilePressed()) );
    connect( m_playStreamButton, SIGNAL(clicked()), this, SIGNAL(openStreamPressed()) );
    connect( m_recentlyPlayed, SIGNAL(itemDoubleClicked(QUrl)), this, SIGNAL(loadUrl(QUrl)) );
    connect( this, SIGNAL(reloadRecentlyList()), m_recentlyPlayed, SLOT(loadEntries()) );
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
