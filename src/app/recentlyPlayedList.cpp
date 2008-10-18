#include "recentlyPlayedList.h"
#include <KListWidget>
#include <KApplication>
#include <KConfig>
#include <KMenu>
#include <KDialog>
#include <KLocale>

#include <QFile>
#include <QContextMenuEvent>
#include <KDebug>


//this is a widget for dispaying the rcently played items in a list. It is subclassed so that we can hook up a context menu
RecentlyPlayedList::RecentlyPlayedList(QWidget *parent)
		:KListWidget(parent)
{
  setAlternatingRowColors( true );
  setSelectionMode(QAbstractItemView::SingleSelection);
  
  configGroup = new KConfigGroup( KGlobal::config(), "General" );

  loadEntries();

}

void
RecentlyPlayedList::loadEntries()
{
  clear();
  const QStringList entries = configGroup->readPathEntry( "Recent Urls", QStringList() );

//   foreach( const QString &s, list1 )
//       urls.prepend(s); //copy the stringlist individually into a KURL list in reverse order
// 
//   foreach( const KUrl &it, urls) {
//       while( urls.count( it ) > 1 )
//           urls.removeAt( urls.indexOf(it) );
//       if( it.protocol() == "file" && !QFile::exists( it.path() ) )
//           //remove stale entries
//           urls.removeAll( it );
//     }

  //itterate backwards because the newest entry is at the end
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
  kdDebug() << "Loading Menu";
  menu.addAction(KIcon("list-remove"),"Remove Entry",this,SLOT(removeEntry()));
  menu.addAction(KIcon("list-remove"),"Clear List",this,SLOT(clearList()));
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

#include "recentlyPlayedList.moc"
