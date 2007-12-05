/***********************************************************************
 * Copyright 2005  Max Howell <max.howell@methylblue.com>
 *           2007  Ian Monroe <ian@monroe.nu>
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

#include "listView.cpp"
#include "playDialog.h"

#include <KApplication>
#include <KConfig>
#include <KDialog>
#include <KLocale>
#include <KGuiItem>
#include <KPushButton>
#include <KStandardGuiItem>

#include <QFile>
#include <QLabel>
#include <QLayout>
#include <QSignalMapper>

namespace Codeine {

PlayDialog::PlayDialog( QWidget *parent, bool be_welcome_dialog )
        : QDialog( parent )
{
    setWindowTitle( KDialog::makeStandardCaption( i18n("Play Media") ) );

    QSignalMapper *mapper = new QSignalMapper( this );
    QWidget *o, *closeButton = new KPushButton( KStandardGuiItem::close(), this );
    QBoxLayout *hbox = new QVBoxLayout();
    QBoxLayout *vbox = new QVBoxLayout();
    hbox->setMargin( 15 );  vbox->setMargin( 15 );
    hbox->setSpacing( 20 ); vbox->setSpacing( 20 );

    vbox->addWidget( new QLabel( i18n( "What media would you like to play?" ), this ) );

    QGridLayout *grid = new QGridLayout();
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

    hbox = new QHBoxLayout();
    vbox->addLayout( hbox );
    hbox->addItem( new QSpacerItem( 10, 10, QSizePolicy::Expanding ) );

    if( be_welcome_dialog ) {
        QWidget *w = new KPushButton( KStandardGuiItem::quit(), this );
        hbox->addWidget( w );
        connect( w, SIGNAL(clicked()), kapp, SLOT(quit()) );
    }

    hbox->addWidget( closeButton );

    connect( mapper, SIGNAL(mapped( int )), SLOT(done( int )) );
    setLayout( vbox );
}

void
PlayDialog::createRecentFileWidget( QBoxLayout *layout )
{
    QListWidget *lv;
    lv = new Codeine::ListView( this );
//    lv->setColumnText( 1, i18n("Recently Played Media") );

    const QStringList list1 = KConfigGroup( KGlobal::config(), "General" ).readPathEntry( "Recent Urls", QStringList() );
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
        //new QTableWidgetItem( lv, 0, (*it).url(), fileName.isEmpty() ? (*it).prettyUrl() : fileName );
        QListWidgetItem* listItem = new QListWidgetItem(  fileName.isEmpty() ? (*it).prettyUrl() : fileName );
        lv->addItem( listItem );
    }

    if( lv->count() ) {
        layout->addWidget( lv, 1 );
        connect( lv, SIGNAL(executed( QListWidget* )), SLOT(done( QListWidget* )) );
    }
    else
        delete lv;
}

void
PlayDialog::done( QListWidgetItem *item )
{
    m_url = item->text();
    QDialog::done( RECENT_FILE );
}

}

#include "playDialog.moc"
