/*
    SPDX-FileCopyrightText: 2012 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYER_MEDIAPLAYER2PLAYER_H
#define DRAGONPLAYER_MEDIAPLAYER2PLAYER_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

#include <Phonon/MediaSource>

class MediaPlayer2Player : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player") // Docs: https://specifications.freedesktop.org/mpris-spec/latest/Player_Interface.html

    Q_PROPERTY(QString PlaybackStatus READ PlaybackStatus)
    Q_PROPERTY(QString LoopStatus READ LoopStatus WRITE setLoopStatus)
    Q_PROPERTY(double Rate READ Rate WRITE setRate)
    Q_PROPERTY(bool Shuffle READ Shuffle WRITE setShuffle)
    Q_PROPERTY(QVariantMap Metadata READ Metadata)
    Q_PROPERTY(double Volume READ Volume WRITE setVolume)
    Q_PROPERTY(qlonglong Position READ Position)
    Q_PROPERTY(double MinimumRate READ MinimumRate)
    Q_PROPERTY(double MaximumRate READ MaximumRate)
    Q_PROPERTY(bool CanGoNext READ CanGoNext)
    Q_PROPERTY(bool CanGoPrevious READ CanGoPrevious)
    Q_PROPERTY(bool CanPlay READ CanPlay)
    Q_PROPERTY(bool CanPause READ CanPause)
    Q_PROPERTY(bool CanSeek READ CanSeek)
    Q_PROPERTY(bool CanControl READ CanControl)

public:
    explicit MediaPlayer2Player(QObject* parent);
    ~MediaPlayer2Player() override;

    QString PlaybackStatus() const;
    QString LoopStatus() const;
    void setLoopStatus(const QString& loopStatus) const;
    double Rate() const;
    void setRate(double rate) const;
    bool Shuffle() const;
    void setShuffle(bool shuffle) const;
    QVariantMap Metadata() const;
    double Volume() const;
    void setVolume(double volume) const;
    qlonglong Position() const;
    double MinimumRate() const;
    double MaximumRate() const;
    bool CanGoNext() const;
    bool CanGoPrevious() const;
    bool CanPlay() const;
    bool CanPause() const;
    bool CanSeek() const;
    bool CanControl() const;

Q_SIGNALS:
    void Seeked(qlonglong Position) const;

public Q_SLOTS:
    void Next() const;
    void Previous() const;
    void Pause() const;
    void PlayPause() const;
    void Stop() const;
    void Play() const;
    void Seek(qlonglong Offset) const;
    void SetPosition(const QDBusObjectPath& TrackId, qlonglong Position) const;
    void OpenUri(QString Uri) const;

private Q_SLOTS:
    void tick(qint64 newPos);
    void emitMetadataChange() const;
    void currentSourceChanged() const;
    void stateUpdated() const;
    void totalTimeChanged() const;
    void seekableChanged(bool seekable) const;
    void volumeChanged() const;

private:
    qint64 oldPos;
};

#endif
