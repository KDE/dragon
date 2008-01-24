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


#include <KIcon>
#include <KLocale>

#include "videoWindow.h"

Codeine::PlayAction::PlayAction( QObject *receiver, const char *slot, KActionCollection *ac )
        : KToggleAction( i18n("Play"), ac )
{
    setObjectName( "play" );
    setIcon( KIcon( "media-playback-start" ) );
    setShortcut( Qt::Key_Space );
    ac->addAction( objectName(), this );
    connect( this, SIGNAL( triggered( bool ) ), receiver, slot );
}

void 
Codeine::PlayAction::setPlaying( bool playing )
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
Codeine::PlayAction::setChecked( bool b )
{
    if( videoWindow()->state() == Engine::Empty && sender() && QByteArray( sender()->metaObject()->className() ) == "KToolBarButton" ) {
        // clicking play when empty means open PlayMediaDialog, but we have to uncheck the toolbar button
        // as KDElibs sets that checked automatically..
        setChecked( false );
    }
    else
        KToggleAction::setChecked( b );
}
/////////////////////////////////////////////////////
///Codeine::VolumeAction
////////////////////////////////////////////////////
Codeine::VolumeAction::VolumeAction( QObject *receiver, const char *slot, KActionCollection *ac )
        : KToggleAction( i18n("Volume"), ac )
{
    setObjectName( "volume" );
    setIcon( KIcon( "player-volume" ) );
//    setShortcut( Qt::Key_Space );
    ac->addAction( objectName(), this );
    connect( this, SIGNAL( triggered( bool ) ), receiver, slot );
    connect( engine(), SIGNAL( mutedChanged( bool ) ), this, SLOT( mutedChanged( bool ) ) );
}

void
Codeine::VolumeAction::mutedChanged( bool mute )
{
    if( mute )
        setIcon( KIcon( "player-volume-muted" ) );
    else
        setIcon( KIcon( "player-volume" ) );
}

#include "actions.moc"
