/***********************************************************************
 * Copyright 2004  Max Howell <max.howell@methylblue.com>
 *           2007  Ian Monroe <ian@monroe.nu>
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

#ifndef CODEINELISTVIEW_CPP
#define CODEINELISTVIEW_CPP

#include <k3listview.h>

namespace Codeine
{
    class ListView : public K3ListView
    {
    public:
        ListView( QWidget *parent ) : K3ListView( parent )
        {
            addColumn( QString::null, 0 );
            addColumn( QString::null );

            setResizeMode( LastColumn );
            setMargin( 2 );
            setSorting( -1 );
            setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
            setAllColumnsShowFocus( true );
            setItemMargin( 3 );
        }

        virtual QSize sizeHint() const
        {
            const QSize sh = K3ListView::sizeHint();

            return QSize( sh.width(),
                childCount() == 0
                    ? 50
                    : qMin( sh.height(), childCount() * (firstChild()->height()) + margin() * 2 + 4 + reinterpret_cast<QWidget*>(header())->height() ) );
        }
    };
}

#endif
