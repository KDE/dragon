/*
    SPDX-FileCopyrightText: 2004 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYERLISTVIEW_CPP
#define DRAGONPLAYERLISTVIEW_CPP

#include <KListWidget>

namespace Dragon
{
    class ListView : public KListWidget
    {
        public:
            ListView( QWidget *parent ) 
                : KListWidget( parent )
            {
        //       setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
                setAlternatingRowColors( true );
            }
    };
}

#endif
