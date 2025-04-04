/*
    SPDX-FileCopyrightText: 2012 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "mediaplayer2player.h"
#include "mpris2.h"

#include <QAudioOutput>
#include <QCryptographicHash>
#include <QMediaMetaData>

using namespace Qt::StringLiterals;

static QByteArray makeTrackId(const QString &source)
{
    return QByteArray("/org/kde/dragonplayer") + "/tid_" + QCryptographicHash::hash(source.toLocal8Bit(), QCryptographicHash::Sha1).toHex();
}

MediaPlayer2Player::MediaPlayer2Player(QMediaPlayer *player, QObject *parent)
    : QDBusAbstractAdaptor(parent)
    , oldPos(0)
    , m_player(player)
{
    connect(m_player, &QMediaPlayer::positionChanged, this, &MediaPlayer2Player::tick);
    connect(m_player, &QMediaPlayer::sourceChanged, this, &MediaPlayer2Player::currentSourceChanged);
    connect(m_player, &QMediaPlayer::metaDataChanged, this, &MediaPlayer2Player::emitMetadataChange);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &MediaPlayer2Player::stateUpdated);
    connect(m_player, &QMediaPlayer::durationChanged, this, &MediaPlayer2Player::emitMetadataChange);
    connect(m_player, &QMediaPlayer::seekableChanged, this, &MediaPlayer2Player::seekableChanged);
    connect(m_player->audioOutput(), &QAudioOutput::volumeChanged, this, &MediaPlayer2Player::volumeChanged);
}

MediaPlayer2Player::~MediaPlayer2Player() = default;

bool MediaPlayer2Player::CanGoNext() const
{
    return false;
}

void MediaPlayer2Player::Next() const
{
    qWarning() << "Next() not supported";
}

bool MediaPlayer2Player::CanGoPrevious() const
{
    return false;
}

void MediaPlayer2Player::Previous() const
{
    qWarning() << "Previous() not supported";
}

bool MediaPlayer2Player::CanPause() const
{
    return m_player->error() == QMediaPlayer::NoError;
}

void MediaPlayer2Player::Pause() const
{
    m_player->pause();
}

void MediaPlayer2Player::PlayPause() const
{
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        m_player->pause();
    } else {
        m_player->play();
    }
}

void MediaPlayer2Player::Stop() const
{
    m_player->stop();
}

bool MediaPlayer2Player::CanPlay() const
{
    return true;
}

void MediaPlayer2Player::Play() const
{
    m_player->play();
}

void MediaPlayer2Player::SetPosition(const QDBusObjectPath &TrackId, qlonglong Position) const
{
    m_player->setPosition(Position);
}

void MediaPlayer2Player::OpenUri(QString Uri) const
{
    m_player->setSource(QUrl(Uri));
}

QString MediaPlayer2Player::PlaybackStatus() const
{
    switch (m_player->playbackState()) {
    case QMediaPlayer::PlayingState:
        return QStringLiteral("Playing");
        break;
    case QMediaPlayer::PausedState:
        return QStringLiteral("Paused");
        break;
    case QMediaPlayer::StoppedState:
        return QStringLiteral("Stopped");
        break;
    }
    return QStringLiteral("Stopped");
}

QString MediaPlayer2Player::LoopStatus() const
{
    return QStringLiteral("None");
}

void MediaPlayer2Player::setLoopStatus(const QString &loopStatus) const
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
    QVariantMap metaData{
        {QStringLiteral("mpris:trackid"), QVariant::fromValue<QDBusObjectPath>(QDBusObjectPath(makeTrackId(m_player->source().toString()).constData()))},
        {QStringLiteral("xesam:url"), QVariant::fromValue(m_player->source().toString())},
    };

    const auto data = m_player->metaData();
    const auto keys = data.keys();
    if (keys.contains(QMediaMetaData::Duration)) {
        metaData.insert(QStringLiteral("mpris:length"), data.value(QMediaMetaData::Duration).toLongLong());
    }

    // Fields where we don't need to perform any type conversions
    const std::map<QMediaMetaData::Key, QString> trivialData = {
        {QMediaMetaData::AlbumArtist, u"xesam:artist"_s},
        {QMediaMetaData::AlbumTitle, u"xesam:album"_s},
        {QMediaMetaData::Genre, u"xesam:genre"_s},
        {QMediaMetaData::TrackNumber, u"xesam:trackNumber"_s},
    };

    for (const auto &[key, value] : trivialData) {
        if (keys.contains(key)) {
            metaData.insert(value, data.value(key));
        }
    }

    return metaData;
}

double MediaPlayer2Player::Volume() const
{
    return m_player->audioOutput()->volume();
}

void MediaPlayer2Player::setVolume(double volume) const
{
    m_player->audioOutput()->setVolume(qBound(0.0F, float(volume), 1.0F));
}

qlonglong MediaPlayer2Player::Position() const
{
    return m_player->position();
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
    return m_player->isSeekable();
}

void MediaPlayer2Player::Seek(qlonglong Offset) const
{
    m_player->setPosition(m_player->position() + Offset);
}

bool MediaPlayer2Player::CanControl() const
{
    return true;
}

void MediaPlayer2Player::tick(qint64 newPos)
{
    Q_EMIT Seeked(newPos);
}

void MediaPlayer2Player::emitMetadataChange() const
{
    const QVariantMap properties{{QStringLiteral("Metadata"), Metadata()}};
    Mpris2::signalPropertiesChange(this, properties);
}

void MediaPlayer2Player::currentSourceChanged() const
{
    const QVariantMap properties{
        {QStringLiteral("Metadata"), Metadata()},
        {QStringLiteral("CanSeek"), CanSeek()},
    };
    Mpris2::signalPropertiesChange(this, properties);
}

void MediaPlayer2Player::stateUpdated() const
{
    const QVariantMap properties{
        {QStringLiteral("PlaybackStatus"), PlaybackStatus()},
        {QStringLiteral("CanPause"), CanPause()},
    };
    Mpris2::signalPropertiesChange(this, properties);
}

void MediaPlayer2Player::totalTimeChanged() const
{
    const QVariantMap properties{{QStringLiteral("Metadata"), Metadata()}};
    Mpris2::signalPropertiesChange(this, properties);
}

void MediaPlayer2Player::seekableChanged(bool seekable) const
{
    const QVariantMap properties{{QStringLiteral("CanSeek"), seekable}};
    Mpris2::signalPropertiesChange(this, properties);
}

void MediaPlayer2Player::volumeChanged() const
{
    const QVariantMap properties{{QStringLiteral("Volume"), Volume()}};
    Mpris2::signalPropertiesChange(this, properties);
}

#include "moc_mediaplayer2player.cpp"
