/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "actions.h"
#include "theStream.h"

#include <QDebug>
#include <QIcon>

#include <KLocalizedString>
#include <KGuiItem>

#include "videoWindow.h"

Dragon::PlayAction::PlayAction(KActionCollection *ac)
    : KDualAction( ac )
{
    setObjectName( QLatin1String( "play" ) );

    setInactiveGuiItem(KGuiItem(i18nc("@action", "Play"), QStringLiteral("media-playback-start")));
    setActiveGuiItem(KGuiItem(i18nc("@action", "Pause"), QStringLiteral( "media-playback-pause")));
    setAutoToggle( false );
    ac->setDefaultShortcuts(this, QList<QKeySequence>() << Qt::Key_Space << Qt::Key_MediaPlay);
    ac->addAction( objectName(), this );
}

void Dragon::PlayAction::setPlaying( bool playing )
{
    setActive( playing );
}

/////////////////////////////////////////////////////
///Codeine::VolumeAction
////////////////////////////////////////////////////
Dragon::VolumeAction::VolumeAction(KActionCollection *ac)
    : KToggleAction(i18nc("@option:check Volume of sound output", "Volume"), ac)
{
    setObjectName( QLatin1String( "volume" ) );
    setIcon( QIcon::fromTheme(QLatin1String( "player-volume" ) ) );
    ac->setDefaultShortcut(this, Qt::Key_V);
    ac->addAction( objectName(), this );
    connect(engine(), &Dragon::VideoWindow::mutedChanged, this, &Dragon::VolumeAction::mutedChanged);
}

void Dragon::VolumeAction::mutedChanged( bool mute )
{
    if ( mute )
        setIcon( QIcon::fromTheme( QLatin1String( "player-volume-muted" ) ) );
    else
        setIcon( QIcon::fromTheme( QLatin1String( "player-volume" ) ) );
}
