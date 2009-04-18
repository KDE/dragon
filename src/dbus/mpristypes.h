/*
 * Copyright 2009  Alex Merry <alex.merry@kdemail.net>
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
#ifndef MPRISDEFS_H
#define MPRISDEFS_H

#include <QDBusArgument>

enum MprisCaps {
    NO_CAPS               = 0,
    CAN_GO_NEXT           = 1 << 0,
    CAN_GO_PREV           = 1 << 1,
    CAN_PAUSE             = 1 << 2,
    CAN_PLAY              = 1 << 3,
    CAN_SEEK              = 1 << 4,
    CAN_PROVIDE_METADATA  = 1 << 5,
    CAN_HAS_TRACKLIST     = 1 << 6,
    UNKNOWN_CAP           = 1 << 7
};


struct MprisSpecVersion
{
    quint16 major;
    quint16 minor;
};

Q_DECLARE_METATYPE(MprisSpecVersion)

// Marshall the MprisSpecVersion data into a D-BUS argument
QDBusArgument &operator<<(QDBusArgument &argument, const MprisSpecVersion &version);
// Retrieve the MprisSpecVersion data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, MprisSpecVersion &version);


struct MprisStatus
{
    enum PlayMode {
        Playing = 0,
        Paused = 1,
        Stopped = 2
    };

    enum RandomMode {
        Linear = 0,
        Random = 1
    };

    enum TrackRepeatMode {
        GoToNext = 0,
        RepeatCurrent = 1
    };

    enum PlaylistRepeatMode {
        StopWhenFinished = 0,
        PlayForever = 1
    };

    MprisStatus(PlayMode _play = Stopped,
                    RandomMode _random = Linear,
                    TrackRepeatMode _trackRepeat = GoToNext,
                    PlaylistRepeatMode _playlistRepeat = StopWhenFinished)
        : play(_play),
          random(_random),
          trackRepeat(_trackRepeat),
          playlistRepeat(_playlistRepeat)
    {
    }
    PlayMode           play;
    RandomMode         random;
    TrackRepeatMode    trackRepeat;
    PlaylistRepeatMode playlistRepeat;
};

Q_DECLARE_METATYPE(MprisStatus)

// Marshall the MprisStatus data into a D-BUS argument
QDBusArgument &operator<<(QDBusArgument &argument, const MprisStatus &status);
// Retrieve the MprisStatus data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, MprisStatus &status);

#endif // MPRISDEFS_H
