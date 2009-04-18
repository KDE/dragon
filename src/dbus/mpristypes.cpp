/*
 * Copyright 2008  Alex Merry <alex.merry@kdemail.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "mpristypes.h"

// Marshall the MprisSpecVersion data into a D-BUS argument
QDBusArgument &operator<<(QDBusArgument &argument, const MprisSpecVersion &version)
{
    argument.beginStructure();
    argument << version.major << version.minor;
    argument.endStructure();
    return argument;
}

// Retrieve the MprisSpecVersion data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, MprisSpecVersion &version)
{
    argument.beginStructure();
    argument >> version.major >> version.minor;
    argument.endStructure();
    return argument;
}

// Marshall the MprisStatus data into a D-BUS argument
QDBusArgument &operator<<(QDBusArgument &argument, const MprisStatus &status)
{
    argument.beginStructure();
    argument << (qint32)status.play;
    argument << (qint32)status.random;
    argument << (qint32)status.trackRepeat;
    argument << (qint32)status.playlistRepeat;
    argument.endStructure();
    return argument;
}

// Retrieve the MprisStatus data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, MprisStatus &status)
{
    qint32 play, random, trackRepeat, playlistRepeat;

    argument.beginStructure();
    argument >> play;
    argument >> random;
    argument >> trackRepeat;
    argument >> playlistRepeat;
    argument.endStructure();

    status.play = (MprisStatus::PlayMode)play;
    status.random = (MprisStatus::RandomMode)random;
    status.trackRepeat = (MprisStatus::TrackRepeatMode)trackRepeat;
    status.playlistRepeat = (MprisStatus::PlaylistRepeatMode)playlistRepeat;

    return argument;
}

// vim: sw=4 sts=4 et tw=100
