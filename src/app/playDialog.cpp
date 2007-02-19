// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include "config.h"
#include "listView.cpp"
#include <kapplication.h>
#include <kconfig.h>
#include <kguiitem.h>
#include <klistview.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>
#include "playDialog.h"
#include "mxcl.library.h"
#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qsignalmapper.h>

QString i18n( const char *text );


namespace Codeine {


PlayDialog::PlayDialog( QWidget *parent, bool be_welcome_dialog )
      : QDialog( parent )
{
   setCaption( kapp->makeStdCaption( i18n("Play Media") ) );

   QSignalMapper *mapper = new QSignalMapper( this );
   QWidget *o, *closeButton = new KPushButton( KStdGuiItem::close(), this );
   QBoxLayout *hbox, *vbox = new QVBoxLayout( this, 15, 20 );

   vbox->addWidget( new QLabel( i18n( "What media would you like to play?" ), this ) );

   QGridLayout *grid = new QGridLayout( vbox, 1, 3, 20 );

   //TODO use the kguiItems from the actions
   mapper->setMapping( o = new KPushButton( KGuiItem( i18n("Play File..."), "fileopen" ), this ), FILE );
   connect( o, SIGNAL(clicked()), mapper, SLOT(map()) );
   grid->QLayout::add( o );

   mapper->setMapping( o = new KPushButton( KGuiItem( i18n("Play VCD"), "cdaudio_unmount" ), this ), VCD );
   connect( o, SIGNAL(clicked()), mapper, SLOT(map()) );
   grid->QLayout::add( o );

   mapper->setMapping( o = new KPushButton( KGuiItem( i18n("Play DVD"), "dvd_unmount" ), this ), DVD );
   connect( o, SIGNAL(clicked()), mapper, SLOT(map()) );
   grid->QLayout::add( o );

   mapper->setMapping( closeButton, QDialog::Rejected );
   connect( closeButton, SIGNAL(clicked()), mapper, SLOT(map()) );

   createRecentFileWidget( vbox );

   hbox = new QHBoxLayout( vbox );
   hbox->addItem( new QSpacerItem( 10, 10, QSizePolicy::Expanding ) );

   if( be_welcome_dialog ) {
      QWidget *w = new KPushButton( KStdGuiItem::quit(), this );
      hbox->addWidget( w );
      connect( w, SIGNAL(clicked()), kapp, SLOT(quit()) );
   }

   hbox->addWidget( closeButton );

   connect( mapper, SIGNAL(mapped( int )), SLOT(done( int )) );
}

void
PlayDialog::createRecentFileWidget( QBoxLayout *layout )
{
   KListView *lv;
   lv = new Codeine::ListView( this );
   lv->setColumnText( 1, i18n("Recently Played Media") );

   const QStringList list1 = Codeine::config( "General" )->readPathListEntry( "Recent Urls" );
   KURL::List urls;

   foreach( list1 )
      urls += *it;

   for( KURL::List::Iterator it = urls.begin(), end = urls.end(); it != end; ) {
      if( urls.contains( *it ) > 1 )
         //remove duplicates
         it = urls.remove( it );
      else if( (*it).protocol() == "file" && !QFile::exists( (*it).path() ) )
         //remove stale entries
         it = urls.remove( it );
      else
         ++it;
   }

   for( KURL::List::ConstIterator it = urls.begin(), end = urls.end(); it != end; ++it ) {
      const QString fileName = (*it).fileName();
      new KListViewItem( lv, 0, (*it).url(), fileName.isEmpty() ? (*it).prettyURL() : fileName );
   }

   if( lv->childCount() ) {
      layout->addWidget( lv, 1 );
      connect( lv, SIGNAL(executed( QListViewItem* )), SLOT(done( QListViewItem* )) );
   }
   else
      delete lv;
}

void
PlayDialog::done( QListViewItem *item )
{
   m_url = item->text( 0 );
   QDialog::done( RECENT_FILE );
}

}
