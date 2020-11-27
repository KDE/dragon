/*
    SPDX-FileCopyrightText: 2012 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "mediaplayer2player.h"
#include "mainWindow.h"
#include "mpris2.h"
#include "videoWindow.h"

#include <QCryptographicHash>

static QByteArray makeTrackId(const QString& source)
{
    return QByteArray("/org/kde/") + APP_NAME + "/tid_" +
            QCryptographicHash::hash(source.toLocal8Bit(), QCryptographicHash::Sha1)
            .toHex();
}

MediaPlayer2Player::MediaPlayer2Player(QObject* parent) : QDBusAbstractAdaptor(parent)
{
    connect(Dragon::engine(), &Dragon::VideoWindow::tick, this, &MediaPlayer2Player::tick);
    connect(Dragon::engine(), &Dragon::VideoWindow::currentSourceChanged,
            this, &MediaPlayer2Player::currentSourceChanged);
    connect(Dragon::engine(), &Dragon::VideoWindow::metaDataChanged,
            this, &MediaPlayer2Player::emitMetadataChange);
    connect(Dragon::engine(), &Dragon::VideoWindow::stateUpdated,
            this, &MediaPlayer2Player::stateUpdated);
    connect(Dragon::engine(), &Dragon::VideoWindow::totalTimeChanged,
            this, &MediaPlayer2Player::emitMetadataChange);
    connect(Dragon::engine(), &Dragon::VideoWindow::seekableChanged,
            this, &MediaPlayer2Player::seekableChanged);
    connect(Dragon::engine(), &Dragon::VideoWindow::volumeChanged,
            this, &MediaPlayer2Player::volumeChanged);
}

MediaPlayer2Player::~MediaPlayer2Player()
{
}

bool MediaPlayer2Player::CanGoNext() const
{
    return Dragon::engine()->canGoNext();
}

void MediaPlayer2Player::Next() const
{
    Dragon::engine()->nextChapter();
}

bool MediaPlayer2Player::CanGoPrevious() const
{
    return Dragon::engine()->canGoPrev();
}

void MediaPlayer2Player::Previous() const
{
    Dragon::engine()->prevChapter();
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
    if (TrackId.path().toLocal8Bit() == makeTrackId(Dragon::engine()->urlOrDisc()))
        Dragon::engine()->seek(Position / 1000);
}

void MediaPlayer2Player::OpenUri(QString Uri) const
{
    static_cast<Dragon::MainWindow*>(Dragon::mainWindow())->open(QUrl(Uri));
}

QString MediaPlayer2Player::PlaybackStatus() const
{
    switch (Dragon::engine()->state()) {
    case (Phonon::PlayingState):
        return QStringLiteral("Playing");
        break;
    case (Phonon::PausedState):
    case (Phonon::BufferingState):
        return QStringLiteral("Paused");
        break;
    case (Phonon::StoppedState):
        return QStringLiteral("Stopped");
        break;
    default:
        return QStringLiteral("Stopped");
        break;
    }
}

QString MediaPlayer2Player::LoopStatus() const
{
    return QStringLiteral("None");
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

    switch (Dragon::engine()->mediaSourceType()) {
    case Phonon::MediaSource::Invalid:
    case Phonon::MediaSource::Empty:
        break;
    default:
        metaData = {
            { QStringLiteral("mpris:trackid"),
              QVariant::fromValue<QDBusObjectPath>(QDBusObjectPath(makeTrackId(Dragon::engine()->urlOrDisc()).constData())) },
            { QStringLiteral("mpris:length"), Dragon::engine()->length() * 1000 },
            { QStringLiteral("xesam:url"), Dragon::engine()->urlOrDisc() },
        };
    }

    QMultiMap<QString, QString> phononMetaData = Dragon::engine()->metaData();
    QMultiMap<QString, QString>::const_iterator i = phononMetaData.constBegin();

    while (i != phononMetaData.constEnd()) {
        if (i.key() == QLatin1String("ALBUM") || i.key() == QLatin1String("TITLE"))
            metaData[QLatin1String("xesam:") + i.key().toLower()] = i.value();
        else if (i.key() == QLatin1String("ARTIST") || i.key() == QLatin1String("GENRE"))
            metaData[QLatin1String("xesam:") + i.key().toLower()] = QStringList(i.value());

        ++i;
    }

    return metaData;
}

double MediaPlayer2Player::Volume() const
{
    return Dragon::engine()->volume();
}

void MediaPlayer2Player::setVolume(double volume) const
{
    Dragon::engine()->setVolume(qBound(qreal(0.0), qreal(volume), qreal(1.0)));
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
    if (newPos - oldPos > Dragon::engine()->tickInterval() + 250 || newPos < oldPos)
        Q_EMIT Seeked(newPos * 1000);

    oldPos = newPos;
}

void MediaPlayer2Player::emitMetadataChange() const
{
    const QVariantMap properties { { QStringLiteral("Metadata"), Metadata() } };
    Mpris2::signalPropertiesChange(this, properties);
}

void MediaPlayer2Player::currentSourceChanged() const
{
    const QVariantMap properties {
        { QStringLiteral("Metadata"), Metadata() },
        { QStringLiteral("CanSeek"), CanSeek() },
    };
    Mpris2::signalPropertiesChange(this, properties);
}

void MediaPlayer2Player::stateUpdated() const
{
    const QVariantMap properties {
        { QStringLiteral("PlaybackStatus"), PlaybackStatus() },
        { QStringLiteral("CanPause"), CanPause() },
    };
    Mpris2::signalPropertiesChange(this, properties);
}

void MediaPlayer2Player::totalTimeChanged() const
{
    const QVariantMap properties { { QStringLiteral("Metadata"), Metadata() } };
    Mpris2::signalPropertiesChange(this, properties);
}

void MediaPlayer2Player::seekableChanged(bool seekable) const
{
    const QVariantMap properties { { QStringLiteral("CanSeek"), seekable } };
    Mpris2::signalPropertiesChange(this, properties);
}

void MediaPlayer2Player::volumeChanged() const
{
    const QVariantMap properties { { QStringLiteral("Volume"), Volume() } };
    Mpris2::signalPropertiesChange(this, properties);
}
