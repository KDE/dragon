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

#include "bluRayProbe.h"

#include <KDebug>

#include <QtCore/QFile>
#include <QtCore/QStringBuilder>

#include <solid/storageaccess.h>

namespace Dragon {

BluRayProbe::BluRayProbe(Solid::Device &device, QObject *parent)
    : QObject(parent)
    , m_device(device)
    , m_storage(device.as<Solid::StorageAccess>())
{
}

void BluRayProbe::probe()
{
    kDebug() << "BR: BluRayRom detected, using mount probe.";
    m_wasAccessible = true;
    if (!m_storage->isAccessible()) {
        kDebug() << "BR: Not mounted yet -> trying to mount.";
        m_wasAccessible = false;
        connect(m_storage, SIGNAL(setupDone(Solid::ErrorType,QVariant,QString)),
                this, SLOT(setupDone(Solid::ErrorType,QVariant,QString)));
        if (!m_storage->setup()) {
            kDebug() << "BR: mount failed.";
            disconnect(m_storage);
            return;
        }
        return;
    }
    emit foundBluRayVideo(m_device);
}

void BluRayProbe::setupDone(Solid::ErrorType error, QVariant /*errorData*/, const QString &udi)
{
    if (error != Solid::NoError) {
        kDebug() << "BR: mount failed (solid setup failed).";
        emit foundNoBluRayVideo();
        return;
    }

    QString bdmvPath = m_storage->filePath() % QLatin1Char('/') % QLatin1Literal("BDMV");
    kDebug() << "BR: checking for BDMV at" << bdmvPath;
    if (QFile(bdmvPath).exists()) {
        kDebug() << "BR: BDMV found, marking playable.";
        emit foundBluRayVideo(m_device);
    } else {
        kDebug() << "BR: BDMV not found.";
        if (!m_wasAccessible) {
            kDebug() << "BR: unmounting again.";
            m_storage->teardown();
        }
        emit foundNoBluRayVideo();
    }

    deleteLater();
}

} // namespace Dragon
