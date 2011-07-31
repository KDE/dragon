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
#include "theStream.h"

#include <KDebug>
#include <KIcon>
#include <KLocale>

#include "videoWindow.h"

Dragon::PlayAction::PlayAction( QObject *receiver, const char *slot, KActionCollection *ac )
        : KDualAction( ac )
{
    setObjectName( QLatin1String( "play" ) );

    setInactiveGuiItem( KGuiItem( i18n( "Play" ), KIcon( QLatin1String( "media-playback-start" ) ) ) );
    setActiveGuiItem( KGuiItem( i18n( "Pause" ), KIcon( QLatin1String( "media-playback-pause" ) ) ) );
    setAutoToggle( false );
    setShortcut( Qt::Key_Space );
    ac->addAction( objectName(), this );
    connect( this, SIGNAL(triggered(bool)), receiver, slot );
}

void 
Dragon::PlayAction::setPlaying( bool playing )
{
    setActive( playing );
}

/////////////////////////////////////////////////////
///Codeine::VolumeAction
////////////////////////////////////////////////////
Dragon::VolumeAction::VolumeAction( QObject *receiver, const char *slot, KActionCollection *ac )
        : KToggleAction( i18nc( "Volume of sound output", "Volume"), ac )
{
    setObjectName( QLatin1String( "volume" ) );
    setIcon( KIcon( QLatin1String(  "player-volume" ) ) );
    setShortcut( Qt::Key_V );
    ac->addAction( objectName(), this );
    connect( this, SIGNAL(triggered(bool)), receiver, slot );
    connect( engine(), SIGNAL(mutedChanged(bool)), this, SLOT(mutedChanged(bool)) );
}

void
Dragon::VolumeAction::mutedChanged( bool mute )
{
    if( mute )
        setIcon( KIcon( QLatin1String(  "player-volume-muted" ) ) );
    else
        setIcon( KIcon( QLatin1String(  "player-volume" ) ) );
}

#include "actions.moc"
