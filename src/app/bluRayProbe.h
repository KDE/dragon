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

#ifndef DRAGON_BLURAYPROBE_H
#define DRAGON_BLURAYPROBE_H

#include <QtCore/QObject>
#include <solid/device.h>
#include <solid/solidnamespace.h>

namespace Solid {
class Device;
class StorageAccess;
}

namespace Dragon {

class BluRayProbe : public QObject
{
    Q_OBJECT
public:
    explicit BluRayProbe(Solid::Device &device, QObject *parent = 0);

    void probe();

signals:
    void foundBluRayVideo(Solid::Device &device);
    void foundNoBluRayVideo();

private slots:
    void setupDone(Solid::ErrorType error, QVariant errorData, const QString &udi);

private:
    /** Was mounted before we did probing */
    bool m_wasAccessible;

    /** Probed device */
    Solid::Device m_device;

    /** Probed storage */
    Solid::StorageAccess *m_storage;
};

} // namespace Dragon

#endif // DRAGON_BLURAYPROBE_H
