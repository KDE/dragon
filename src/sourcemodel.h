// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2025 Harald Sitter <sitter@kde.org>

#pragma once

#include <QMediaPlayer>
#include <QQmlEngine>
#include <QRangeModel>
#include <QtQmlIntegration>

#include <KFileItem>
#include <KIO/ListJob>
#include <KIO/StatJob>

class SourceModel : public QRangeModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QMediaPlayer *player MEMBER m_player WRITE setPlayer NOTIFY playerChanged)
public:
    explicit SourceModel(QObject *parent = nullptr)
        : QRangeModel(&m_sources, parent)
    {
    }

    void setPlayer(QMediaPlayer *player)
    {
        qWarning() << "Setting player on SourceModel";
        if (m_player == player) {
            return;
        }
        disconnect(m_player);

        connect(player, &QMediaPlayer::playbackStateChanged, this, [](QMediaPlayer::PlaybackState state) {
            qWarning() << "Playback state changed:" << state;
        });

        connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
            qWarning() << "Media status changed:" << status;
            if (status == QMediaPlayer::EndOfMedia) {
                next();
            }
        });

        connect(player, &QMediaPlayer::errorOccurred, this, [](QMediaPlayer::Error error, const QString &errorString) {
            qWarning() << "Media player error:" << error << errorString;
        });

        m_player = player;
        setAnySource();
        Q_EMIT playerChanged();
    }

    Q_INVOKABLE QJSValue addSourcePromise(const QUrl &url)
    {
        // Unfortunately QML doesn't yet have Promise.withResolvers() so we need to take the long way around.
        const auto promiseAndResolvers = qmlEngine(this)->evaluate(
            "let resolve, reject; \
             const promise = new Promise((res, rej) => { \
                resolve = res; \
                reject = rej; \
             }); \
             [promise, resolve, reject]; \
        ");
        Q_ASSERT(promiseAndResolvers.isArray());
        const auto promise = promiseAndResolvers.property(0);
        const auto resolve = promiseAndResolvers.property(1);
        const auto reject = promiseAndResolvers.property(2);

        auto stat = KIO::stat(url);
        connect(stat, &KIO::StatJob::result, this, [this, stat, url, resolve, reject]() {
            if (stat->error()) {
                qWarning() << "Failed to stat for source addition:" << stat->errorString();
                reject.call(QJSValueList() << QJSValue(stat->errorString()));
                return;
            }

            if (!stat->statResult().isDir()) {
                addSource(url);
                resolve.call();
                return;
            }

            auto list = KIO::listRecursive(url, KIO::DefaultFlags, KIO::ListJob::ListFlag());
            connect(list, &KIO::ListJob::entries, this, [this, url]([[maybe_unused]] KJob *job, const KIO::UDSEntryList &entries) {
                for (const auto &entry : entries) {
                    KFileItem fileItem(entry, url, /* delay mimetype */ true, /* url is dir */ true);
                    if (fileItem.isDir()) {
                        continue; // skip dirs
                    }
                    const auto entryUrl = fileItem.url();
                    // TODO create addSources so this becomes more efficient
                    addSource(entryUrl);
                }
            });
            connect(list, &KIO::ListJob::result, this, [this, list, url, resolve, reject]() {
                if (list->error()) {
                    qWarning() << "Failed to list dir for source addition:" << list->errorString();
                    reject.call(QJSValueList() << QJSValue(list->errorString()));
                    return;
                }

                qWarning() << "Added source to SourceModel:" << url << rowCount();
                resolve.call();
            });
        });

        return promise;
    }

    Q_INVOKABLE void next()
    {
        static std::random_device randomDevice;
        static std::mt19937 seed(randomDevice());
        std::uniform_int_distribution<> distribution(0, m_sources.size());

        m_currentIndex = distribution(seed);
        m_player->setProperty("source", QUrl(m_sources.at(m_currentIndex)));
        // Must call play again after chaning source.
        m_player->play();
        return;

        // TODO impl modes

        if (m_currentIndex + 1 >= m_sources.size()) {
            qWarning() << "Reached end of source list, not advancing.";
            return;
        }

        m_currentIndex++;
        // Very careful. The source property is actually overridden in the QML MediaPlayer, it no longer directly
        // relates to setSource. While using setSource would result in playback, it'd mean qml has no idea what
        // the source is since its property reads a different internal variable.
        // So: use the property settter not the C++ function.
        m_player->setProperty("source", QUrl(m_sources.at(m_currentIndex)));
        // Must call play again after chaning source.
        m_player->play();
    }

Q_SIGNALS:
    void playerChanged();

private:
    void addSource(const QUrl &url)
    {
        const auto idx = m_sources.size();
        if (!insertRows(idx, /* count */ 1)) {
            qWarning() << "Failed to insert row for new source";
            return;
        }
        if (!setData(index(idx, /* column */ 0), QVariant::fromValue(url))) {
            qWarning() << "Failed to set data for new source";
            return;
        }

        setAnySource();

        qWarning() << "Added source to SourceModel:" << url << rowCount();
    }
    void setAnySource()
    {
        if (!m_player->source().isValid() && !m_sources.isEmpty()) {
            const auto source = QUrl(m_sources.at(m_currentIndex));
            m_player->setProperty("source", source);
        }
    }
    qsizetype m_currentIndex = 0;
    // TODO report bug about QString not being usable with disabled ascii
    // TODO report bug about row count not changing in qml
    QList<QUrl> m_sources;
    QMediaPlayer *m_player = nullptr;
    // TODO deal with fileopen for directories somehow?
};
