/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef TIMELABEL_H
#define TIMELABEL_H

#include <QLabel>

class TimeLabel : public QLabel
{
    Q_OBJECT
public:
    explicit TimeLabel( QWidget *parent );
    ~TimeLabel() override;
    void mousePressEvent( QMouseEvent * ) override;
    void updateTime();
    enum TimeFormats { SHOW_REMAINING, SHOW_COMPLETED };
public Q_SLOTS:
    void setCurrentTime( qint64 );
    void setTotalTime( qint64 );
private:
    TimeFormats m_timeFormat;
    qint64 m_currentTime;
    qint64 m_totalTime;
};

#endif
