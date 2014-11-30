/***********************************************************************
 * Copyright 2011  Geoffry Song <goffrie@gmail.com>
 * Copyright 2014 Lukáš Tinkl <lukas@kde.org>
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

#include "playerApplication.h"
#include "mainWindow.h"

#include <KWindowSystem>

namespace Dragon
{

PlayerApplication::PlayerApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_mainWindow(Q_NULLPTR)
{
}

PlayerApplication::~PlayerApplication()
{
    if (m_mainWindow) {
        m_mainWindow = Q_NULLPTR;
        delete m_mainWindow;
    }
}

void PlayerApplication::slotActivateRequested(const QStringList &arguments, const QString &workingDirectory)
{
    Q_UNUSED(workingDirectory)
    qDebug() << Q_FUNC_INFO << arguments;
    if (!arguments.filter("play-dvd", Qt::CaseInsensitive).isEmpty()) {
        newInstance(true);
        forceActiveWindow();
    } else if (arguments.count() == 2) { // 1st arg binary name, 2nd arg file to open
        QString urlArg = arguments.at(1);
        QUrl url;
        if (urlArg.startsWith("/")) {
            url = QUrl::fromLocalFile(urlArg);
        } else {
            url = QUrl(urlArg);
        }
        newInstance(false, {url});
        forceActiveWindow();
    }
}

void PlayerApplication::slotOpenRequested(const QList<QUrl> &uris)
{
    qDebug() << Q_FUNC_INFO << uris;
    newInstance(false, uris);
    forceActiveWindow();
}

void PlayerApplication::forceActiveWindow()
{
    KWindowSystem::forceActiveWindow(Dragon::mainWindow()->winId());
}

void PlayerApplication::newInstance(bool playDisc, const QList<QUrl> &uris)
{
    if (!m_mainWindow)
        m_mainWindow = new Dragon::MainWindow;

    if (isSessionRestored())
        m_mainWindow->restore(1, false);
    else if (playDisc)
        m_mainWindow->playDisc();
    else if (!uris.isEmpty()) {
        m_mainWindow->open(uris.first());
        m_mainWindow->adjustSize();
    }

    m_mainWindow->show();
}

} // namespace Dragon
