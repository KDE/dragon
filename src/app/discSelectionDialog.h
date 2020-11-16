/*
    SPDX-FileCopyrightText: 2008 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYER_DISCSELECTIONDIALOG_H
#define DRAGONPLAYER_DISCSELECTIONDIALOG_H

#include <QList>

#include <QDialog>
#include <Solid/Device>

class QListWidget;
class QListWidgetItem;

class DiscSelectionDialog : public QDialog
{
    Q_OBJECT
public:
    DiscSelectionDialog( QWidget* parent, const QList< Solid::Device >& deviceList );
private Q_SLOTS:
    void discItemSelected( QListWidgetItem *item );
    void okClicked();
private:
    void openItem( QListWidgetItem *item );
    QListWidget* m_listWidget;
};

#endif
