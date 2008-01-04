/***************************************************************************
 *   Copyright (C) 2004-5 The Amarok Developers                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.          *
 ***************************************************************************/

#include "timeLabel.h"
#include <QLabel>
#include <KGlobalSettings>

TimeLabel::TimeLabel( QWidget *parent ) : QLabel( " 0:00:00 ", parent )
{
    setFont( KGlobalSettings::fixedFont() );
    setAlignment( Qt::AlignCenter );
    setMinimumSize( sizeHint() );
    currentTime=0;
}

void
TimeLabel::mousePressEvent( QMouseEvent * )
{
    if(timeFormat==SHOW_REMAINING)
        timeFormat=SHOW_COMPLETED;
    else
        timeFormat=SHOW_REMAINING;
    updateTime();
}

void
TimeLabel::updateTime()
{
    qint64 ms;
    #define zeroPad( n ) n < 10 ? QString("0%1").arg( n ) : QString::number( n )
    if(timeFormat==SHOW_REMAINING)
        ms=totalTime-currentTime;
    else
        ms=currentTime;
    const int s  = ms / 1000;
    const int m  =  s / 60;
    const int h  =  m / 60;
    QString time = zeroPad( s % 60 ); //seconds
    time.prepend( ':' );
    time.prepend( zeroPad( m % 60 ) ); //minutes
    time.prepend( ':' );
    time.prepend( QString::number( h ) ); //hours
    if(timeFormat==SHOW_REMAINING)
        time.prepend('-');
    setText( time );
}

void
TimeLabel::newCurrentTime(qint64 time)
{
    currentTime=time;
    updateTime();
}

void
TimeLabel::newTotalTime(qint64 time)
{
    totalTime=time;
    updateTime();
}
