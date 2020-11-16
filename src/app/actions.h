/*
    SPDX-FileCopyrightText: 2004 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYERACTIONS_H
#define DRAGONPLAYERACTIONS_H

#include <KDualAction>      //baseclass
#include <KToggleAction>    //baseclass
#include <KActionCollection> //convenience

namespace Dragon
{
KActionCollection *actionCollection(); ///defined in mainWindow.cpp, part.cpp
QAction *action( const char* ); ///defined in mainWindow.cpp, part.cpp
inline KToggleAction *toggleAction( const char *name ) { return (KToggleAction*)action( name ); }

class PlayAction : public KDualAction
{
    Q_OBJECT
public:
    PlayAction( QObject *receiver, const char *slot, KActionCollection* );
    void setPlaying( bool playing );
};

class VolumeAction : public KToggleAction
{
    Q_OBJECT
public:
    VolumeAction( QObject *receiver, const char *slot, KActionCollection* );
private Q_SLOTS:
    void mutedChanged( bool );
};
}

#endif

