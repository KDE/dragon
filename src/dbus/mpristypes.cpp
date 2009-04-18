/*
 * Copyright 2008  Alex Merry <alex.merry@kdemail.net>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
