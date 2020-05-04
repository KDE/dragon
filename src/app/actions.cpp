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

#include <QDebug>
#include <QIcon>

#include <KLocalizedString>
#include <KGuiItem>

#include "videoWindow.h"

Dragon::PlayAction::PlayAction( QObject *receiver, const char *slot, KActionCollection *ac )
    : KDualAction( ac )
{
    setObjectName( QLatin1String( "play" ) );

    setInactiveGuiItem(KGuiItem(i18nc("@action", "Play"), QStringLiteral("media-playback-start")));
    setActiveGuiItem(KGuiItem(i18nc("@action", "Pause"), QStringLiteral( "media-playback-pause")));
    setAutoToggle( false );
    ac->setDefaultShortcuts(this, QList<QKeySequence>() << Qt::Key_Space << Qt::Key_MediaPlay);
    ac->addAction( objectName(), this );
    connect( this, SIGNAL(triggered(bool)), receiver, slot );
}

void Dragon::PlayAction::setPlaying( bool playing )
{
    setActive( playing );
}

/////////////////////////////////////////////////////
///Codeine::VolumeAction
////////////////////////////////////////////////////
Dragon::VolumeAction::VolumeAction( QObject *receiver, const char *slot, KActionCollection *ac )
    : KToggleAction(i18nc("@option:check Volume of sound output", "Volume"), ac)
{
    setObjectName( QLatin1String( "volume" ) );
    setIcon( QIcon::fromTheme(QLatin1String( "player-volume" ) ) );
    ac->setDefaultShortcut(this, Qt::Key_V);
    ac->addAction( objectName(), this );
    connect( this, SIGNAL(triggered(bool)), receiver, slot );
    connect( engine(), SIGNAL(mutedChanged(bool)), this, SLOT(mutedChanged(bool)) );
}

void Dragon::VolumeAction::mutedChanged( bool mute )
{
    if ( mute )
        setIcon( QIcon::fromTheme( QLatin1String( "player-volume-muted" ) ) );
    else
        setIcon( QIcon::fromTheme( QLatin1String( "player-volume" ) ) );
}
