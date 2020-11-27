/*
    SPDX-FileCopyrightText: 2008 David Edmundson <kde@davidedmundson.co.uk>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "recentlyPlayedList.h"

#include <QListWidget>
#include <QApplication>
#include <QClipboard>
#include <KConfig>
#include <QDebug>
#include <QMenu>
#include <QDialog>
#include <QFile>
#include <QFileInfo>
#include <QContextMenuEvent>
#include <QIcon>

#include <KLocalizedString>
#include <KMessageBox>
#include <KSharedConfig>

//this is a widget for dispaying the recently played items in a list. It is subclassed so that we can hook up a context menu
RecentlyPlayedList::RecentlyPlayedList(QWidget *parent)
    : QListWidget(parent)
{
    connect(this, QOverload<QListWidgetItem*>::of(&QListWidget::itemDoubleClicked),
            this, QOverload<QListWidgetItem*>::of(&RecentlyPlayedList::itemDoubleClicked));
    setAlternatingRowColors( true );
    setSelectionMode(QAbstractItemView::SingleSelection);

    QAction *copy = new QAction(i18nc("@action Copy the URL of the selected multimedia", "Copy URL"), this);
    copy->setIcon(QIcon::fromTheme(QStringLiteral("edit-copy")));
    connect(copy, &QAction::triggered, this, &RecentlyPlayedList::copyUrl);
    copy->setShortcut(QKeySequence::Copy);

    QAction *clear = new QAction(i18nc("@action", "Clear List"), this);
    clear->setIcon(QIcon::fromTheme(QStringLiteral("edit-clear-list")));
    connect(clear, &QAction::triggered, this, &RecentlyPlayedList::clearList);
    clear->setShortcut(QKeySequence::Cut);

    QAction *remove = new QAction(i18nc("@action", "Remove Entry"), this);
    remove->setIcon(QIcon::fromTheme(QStringLiteral("list-remove")));
    connect(remove, &QAction::triggered, this, &RecentlyPlayedList::removeEntry);
    remove->setShortcut(QKeySequence::Delete);

    addActions({copy, remove, clear});

    configGroup = new KConfigGroup( KSharedConfig::openConfig(), "General" );
    loadEntries();
}

RecentlyPlayedList::~RecentlyPlayedList()
{
    delete configGroup;
}

void RecentlyPlayedList::loadEntries()
{
    clear();
    const QStringList entries = configGroup->readPathEntry( "Recent Urls", QStringList() );

    QListIterator<QString> i(entries);
    i.toBack();
    while(i.hasPrevious()) {
        QUrl url = QUrl(i.previous()); // kf5 FIXME?
        QListWidgetItem* listItem = new QListWidgetItem(  url.fileName().isEmpty() ? url.toDisplayString() : url.fileName() );
        listItem->setData( 0xdecade, QVariant::fromValue( url ) );

        if(KConfigGroup( KSharedConfig::openConfig(), url.toDisplayString() ).readPathEntry( "IsVideo", QString() )==QLatin1String( "false" ))
            listItem->setIcon( QIcon::fromTheme( QLatin1String( "audio-x-generic" ) ) );
        else
            listItem->setIcon( QIcon::fromTheme( QLatin1String( "video-x-generic" ) ) );
        addItem( listItem );
    }
}

void RecentlyPlayedList::contextMenuEvent(QContextMenuEvent * event )
{
    if (!currentItem())
        return;
    QMenu menu;
    menu.addActions(actions());
    menu.exec(event->globalPos());
}

void RecentlyPlayedList::removeEntry()
{
    if (!currentItem())
        return;
    const auto list = configGroup->readPathEntry( "Recent Urls", QStringList() );
    const QUrl toRemove = currentItem()->data(0xdecade).value<QUrl>();
    auto urls = QUrl::fromStringList(list);
    urls.removeAll(toRemove);
    configGroup->writePathEntry("Recent Urls", QUrl::toStringList(urls));
    loadEntries();
}

void RecentlyPlayedList::clearList()
{
    configGroup->writePathEntry("Recent Urls",QString());
    loadEntries();
}

void RecentlyPlayedList::copyUrl()
{
    if (!currentItem())
        return;
    const QUrl toCopy = currentItem()->data(0xdecade).toUrl();
    QApplication::clipboard()->setText(toCopy.toString());
}

//send the url for the item clicked, not the item
void RecentlyPlayedList::itemDoubleClicked(QListWidgetItem* item)
{
    const QUrl url = item->data(0xdecade).value<QUrl>();

    if( url.isLocalFile() ) {
        QFileInfo fileInfo( url.toLocalFile() );

        if( !fileInfo.exists() ) {
            if( KMessageBox::questionYesNo( this,
                                            i18n( "This file could not be found. Would you like to remove it from the playlist?" ),
                                            i18nc("@title:window", "File not found" ) ) == KMessageBox::Yes ) {
                removeEntry();
            }

            return;
        }
    }

    Q_EMIT itemDoubleClicked(url);
}
