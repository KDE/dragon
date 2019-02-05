/***********************************************************************
 * Copyright 2011  Geoffry Song <goffrie@gmail.com>
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

#ifndef DRAGONPLAYER_PLAYERAPPLICATION_H
#define DRAGONPLAYER_PLAYERAPPLICATION_H

#include <QApplication>

namespace Dragon
{
class MainWindow;

class PlayerApplication : public QApplication
{
    Q_OBJECT
public:
    PlayerApplication(int &argc, char **argv);
    ~PlayerApplication() override;
    void newInstance(bool playDisc = false, const QList<QUrl> &uris = QList<QUrl>());

public Q_SLOTS:
    void slotActivateRequested(const QStringList &arguments, const QString &workingDirectory);
    void slotOpenRequested(const QList<QUrl> &uris);

private:
    void forceActiveWindow();
    MainWindow *m_mainWindow;
};

} // namespace Dragon

#endif
