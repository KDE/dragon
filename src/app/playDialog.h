/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYERPLAYDIALOG_H
#define DRAGONPLAYERPLAYDIALOG_H

#include <QDialog>
#include <QUrl>

class QListWidgetItem;
class QGridLayout;

namespace Dragon
{
class PlayDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PlayDialog( QWidget*, bool show_welcome_dialog = false );

    QUrl url() const { return m_url; }

    enum DialogCode { FILE = QDialog::Accepted + 2, VCD, DVD, RECENT_FILE };

private Q_SLOTS:
    void finished(QListWidgetItem *item );

private:
    void createRecentFileWidget( QGridLayout* );

    QUrl m_url;
};
}

#endif
