/***********************************************************************
 * Copyright 2012 Harald Sitter <sitter@kde.org>
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

#include "discScanner.h"

#include <solid/opticaldisc.h>

#include "bluRayProbe.h"

namespace Dragon {

DiscScanner::DiscScanner(QObject *parent)
    : QObject(parent)
    , m_waitCount(0)
{
}

void DiscScanner::scan()
{
    foreach (Solid::Device device, Solid::Device::listFromType(Solid::DeviceInterface::OpticalDisc)) {
        const Solid::OpticalDisc *disc = device.as<const Solid::OpticalDisc>();
        if (disc) {
            if(disc->availableContent() & ( Solid::OpticalDisc::VideoDvd
                                            | Solid::OpticalDisc::VideoCd
                                            | Solid::OpticalDisc::SuperVideoCd
                                            | Solid::OpticalDisc::Audio
                                            | Solid::OpticalDisc::VideoBluRay))
                m_playableDevices << device;
            else if (disc->discType() == Solid::OpticalDisc::BluRayRom) {
                BluRayProbe *probe = new BluRayProbe(device, this); // Auto deletes.
                connect(probe, SIGNAL(foundBluRayVideo(Solid::Device&)),
                        this, SLOT(gotDevice(Solid::Device&)));
                ++m_waitCount;
                probe->probe();
            }
        }
    }
    tryEmit();
}

void DiscScanner::gotDevice(Solid::Device &device)
{
    m_playableDevices << device;
    --m_waitCount;
    tryEmit();
}

void DiscScanner::gotNoDevice()
{
    --m_waitCount;
    tryEmit();
}


inline void DiscScanner::tryEmit()
{
    if (m_waitCount == 0)
        emit detectedDevices(m_playableDevices);
}

} // namespace Dragon
