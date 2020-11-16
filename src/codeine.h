/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef CODEINE_H
#define CODEINE_H

#include <config.h>

// try to keep this file light. It gets included by
// practically every implementation and many headers

namespace Engine
{}

class QWidget;

namespace Dragon
{
QWidget *mainWindow(); //defined in mainWindow.cpp
}

/// used by mainWindow.h
int main( int, char** );

#define APP_VERSION PROJECT_VERSION
#define APP_NAME "dragonplayer"

#endif
