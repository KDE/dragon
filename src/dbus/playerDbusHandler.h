#ifndef CODEINE_PLAYERDBUSHANDLER_H
#define CODEINE_PLAYERDBUSHANDLER_H

#include <QObject>

class PlayerDbusHandler : public QObject
{
public:
    PlayerDbusHandler(QObject *parent);
    virtual ~PlayerDbusHandler();
    enum DbusStatus { Playing = 0, Paused = 1, Stopped = 2 };

public slots:
    int GetStatus();
    void Load(const QString &in0);
    void Pause();
    void Play();
    void PlayPause();
    int PositionGet();
    void PositionSet(int in0);
    void Stop();
    int VolumeGet();
    void VolumeSet(int in0);
signals:
    void StatusChange(int in0);
    void TrackChange(const QString &in0);
};

#endif
