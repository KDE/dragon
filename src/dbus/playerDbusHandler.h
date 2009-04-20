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

#ifndef DRAGONPLAYER_PLAYERDBUSHANDLER_H
#define DRAGONPLAYER_PLAYERDBUSHANDLER_H

#include <QObject>
#include <QVariantMap>
#include "mpristypes.h"
#include <phonon/phononnamespace.h>

class PlayerDbusHandler : public QObject
{
Q_OBJECT
public:
    PlayerDbusHandler(QObject *parent);
    virtual ~PlayerDbusHandler();
public slots:
    Mpris::Status GetStatus();
    void Next(); // no-op
    void Prev(); // no-op
    void Pause();
    void Play();
    void PlayPause();
    void Repeat(bool on); // no-op
    int PositionGet();
    void PositionSet( int msec );
    void Stop();
    int VolumeGet();
    void VolumeSet( int percent );
    int GetCaps();
    QVariantMap GetMetadata();
signals:
    void CapsChange( int );
    void StatusChange( Mpris::Status );
    void TrackChange( QVariantMap );
private slots:
    void capsChangeSlot();
    void statusChangeSlot( Phonon::State state );
    void metadataChangeSlot();
private:
    Mpris::Status::PlayMode PhononStateToMprisState( Phonon::State );
    Mpris::Status::PlayMode m_lastEmittedState;
};

#endif
