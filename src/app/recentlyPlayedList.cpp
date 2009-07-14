/***********************************************************************
 * Copyright 2008 David Edmundson <kde@davidedmundson.co.uk>
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

#include "recentlyPlayedList.h"
#include <KListWidget>
#include <KApplication>
#include <KConfig>
#include <KMenu>
#include <KDialog>
#include <KLocale>

#include <QFile>
#include <QContextMenuEvent>
#include "debug.h"

//this is a widget for dispaying the rcently played items in a list. It is subclassed so that we can hook up a context menu
RecentlyPlayedList::RecentlyPlayedList(QWidget *parent)
		:KListWidget(parent)
{
  connect(this,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this, SLOT(itemDoubleClicked(QListWidgetItem*)));
  setAlternatingRowColors( true );
  setSelectionMode(QAbstractItemView::SingleSelection);
  
  configGroup = new KConfigGroup( KGlobal::config(), "General" );
  loadEntries();
}

RecentlyPlayedList::~RecentlyPlayedList()
{
  delete configGroup;
}

void
RecentlyPlayedList::loadEntries()
{
  clear();
  const QStringList entries = configGroup->readPathEntry( "Recent Urls", QStringList() );

  QListIterator<QString> i(entries);
  i.toBack();
  while(i.hasPrevious())
  {
	KUrl url = KUrl(i.previous());	  
	QListWidgetItem* listItem = new QListWidgetItem(  url.fileName().isEmpty() ? url.prettyUrl() : url.fileName() );
	listItem->setData( 0xdecade, QVariant::fromValue( url ) );

	if(KConfigGroup( KGlobal::config(), url.prettyUrl()).readPathEntry( "IsVideo", QString() )=="false")
	  listItem->setIcon( KIcon( "audio-x-generic" ) );
	else
	  listItem->setIcon( KIcon( "video-x-generic" ) );
	addItem( listItem );
  }
}

void
RecentlyPlayedList::contextMenuEvent(QContextMenuEvent * event )
{
  KMenu menu;
  debug() << "Loading Menu";
  menu.addAction(KIcon("list-remove"),i18n("Remove Entry"),this,SLOT(removeEntry()));
  menu.addAction(KIcon("list-remove"),i18n("Clear List"),this,SLOT(clearList()));
  menu.exec( event->globalPos() );
}

void
RecentlyPlayedList::removeEntry()
{
  QStringList list = configGroup->readPathEntry( "Recent Urls", QStringList() );
  KUrl toRemove = currentItem()->data(0xdecade).value<KUrl>();
  list.removeAll(toRemove.prettyUrl());
  configGroup->writePathEntry("Recent Urls",list.join(","));
  loadEntries();
}

void
RecentlyPlayedList::clearList()
{
  configGroup->writePathEntry("Recent Urls","");
  loadEntries();
}

//send the url for the item clicked, not the item
void
RecentlyPlayedList::itemDoubleClicked(QListWidgetItem* item)
{
  KUrl url = item->data(0xdecade).value<KUrl>();
  emit(itemDoubleClicked(url));
}

#include "recentlyPlayedList.moc"
