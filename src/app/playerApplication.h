/*
    SPDX-FileCopyrightText: 2011 Geoffry Song <goffrie@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

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
