/***********************************************************************
 * Copyright 2005  Max Howell <max.howell@methylblue.com>
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

#ifndef DRAGONPLAYER_MESSAGEBOX
#define DRAGONPLAYER_MESSAGEBOX

#include <KMessageBox>
namespace Dragon {
    static class VideoWindow* videoWindow();
    namespace MessageBox
    {
        static inline void error( const QString &message )
        {
            KMessageBox::error( (QWidget*)videoWindow(), message );
        }
    
        static inline void sorry( const QString &message )
        {
            KMessageBox::error( (QWidget*)videoWindow(), message );
        }
    
        static inline void information( const QString &message, const QString &title )
        {
            KMessageBox::information( (QWidget*)videoWindow(), message, title );
        }
    }
}
#endif
