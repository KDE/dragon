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
    template<class Receiver, class Func>
    inline PlayAction(const Receiver *receiver, Func slot, KActionCollection* ac)
        : PlayAction(ac)
    {
        connect(this, &QAction::triggered, receiver, slot);
    }
    void setPlaying( bool playing );
private:
    explicit PlayAction(KActionCollection* ac);
};

class VolumeAction : public KToggleAction
{
    Q_OBJECT
public:
    template<class Receiver, class Func>
    inline VolumeAction(const Receiver *receiver, Func slot, KActionCollection* ac)
        : VolumeAction(ac)
    {
        connect(this, &QAction::triggered, receiver, slot);
    }
private Q_SLOTS:
    void mutedChanged( bool );
private:
    explicit VolumeAction(KActionCollection* ac);
};
}

#endif

