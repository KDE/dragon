#ifndef CODEINE_PLAYERDBUSHANDLER_H
#define CODEINE_PLAYERDBUSHANDLER_H

#include <QObject>
#include <QVariantMap>

class PlayerDbusHandler : public QObject
{
Q_OBJECT
public:
    PlayerDbusHandler(QObject *parent);
    virtual ~PlayerDbusHandler();
    enum DbusStatus { Playing = 0, Paused = 1, Stopped = 2 };
    //http://wiki.xmms2.xmms.se/index.php/MPRIS#GetCaps
    enum DbusCaps {
         NONE                  = 0,
         //CAN_GO_NEXT           = 1 << 0, dragon player can never go next or previous
         //CAN_GO_PREV           = 1 << 1,
         CAN_PAUSE             = 1 << 2,
         CAN_PLAY              = 1 << 3,
         CAN_SEEK              = 1 << 4,
         CAN_PROVIDE_METADATA  = 1 << 5 };
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
    int GetCaps();
    QVariantMap GetMetaData();
signals:
    void CapsChange( int );
private slots:
    void capsChangeSlot();
};

#endif
