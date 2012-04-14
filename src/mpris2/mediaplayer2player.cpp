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

#include "mediaplayer2player.h"
#include "mainWindow.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <KDebug>

MediaPlayer2Player::MediaPlayer2Player(QObject* parent) : QDBusAbstractAdaptor(parent)
{
    connect(Dragon::engine(), SIGNAL(tick(qint64)), this, SLOT(tick(qint64)));
    connect(Dragon::engine(), SIGNAL(currentSourceChanged(Phonon::MediaSource)), this, SLOT(currentSourceChanged(Phonon::MediaSource)));
    connect(Dragon::engine(), SIGNAL(stateUpdated(Phonon::State,Phonon::State)), this, SLOT(stateUpdated(Phonon::State,Phonon::State)));
    connect(Dragon::engine(), SIGNAL(totalTimeChanged(qint64)), this, SLOT(totalTimeChanged(qint64)));
    connect(Dragon::engine(), SIGNAL(seekableChanged(bool)), this, SLOT(seekableChanged(bool)));
    connect(Dragon::engine(), SIGNAL(volumeChanged(qreal)), this, SLOT(volumeChanged(qreal)));
}

MediaPlayer2Player::~MediaPlayer2Player()
{
}

bool MediaPlayer2Player::CanGoNext() const
{
    return false;
}

void MediaPlayer2Player::Next() const
{
}

bool MediaPlayer2Player::CanGoPrevious() const
{
    return false;
}

void MediaPlayer2Player::Previous() const
{
}

bool MediaPlayer2Player::CanPause() const
{
    return Dragon::engine()->state() != Phonon::ErrorState;
}

void MediaPlayer2Player::Pause() const
{
    Dragon::engine()->pause();
}

void MediaPlayer2Player::PlayPause() const
{
    Dragon::engine()->playPause();
}

void MediaPlayer2Player::Stop() const
{
    Dragon::engine()->stop();
}

bool MediaPlayer2Player::CanPlay() const
{
    return true;
}

void MediaPlayer2Player::Play() const
{
    Dragon::engine()->play();
}

void MediaPlayer2Player::SetPosition(const QDBusObjectPath& TrackId, qlonglong Position) const
{
    Q_UNUSED(TrackId)

     Dragon::engine()->seek(Position / 1000);
}

void MediaPlayer2Player::OpenUri(QString Uri) const
{
    static_cast<Dragon::MainWindow*>(Dragon::mainWindow())->open(KUrl(Uri));
}

QString MediaPlayer2Player::PlaybackStatus() const
{
    switch (Dragon::engine()->state()) {
        case (Phonon::PlayingState):
            return "Playing";
            break;
        case (Phonon::PausedState):
        case (Phonon::BufferingState):
            return "Paused";
            break;
        case (Phonon::StoppedState):
            return "Stopped";
            break;
        default:
            return "Stopped";
            break;
    }
}

QString MediaPlayer2Player::LoopStatus() const
{
    return "None";
}

void MediaPlayer2Player::setLoopStatus(const QString& loopStatus) const
{
    Q_UNUSED(loopStatus)
}

double MediaPlayer2Player::Rate() const
{
    return 1.0;
}

void MediaPlayer2Player::setRate(double rate) const
{
    Q_UNUSED(rate)
}

bool MediaPlayer2Player::Shuffle() const
{
    return false;
}

void MediaPlayer2Player::setShuffle(bool shuffle) const
{
    Q_UNUSED(shuffle)
}

QVariantMap MediaPlayer2Player::Metadata() const
{
    QVariantMap metaData;

    metaData["mpris:trackid"] = QVariant::fromValue<QDBusObjectPath>(QDBusObjectPath("/org/mpris/MediaPlayer2/Track/1"));

    QMultiMap<QString, QString> phononMetaData = Dragon::engine()->metaData();
    QMultiMap<QString, QString>::const_iterator i = phononMetaData.constBegin();

    while (i != phononMetaData.constEnd()) {
        if (i.key() == "ALBUM" || i.key() == "TITLE")
            metaData["xesam:" + i.key().toLower()] = i.value();
        else if (i.key() == "ARTIST" || i.key() == "GENRE")
            metaData["xesam:" + i.key().toLower()] = QStringList(i.value());

        ++i;
    }

    metaData["mpris:length"] = Dragon::engine()->length() * 1000;
    metaData["xesam:url"] = Dragon::engine()->urlOrDisc();

    return metaData;
}

double MediaPlayer2Player::Volume() const
{
    return Dragon::engine()->volume();
}

void MediaPlayer2Player::setVolume(double volume) const
{
    Dragon::engine()->setVolume(volume);
}

qlonglong MediaPlayer2Player::Position() const
{
    return Dragon::engine()->currentTime() * 1000;
}

double MediaPlayer2Player::MinimumRate() const
{
    return 1.0;
}

double MediaPlayer2Player::MaximumRate() const
{
    return 1.0;
}

bool MediaPlayer2Player::CanSeek() const
{
    return Dragon::engine()->isSeekable();
}

void MediaPlayer2Player::Seek(qlonglong Offset) const
{
    Dragon::engine()->seek(((Dragon::engine()->currentTime() * 1000) + Offset) / 1000);
}

bool MediaPlayer2Player::CanControl() const
{
    return true;
}

void MediaPlayer2Player::tick(qint64 newPos)
{
    signalPropertyChange("Position", Position());

    if (newPos - oldPos > Dragon::engine()->tickInterval() + 250 || newPos < oldPos)
        emit Seeked(newPos * 1000);

    oldPos = newPos;
}

void MediaPlayer2Player::currentSourceChanged(Phonon::MediaSource source) const
{
    Q_UNUSED(source)

    signalPropertyChange("Metadata", Metadata());
    signalPropertyChange("CanSeek", CanSeek());
}

void MediaPlayer2Player::stateUpdated(Phonon::State current, Phonon::State old) const
{
    Q_UNUSED(current)
    Q_UNUSED(old)

    signalPropertyChange("PlaybackStatus", PlaybackStatus());
    signalPropertyChange("CanPause", CanPause());
}

void MediaPlayer2Player::totalTimeChanged(qint64 time) const
{
    Q_UNUSED(time)

    signalPropertyChange("Metadata", Metadata());
}

void MediaPlayer2Player::seekableChanged(bool seekable) const
{
    signalPropertyChange("CanSeek", seekable);
}

void MediaPlayer2Player::volumeChanged(qreal newVol) const
{
    Q_UNUSED(newVol)

    signalPropertyChange("Volume", Volume());
}

void MediaPlayer2Player::signalPropertyChange(const QString& property, const QVariant& value) const
{
    QVariantMap properties;
    properties[property] = value;

    QDBusMessage msg = QDBusMessage::createSignal("/org/mpris/MediaPlayer2",
        "org.freedesktop.DBus.Properties", "PropertiesChanged" );

    QVariantList args;
    args << "org.mpris.MediaPlayer2.Player";
    args << properties;
    args << QStringList();

    msg.setArguments(args);

    QDBusConnection::sessionBus().send(msg);
}
