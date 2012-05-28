/***********************************************************************
 * Copyright 2012 Trever Fischer <tdfischer@fedoraproject.org>
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

#include "recentlyPlayedModel.h"
#include <KGlobal>
#include <KIcon>
#include <KConfig>

RecentlyPlayedModel::RecentlyPlayedModel(QObject *parent)
  : QAbstractListModel(parent)
{
  m_configGroup = new KConfigGroup( KGlobal::config(), "General");
  loadEntries();
}

void
RecentlyPlayedModel::loadEntries()
{
  beginResetModel();
  const QStringList entries = m_configGroup->readPathEntry( "Recent Urls", QStringList() );

  QListIterator<QString> i(entries);
  i.toBack();
  while(i.hasPrevious())
  {
	KUrl url = KUrl(i.previous());
  RecentlyPlayedEntry entry;
  entry.url = url;
  entry.name = url.fileName().isEmpty() ? url.prettyUrl() : url.fileName();

	if(KConfigGroup( KGlobal::config(), url.prettyUrl()).readPathEntry( "IsVideo", QString() )==QLatin1String( "false" ))
	  entry.icon = KIcon( QLatin1String(  "audio-x-generic" ) );
	else
	  entry.icon = KIcon( QLatin1String(  "video-x-generic" ) );
  m_recent << entry;
  }
  endResetModel();
}

int
RecentlyPlayedModel::rowCount(const QModelIndex &parent) const
{
    return m_recent.size();
}

QVariant
RecentlyPlayedModel::data(const QModelIndex &idx, int role) const
{
    RecentlyPlayedEntry item = m_recent[idx.row()];
    switch (role) {
        case Qt::DisplayRole:
            return item.name;
        case Qt::DecorationRole:
            return item.icon;
        case URLRole:
            return item.url;
    }
    return QVariant();
}
