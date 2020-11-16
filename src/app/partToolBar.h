/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYER_PARTTOOLBAR_H
#define DRAGONPLAYER_PARTTOOLBAR_H

#include <KToolBar>


class MouseOverToolBar : public KToolBar
{
    bool eventFilter( QObject*, QEvent* ) override;

public:
    explicit MouseOverToolBar( QWidget *parent );
};

#endif
