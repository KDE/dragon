/***********************************************************************
 * Copyright 2012  Eike Hein <hein@kde.org>
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

#include "mediaplayer2.h"
#include "codeine.h"

#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>
#include <KProtocolInfo>
#include <KServiceTypeTrader>
#include <KWindowSystem>

#include <QWidget>

MediaPlayer2::MediaPlayer2(QObject* parent) : QDBusAbstractAdaptor(parent)
{
}

MediaPlayer2::~MediaPlayer2()
{
}

bool MediaPlayer2::CanRaise() const
{
    return true;
}

void MediaPlayer2::Raise() const
{
    Dragon::mainWindow()->raise();
    KWindowSystem::activateWindow(Dragon::mainWindow()->winId());
}

bool MediaPlayer2::CanQuit() const
{
    return true;
}

void MediaPlayer2::Quit() const
{
    kapp->closeAllWindows();
}

bool MediaPlayer2::HasTrackList() const
{
    return false;
}

QString MediaPlayer2::Identity() const
{
    return KCmdLineArgs::aboutData()->programName();
}

QString MediaPlayer2::DesktopEntry() const
{
    return QString(APP_NAME);
}

QStringList MediaPlayer2::SupportedUriSchemes() const
{
    QStringList protocols;

    foreach(const QString& protocol, KProtocolInfo::protocols())
        if (!KProtocolInfo::isHelperProtocol(protocol))
            protocols << protocol;

    return protocols;
}

QStringList MediaPlayer2::SupportedMimeTypes() const
{
    KService::Ptr app = KService::serviceByDesktopName(APP_NAME);

    if (app)
        // Unfortunately KServices cannot return just the MIME types but also includes
        // the "Application" service type, so we need to filter it out.
        return app->serviceTypes().filter(QRegExp("^(?!Application).*$"));

    return QStringList();
}
