/*
    SPDX-FileCopyrightText: 2011 Geoffry Song <goffrie@gmail.com>
    SPDX-FileCopyrightText: 2014 Lukáš Tinkl <lukas@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "playerApplication.h"
#include "mainWindow.h"

#include <KWindowSystem>

namespace Dragon
{

PlayerApplication::PlayerApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_mainWindow(nullptr)
{
}

PlayerApplication::~PlayerApplication()
{
    if (m_mainWindow) {
        m_mainWindow = nullptr;
        delete m_mainWindow;
    }
}

void PlayerApplication::slotActivateRequested(const QStringList &arguments, const QString &workingDirectory)
{
    qDebug() << Q_FUNC_INFO << arguments;
    if (!arguments.filter(QStringLiteral("play-dvd"), Qt::CaseInsensitive).isEmpty()) {
        newInstance(true);
        forceActiveWindow();
    } else if (arguments.count() == 2) { // 1st arg binary name, 2nd arg file to open
        QUrl url = QUrl::fromUserInput(arguments.at(1),
                                       workingDirectory,
                                       QUrl::AssumeLocalFile);
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
