/***********************************************************************
 * Copyright 2005  Max Howell <max.howell@methylblue.com>
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
#include "actions.h"
#include "debug.h"

#include <QToolButton>
#include <KLocale>

#include "videoWindow.h"

namespace Codeine
{
    PlayAction::PlayAction( QObject *receiver, const char *slot, KActionCollection *ac )
            : KToggleAction( i18n("Play"), ac )
     {
          setObjectName( "play" );
          setIcon( KIcon( "media-playback-start" ) );
          setShortcut( Qt::Key_Space );
          ac->addAction( objectName(), this );
          connect( this, SIGNAL( triggered( bool ) ), receiver, slot );
     }

     void PlayAction::setPlaying( bool playing )
     {
        if( playing )
        {
            setIcon( KIcon( "media-playback-pause" ) );
            setText( i18n("&Pause") );
        }
        else 
        {
            setIcon( KIcon( "media-playback-start" ) );
            setText( i18n("&Play") );
        }
     }

    void
    PlayAction::setChecked( bool b )
    {
        if( videoWindow()->state() == Engine::Empty && sender() && QByteArray( sender()->metaObject()->className() ) == "KToolBarButton" ) {
            // clicking play when empty means open PlayMediaDialog, but we have to uncheck the toolbar button
            // as KDElibs sets that checked automatically..
            setChecked( false );
        }
        else
            KToggleAction::setChecked( b );
    }
}

#include "actions.moc"
