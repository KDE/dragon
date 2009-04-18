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
