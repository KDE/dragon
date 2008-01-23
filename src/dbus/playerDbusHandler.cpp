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

#include "playerDbusHandler.h"

#include "codeine.h"
#include "extern.h"
#include "mainWindow.h"
#include "videoWindow.h"

#include "playeradaptor.h" //from builddir

PlayerDbusHandler::PlayerDbusHandler(QObject *parent)
    : QObject(parent)
{
    QObject* pa = new MediaPlayerAdaptor( this );
    connect( Codeine::mainWindow(), SIGNAL( fileChanged( QString ) ), pa, SIGNAL( TrackChange( QString ) ) );
    connect( Codeine::mainWindow(), SIGNAL( dbusStatusChanged( int ) ), pa, SIGNAL( StatusChange( int ) ) );

    connect( Codeine::engine(), SIGNAL( seekableChanged( bool ) ), this, SLOT( capsChangeSlot() )  );
    connect( this, SIGNAL( CapsChange( int ) ), pa, SIGNAL( CapsChange( int ) ) );

    QDBusConnection::sessionBus().registerObject("/Player", this);
}

PlayerDbusHandler::~PlayerDbusHandler()
{

}
//from the first integer of http://wiki.xmms2.xmms.se/index.php/MPRIS#GetStatus
//0 = Playing, 1 = Paused, 2 = Stopped.
int
PlayerDbusHandler::GetStatus()
{
    Engine::State state = Codeine::engine()->state();
    if( state == Engine::Playing )
        return Playing;
    else if( state == Engine::Paused )
        return Paused;
    else
        return Stopped;
}

void
PlayerDbusHandler::Load(const QString &url)
{
    static_cast<Codeine::MainWindow*>( Codeine::mainWindow() )->open( KUrl( url ) );
}

void
PlayerDbusHandler::PlayPause()
{
    static_cast<Codeine::MainWindow*>( Codeine::mainWindow() )->play();
}

void
PlayerDbusHandler::Pause()
{
    Codeine::engine()->pause();
}

void
PlayerDbusHandler::Play()
{
    Codeine::engine()->play();
}

//position is specified in milliseconds
int
PlayerDbusHandler::PositionGet()
{
    return static_cast<int>( Codeine::engine()->currentTime() );
}

void
PlayerDbusHandler::PositionSet( int time )
{
    Codeine::engine()->seek( time );
}

void
PlayerDbusHandler::Stop()
{
    Codeine::engine()->stop();
}

int
PlayerDbusHandler::VolumeGet()
{
    return static_cast<int>( Codeine::engine()->volume() * 100.0 );
}

void
PlayerDbusHandler::VolumeSet( int vol )
{
    Codeine::engine()->setVolume( vol / 100.0 );
}

//see http://wiki.xmms2.xmms.se/index.php/MPRIS_Metadata
QVariantMap
PlayerDbusHandler::GetMetaData()
{
    QVariantMap ret;
    QMultiMap<QString, QString> stringMap = Codeine::engine()->metaData();
    QMultiMap<QString, QString>::const_iterator i = stringMap.constBegin();
    while( i != stringMap.constEnd() ) 
    {
        bool number = false;
        int value = i.value().toInt( &number );
        if( number && ( i.key().toLower() != "tracknumber" ) ) //tracknumber always string, according to MPRIS spec
            ret[ i.key().toLower() ] = value;
        else
            ret[ i.key().toLower() ] = QVariant( i.value() );
        ++i;
    }
    ret[ "location" ] = QVariant( Codeine::engine()->urlOrDisc() );
    return ret;
}

int
PlayerDbusHandler::GetCaps()
{
    int caps = NONE;
    if( static_cast<Codeine::MainWindow*>( Codeine::mainWindow() )->action("play")->isEnabled() )
    {
        caps |= CAN_PAUSE;
        caps |= CAN_PLAY;
    }
    if( Codeine::engine()->isSeekable() )
        caps |= CAN_SEEK;
    caps |= CAN_PROVIDE_METADATA; //though it might be empty...
    return caps;
}

void
PlayerDbusHandler::capsChangeSlot()
{
    emit CapsChange( GetCaps() );
}

#include "playerDbusHandler.moc"
