/***********************************************************************
 * Copyright 2008  Ian Monroe <ian@monroe.nu>
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
