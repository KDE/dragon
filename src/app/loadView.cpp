/***********************************************************************
 * Copyright 2008  David Edmundson <kde@davidedmundson.co.uk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************/

#include "loadView.h"
#include "codeine.h"

#include <QLabel>
#include <QIcon>

#include <KIconLoader>

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
}

void LoadView::setThumbnail(QWidget *object)
{
    object->setParent(m_vThumb);
    object->resize(m_vThumb->size());
    object->show();
}

}
