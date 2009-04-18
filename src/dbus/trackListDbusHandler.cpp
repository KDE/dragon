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

#include "trackListDbusHandler.h"

#include "codeine.h"
#include "mainWindow.h"
#include "playerDbusHandler.h"
#include "theStream.h"
#include "TrackListDbusAdaptor.h"
#include "videoWindow.h"

#include <KUrl>

TrackListDbusHandler::TrackListDbusHandler(QObject *parent)
    : QObject(parent)
{
    QObject* pa = new TrackListDbusAdaptor( this );
    setObjectName("TrackListDbusHandler");

    connect( Dragon::engine(), SIGNAL( currentSourceChanged( Phonon::MediaSource ) ), this, SLOT( slotTrackChange() )  );
    connect( this, SIGNAL( TrackListChange( int ) ), pa, SIGNAL( TrackListChange( int ) ) );

    QDBusConnection::sessionBus().registerObject("/TrackList", this);
}

TrackListDbusHandler::~TrackListDbusHandler()
{  }

int
TrackListDbusHandler::AddTrack(const QString& url, bool playImmediately)
{
    if( playImmediately )
    {
        if ( static_cast<Dragon::MainWindow*>( Dragon::mainWindow() )->open( KUrl( url ) ) )
            return 0;
        else
            return -1;
    }
    else
        return -1;
}

void
TrackListDbusHandler::DelTrack(int)
{   }

int TrackListDbusHandler::GetCurrentTrack()
{
    return 0;
}

int TrackListDbusHandler::GetLength()
{
    if( Dragon::TheStream::hasMedia() )
        return 1;
    else
        return 0;
}

QVariantMap
TrackListDbusHandler::GetMetadata(int position)
{
    if( position == 0 )
    {
        return parent()->findChild<PlayerDbusHandler *>("PlayerDbusHandler")->GetMetadata();
    }
    else
        return QVariantMap();
}

void
TrackListDbusHandler::SetLoop(bool)
{  }

void
TrackListDbusHandler::SetRandom(bool)
{  }

void
TrackListDbusHandler::slotTrackChange()
{
    emit TrackListChange(GetLength());
}

#include "trackListDbusHandler.moc"
