/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

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
