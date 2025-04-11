/*
    SPDX-FileCopyrightText: 2012 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYER_MPRIS2_H
#define DRAGONPLAYER_MPRIS2_H

#include <QAction>
#include <QMediaPlayer>
#include <QObject>
#include <QQmlParserStatus>
#include <QVariantMap>
#include <QtQmlIntegration>

class Mpris2 : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    QML_NAMED_ELEMENT(MPRIS2)

    Q_PROPERTY(QMediaPlayer *player MEMBER m_player NOTIFY playerChanged REQUIRED)

    Q_PROPERTY(QObject *quitAction READ quitAction CONSTANT)
    Q_PROPERTY(QObject *fullscreenAction READ fullscreenAction CONSTANT)
    Q_PROPERTY(QObject *raiseAction READ raiseAction CONSTANT)

public:
    explicit Mpris2(QObject *parent = nullptr);
    ~Mpris2() override;
    Q_DISABLE_COPY_MOVE(Mpris2)

    void classBegin() override;
    void componentComplete() override;

    static void signalPropertiesChange(const QObject *adaptor, const QVariantMap &properties);

    [[nodiscard]] QAction *quitAction() const
    {
        return m_quitAction.get();
    }

    [[nodiscard]] QAction *fullscreenAction() const
    {
        return m_fullscreenAction.get();
    }

    [[nodiscard]] QAction *raiseAction() const
    {
        return m_raiseAction.get();
    }

Q_SIGNALS:
    void playerChanged();

private:
    QMediaPlayer *m_player = nullptr;
    std::unique_ptr<QAction> m_quitAction;
    std::unique_ptr<QAction> m_fullscreenAction;
    std::unique_ptr<QAction> m_raiseAction;
};

#endif
