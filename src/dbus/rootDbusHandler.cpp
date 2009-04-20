/***********************************************************************
 * Copyright 2008  Ian Monroe <ian@monroe.nu>
 * Copyright 2009  Alex Merry <alex.merry@kdemail.net>
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

#include "rootDbusHandler.h"

#include "debug.h"
#include "RootDbusHandlerBase.h"

#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>

RootDbusHandler::RootDbusHandler( QObject* parent )
    : QObject( parent )
{
    new RootDbusHandlerBase( this );
    setObjectName("RootDbusHandler");
    bool successful = QDBusConnection::sessionBus().registerObject("/", this);
    debug() << "registering root? " << successful;
}

RootDbusHandler::~RootDbusHandler()
{ }

QString 
RootDbusHandler::Identity()
{
    const KAboutData* aboutData = KCmdLineArgs::aboutData();
    return QString( "%1 %2" ).arg( aboutData->productName(), aboutData->version() );
}

void
RootDbusHandler::Quit()
{
    kapp->closeAllWindows();
}

Mpris::Version
RootDbusHandler::MprisVersion()
{
    Mpris::Version version;
    version.major = 1;
    version.minor = 0;
    return version;
}

#include "rootDbusHandler.moc"
