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

#include "playDialog.h"

#include "mainWindow.h"
#include "recentlyPlayedList.h"

#include <KApplication>
#include <KConfig>
#include <KDebug>
#include <KDialog>
#include <KLocale>
#include <KGuiItem>
#include <KPushButton>
#include <KStandardGuiItem>

#include <QFile>
#include <QLabel>
#include <QLayout>
#include <QSignalMapper>

namespace Dragon {

PlayDialog::PlayDialog( QWidget *parent, bool be_welcome_dialog )
        : KDialog( parent )
{
    setWindowTitle( KDialog::makeStandardCaption( i18n("Play Media") ) );

    QSignalMapper *mapper = new QSignalMapper( this );
    QWidget *o, *closeButton = new KPushButton( KStandardGuiItem::close(), this );
    QBoxLayout *vbox = new QVBoxLayout();
    //vbox->setMargin( 0 );
    vbox->setSpacing( 15 );
//    hbox->setMargin( 15 );  vbox->setMargin( 15 );
//    hbox->setSpacing( 20 ); vbox->setSpacing( 20 );

    vbox->addWidget( new QLabel( i18n( "What media would you like to play?" ), this ) );

    QGridLayout *grid = new QGridLayout();
    vbox->addLayout( grid );
    grid->setMargin( 0 );
    grid->setVerticalSpacing( 20 );

    //TODO use the kguiItems from the actions
    mapper->setMapping( o = new KPushButton( KGuiItem( i18n("Play File..."), QLatin1String( "document-open" ) ), this ), FILE );
    connect( o, SIGNAL(clicked()), mapper, SLOT(map()) );
    grid->addWidget( o, 0, 0 );

    mapper->setMapping( o = new KPushButton( KGuiItem( i18n("Play Disc"), QLatin1String( "media-optical-video" ) ), this ), DVD );
    connect( o, SIGNAL(clicked()), mapper, SLOT(map()) );
    grid->addWidget( o, 0, 1 );

    mapper->setMapping( closeButton, QDialog::Rejected );
    connect( closeButton, SIGNAL(clicked()), mapper, SLOT(map()) );

    createRecentFileWidget( grid );

    QBoxLayout *hbox = new QHBoxLayout();
    hbox->addItem( new QSpacerItem( 10, 10, QSizePolicy::Expanding ) );

    if( be_welcome_dialog ) {
        QWidget *w = new KPushButton( KStandardGuiItem::quit(), this );
        hbox->addWidget( w );
        connect( w, SIGNAL(clicked()), kapp, SLOT(closeAllWindows()) );
    }

    hbox->addWidget( closeButton );

    connect( mapper, SIGNAL(mapped(int)), mainWindow(), SLOT(playDialogResult(int)) );
    vbox->addLayout( hbox );
    setLayout( vbox );
    setAttribute( Qt::WA_DeleteOnClose, true );
}

void
PlayDialog::createRecentFileWidget( QGridLayout *layout )
{
    RecentlyPlayedList *lv = new RecentlyPlayedList( this );


    //delete list view widget if there are no items in it
    if( lv->count() ) {
        layout->addWidget( lv, 1, 0, 1, -1);
        connect( lv, SIGNAL(executed(QListWidgetItem*)), this, SLOT(finished(QListWidgetItem*)) );
    }
    else
        delete lv;
}

void
PlayDialog::finished( QListWidgetItem *item )
{
    m_url = item->data( 0xdecade ).value<KUrl>();
    ((Dragon::MainWindow*) mainWindow() )->openRecentFile( m_url );
}

}

#include "playDialog.moc"
