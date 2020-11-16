/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYER_FULLSCREENTOOLBARHANDLER_H
#define DRAGONPLAYER_FULLSCREENTOOLBARHANDLER_H

#include <QObject>
#include <QPoint>

class KMainWindow;
class QTimerEvent;

namespace Dragon 
{
class FullScreenToolBarHandler : QObject
{
    Q_OBJECT
public:
    explicit FullScreenToolBarHandler(KMainWindow *parent );
    bool eventFilter( QObject *o, QEvent *e ) override;
    void timerEvent( QTimerEvent* ) override;
private:
    int m_timer_id; // 0 when timer is not running
    QPoint m_home;
    KMainWindow *m_parent;
};
}
#endif
