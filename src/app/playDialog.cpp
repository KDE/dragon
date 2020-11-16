/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "playDialog.h"

#include "mainWindow.h"
#include "recentlyPlayedList.h"

#include <QApplication>
#include <QDebug>
#include <QPushButton>
#include <QFile>
#include <QLabel>
#include <QLayout>
#include <QSignalMapper>
#include <QIcon>

#include <KLocalizedString>
#include <KStandardGuiItem>

namespace Dragon {

PlayDialog::PlayDialog( QWidget *parent, bool be_welcome_dialog )
    : QDialog( parent )
{
    setWindowTitle( i18nc("@title:window", "Play Media") );

    QSignalMapper *mapper = new QSignalMapper( this );
    QWidget *o;
    QPushButton *closeButton = new QPushButton( this );
    KGuiItem::assign(closeButton, KStandardGuiItem::close());
    QBoxLayout *vbox = new QVBoxLayout();
    //vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing( 15 );
    //    hbox->setMargin( 15 );  vbox->setMargin( 15 );
    //    hbox->setSpacing( 20 ); vbox->setSpacing( 20 );

    vbox->addWidget( new QLabel( i18n( "What media would you like to play?" ), this ) );

    QGridLayout *grid = new QGridLayout();
    vbox->addLayout( grid );
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setVerticalSpacing( 20 );

    //TODO use the kguiItems from the actions
    mapper->setMapping( o = new QPushButton( QIcon::fromTheme( "document-open" ), i18nc("@action:button", "Play File..."), this ), FILE );
    connect( o, SIGNAL(clicked()), mapper, SLOT(map()) );
    grid->addWidget( o, 0, 0 );

    mapper->setMapping( o = new QPushButton( QIcon::fromTheme( "media-optical-video" ), i18nc("@action:button", "Play Disc"), this ), DVD );
    connect( o, SIGNAL(clicked()), mapper, SLOT(map()) );
    grid->addWidget( o, 0, 1 );

    mapper->setMapping( closeButton, QDialog::Rejected );
    connect( closeButton, SIGNAL(clicked()), mapper, SLOT(map()) );

    createRecentFileWidget( grid );

    QBoxLayout *hbox = new QHBoxLayout();
    hbox->addItem( new QSpacerItem( 10, 10, QSizePolicy::Expanding ) );

    if( be_welcome_dialog ) {
        QPushButton *w = new QPushButton( this );
        KGuiItem::assign(w, KStandardGuiItem::quit());
        hbox->addWidget( w );
        connect( w, SIGNAL(clicked()), qApp, SLOT(closeAllWindows()) );
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
        connect( lv, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(finished(QListWidgetItem*)) );
    }
    else
        delete lv;
}

void
PlayDialog::finished(QListWidgetItem * item )
{
    m_url = item->data( 0xdecade ).value<QUrl>();
    ((Dragon::MainWindow*) mainWindow() )->openRecentFile( m_url );
}

}
