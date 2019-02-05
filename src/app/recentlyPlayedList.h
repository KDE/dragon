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
    void itemDoubleClicked(QListWidgetItem*);
Q_SIGNALS:
    void itemDoubleClicked(QUrl);
};

#endif
