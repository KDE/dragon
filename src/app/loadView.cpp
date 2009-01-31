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
#include <QLabel>
#include <KStandardDirs>


namespace Codeine
{

LoadView::LoadView( QWidget *parent) 
    : QWidget( parent )
{
  setupUi(this);
  
  m_playDiskButton->setIcon(KIcon("media-optical"));
  m_playDiskButton->setIconSize(QSize(64,64));
  m_playFileButton->setIcon(KIcon("folder"));
  m_playFileButton->setIconSize(QSize(64,64));
  connect(m_playDiskButton,SIGNAL(released()),this,SIGNAL(openDVDPressed()));
  connect(m_playFileButton,SIGNAL(released()),this,SIGNAL(openFilePressed())); 
  connect(m_recentlyPlayed,SIGNAL(itemDoubleClicked(KUrl)),this,SIGNAL(loadUrl(KUrl)));
}



}