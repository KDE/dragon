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

#include "playerDbusHandler.h"

#include "codeine.h"
#include "extern.h"
#include "mainWindow.h"
#include "theStream.h"
#include "videoWindow.h"

#include "playeradaptor.h" //from builddir

PlayerDbusHandler::PlayerDbusHandler(QObject *parent)
    : QObject(parent),
      m_lastEmittedState(Mpris::Status::Stopped)
{
    QObject* pa = new MediaPlayerAdaptor( this );
    setObjectName("PlayerDbusHandler");

    // the presence of media is reflected in the caps:
    connect( Dragon::engine(), SIGNAL( currentSourceChanged( Phonon::MediaSource ) ), this, SLOT( capsChangeSlot() )  );
    // the seekable status is reflected in the caps:
    connect( Dragon::engine(), SIGNAL( seekableChanged( bool ) ), this, SLOT( capsChangeSlot() )  );
    connect( this, SIGNAL( CapsChange( int ) ), pa, SIGNAL( CapsChange( int ) ) );

    connect( Dragon::engine(), SIGNAL( stateUpdated( Phonon::State, Phonon::State ) ), this, SLOT( statusChangeSlot( Phonon::State ) )  );
    connect( this, SIGNAL( StatusChange( Mpris::Status ) ), pa, SIGNAL( StatusChange( Mpris::Status ) ) );

    connect( Dragon::engine(), SIGNAL( metaDataChanged() ), this, SLOT( metadataChangeSlot() )  );
    connect( this, SIGNAL( TrackChange( QVariantMap ) ), pa, SIGNAL( TrackChange( QVariantMap ) ) );

    QDBusConnection::sessionBus().registerObject("/Player", this);
}

PlayerDbusHandler::~PlayerDbusHandler()
{

}
//from the first integer of http://wiki.xmms2.xmms.se/index.php/MPRIS#GetStatus
//0 = Playing, 1 = Paused, 2 = Stopped.
Mpris::Status
PlayerDbusHandler::GetStatus()
{
    Phonon::State state = Dragon::engine()->state();
    return Mpris::Status( PhononStateToMprisState( state ) );
}

void
PlayerDbusHandler::PlayPause()
{
    static_cast<Dragon::MainWindow*>( Dragon::mainWindow() )->play();
}

void
PlayerDbusHandler::Repeat(bool on)
{
    Q_UNUSED(on);
    // no-op
}

void
PlayerDbusHandler::Next()
{
    // no-op
}

void
PlayerDbusHandler::Prev()
{
    // no-op
}

void
PlayerDbusHandler::Pause()
{
    Dragon::engine()->pause();
}

void
PlayerDbusHandler::Play()
{
    Dragon::engine()->play();
}

//position is specified in milliseconds
int
PlayerDbusHandler::PositionGet()
{
    return static_cast<int>( Dragon::engine()->currentTime() );
}

void
PlayerDbusHandler::PositionSet( int time )
{
    Dragon::engine()->seek( time );
}

void
PlayerDbusHandler::Stop()
{
    Dragon::engine()->stop();
}

int
PlayerDbusHandler::VolumeGet()
{
    return static_cast<int>( Dragon::engine()->volume() * 100.0 );
}

void
PlayerDbusHandler::VolumeSet( int vol )
{
    Dragon::engine()->setVolume( vol / 100.0 );
}

//see http://wiki.xmms2.xmms.se/index.php/MPRIS_Metadata
QVariantMap
PlayerDbusHandler::GetMetadata()
{
    QVariantMap ret;
    QMultiMap<QString, QString> stringMap = Dragon::engine()->metaData();
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
    ret[ "location" ] = QVariant( Dragon::engine()->urlOrDisc() );
    return ret;
}

int
PlayerDbusHandler::GetCaps()
{
    int caps = Mpris::NO_CAPS;
    if( Dragon::TheStream::hasMedia() )
    {
        caps |= Mpris::CAN_PAUSE;
        caps |= Mpris::CAN_PLAY;
    }
    if( Dragon::engine()->isSeekable() )
        caps |= Mpris::CAN_SEEK;
    caps |= Mpris::CAN_PROVIDE_METADATA; //though it might be empty...
    return caps;
}

void
PlayerDbusHandler::capsChangeSlot()
{
    emit CapsChange( GetCaps() );
}

void
PlayerDbusHandler::statusChangeSlot( Phonon::State state )
{
    Mpris::Status::PlayMode newState = PhononStateToMprisState( state );
    if ( newState != m_lastEmittedState )
    {
        emit StatusChange( newState );
        m_lastEmittedState = newState;
    }
}

void
PlayerDbusHandler::metadataChangeSlot()
{
    emit TrackChange( GetMetadata() );
}

Mpris::Status::PlayMode
PlayerDbusHandler::PhononStateToMprisState( Phonon::State state )
{
    switch( state )
    {
        case Phonon::PlayingState:
            return Mpris::Status::Playing;
        case Phonon::PausedState:
            return Mpris::Status::Paused;
        default:
            return Mpris::Status::Stopped;
    }
}

#include "playerDbusHandler.moc"
