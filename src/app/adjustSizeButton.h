/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef DRAGONPLAYER_ADJUST_SIZE_BUTTON_H
#define DRAGONPLAYER_ADJUST_SIZE_BUTTON_H

#include <QFrame>

class QEvent;
class QTimerEvent;
class QPushButton;

namespace Dragon
{
class AdjustSizeButton : public QFrame
{
    Q_OBJECT
    int m_counter;
    int m_stage;
    int m_offset;
    int m_timerId;

    QPushButton *m_preferred;
    QPushButton *m_oneToOne;

    QFrame *m_thingy;

public:
    explicit AdjustSizeButton( QWidget *parent );

private:
    void timerEvent( QTimerEvent* ) override;
    bool eventFilter( QObject*, QEvent* ) override;

    inline void move()
    {
        QWidget::move( parentWidget()->width() - width(), parentWidget()->height() - m_offset );
    }
};
}

#endif
