// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include "configfn.h"
#include "listView.cpp"
#include "playDialog.h"

#include <k3listview.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdialog.h>
#include <klocale.h>
#include <kguiitem.h>
#include <kpushbutton.h>
#include <kstandardguiitem.h>

#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qsignalmapper.h>


QString i18n( const char *text );


namespace Codeine {


PlayDialog::PlayDialog( QWidget *parent, bool be_welcome_dialog )
        : QDialog( parent )
{
    setWindowTitle( KDialog::makeStandardCaption( i18n("Play Media") ) );

    QSignalMapper *mapper = new QSignalMapper( this );
    QWidget *o, *closeButton = new KPushButton( KStandardGuiItem::close(), this );
    QBoxLayout *hbox = new QVBoxLayout( this );
    QBoxLayout *vbox = new QVBoxLayout( this );
    hbox->setMargin( 15 );  vbox->setMargin( 15 );
    hbox->setSpacing( 20 ); vbox->setSpacing( 20 );

    vbox->addWidget( new QLabel( i18n( "What media would you like to play?" ), this ) );

    QGridLayout *grid = new QGridLayout( this );
    vbox->addLayout( grid );
    grid->setMargin( 20 );

    //TODO use the kguiItems from the actions
    mapper->setMapping( o = new KPushButton( KGuiItem( i18n("Play File..."), "fileopen" ), this ), FILE );
    connect( o, SIGNAL(clicked()), mapper, SLOT(map()) );
    grid->addWidget( o, 0, 0 );

    mapper->setMapping( o = new KPushButton( KGuiItem( i18n("Play VCD"), "cdaudio_unmount" ), this ), VCD );
    connect( o, SIGNAL(clicked()), mapper, SLOT(map()) );
    grid->addWidget( o, 0, 1 );

    mapper->setMapping( o = new KPushButton( KGuiItem( i18n("Play DVD"), "dvd_unmount" ), this ), DVD );
    connect( o, SIGNAL(clicked()), mapper, SLOT(map()) );
    grid->addWidget( o, 0, 2 );

    mapper->setMapping( closeButton, QDialog::Rejected );
    connect( closeButton, SIGNAL(clicked()), mapper, SLOT(map()) );

    createRecentFileWidget( vbox );

    hbox = new QHBoxLayout( this );
    vbox->addLayout( hbox );
    hbox->addItem( new QSpacerItem( 10, 10, QSizePolicy::Expanding ) );

    if( be_welcome_dialog ) {
        QWidget *w = new KPushButton( KStandardGuiItem::quit(), this );
        hbox->addWidget( w );
        connect( w, SIGNAL(clicked()), kapp, SLOT(quit()) );
    }

    hbox->addWidget( closeButton );

    connect( mapper, SIGNAL(mapped( int )), SLOT(done( int )) );
}

void
PlayDialog::createRecentFileWidget( QBoxLayout *layout )
{
    K3ListView *lv;
    lv = new Codeine::ListView( this );
    lv->setColumnText( 1, i18n("Recently Played Media") );

    const QStringList list1 = Codeine::config( "General" ).readPathEntry( "Recent Urls", QStringList() );
    KUrl::List urls;

    foreach( QString s, list1 )
        urls += s;

    foreach( KUrl it, urls) {
        while( urls.count( it ) > 1 )
            urls.removeAt( urls.indexOf(it) );
        if( it.protocol() == "file" && !QFile::exists( it.path() ) )
            //remove stale entries
            urls.removeAll( it );
    }

    for( KUrl::List::ConstIterator it = urls.begin(), end = urls.end(); it != end; ++it ) {
        const QString fileName = (*it).fileName();
        new K3ListViewItem( lv, 0, (*it).url(), fileName.isEmpty() ? (*it).prettyUrl() : fileName );
    }

    if( lv->childCount() ) {
        layout->addWidget( lv, 1 );
        connect( lv, SIGNAL(executed( Q3ListViewItem* )), SLOT(done( Q3ListViewItem* )) );
    }
    else
        delete lv;
}

void
PlayDialog::done( Q3ListViewItem *item )
{
    m_url = item->text( 0 );
    QDialog::done( RECENT_FILE );
}

}

#include "playDialog.moc"
