/*
    SPDX-FileCopyrightText: 2008 David Edmundson <kde@davidedmundson.co.uk>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef RECENTLYPLAYEDLIST_H
#define RECENTLYPLAYEDLIST_H

#include <QListWidget>
#include <QUrl>

#include <KConfigGroup>

class RecentlyPlayedList : public QListWidget
{
    Q_OBJECT
public:
    explicit RecentlyPlayedList(QWidget*);
    ~RecentlyPlayedList() override;
private:
    void contextMenuEvent(QContextMenuEvent*) override;
    KConfigGroup* configGroup;
public Q_SLOTS:
    void loadEntries();
    void removeEntry();
    void clearList();
    void copyUrl();
    void itemDoubleClicked(QListWidgetItem*);
Q_SIGNALS:
    void itemDoubleClicked(const QUrl&);
};

#endif
