/***********************************************************************
 * Copyright 2005  Max Howell <max.howell@methylblue.com>
 *           2008  David Edmundson <kde@davidedmundson.co.uk>
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

#include "timeLabel.h"
#include <QLabel>

#include <KConfigGroup>
#include <KGlobal>
#include <KGlobalSettings>

TimeLabel::TimeLabel( QWidget *parent )
    : QLabel( QLatin1String( " 0:00:00 " ), parent )
    , m_currentTime( 0 )
{
    setFont( KGlobalSettings::fixedFont() );
    setAlignment( Qt::AlignCenter );
    setMinimumSize( sizeHint() );
    KConfigGroup config = KGlobal::config()->group( "General" );
    m_timeFormat = static_cast<TimeFormats>( config.readEntry<int>( "TimeFormat", static_cast<int>( SHOW_COMPLETED ) ) );
}

TimeLabel::~TimeLabel()
{
    KConfigGroup config = KGlobal::config()->group( "General" );
    config.writeEntry( "TimeFormat", static_cast<int>( m_timeFormat ) );
}

void
TimeLabel::mousePressEvent( QMouseEvent * )
{
    if( m_timeFormat == SHOW_REMAINING )
        m_timeFormat = SHOW_COMPLETED;
    else
        m_timeFormat = SHOW_REMAINING;
    updateTime();
}

void
TimeLabel::updateTime()
{
    qint64 ms;
#define zeroPad( n ) n < 10 ? QString::fromLatin1("0%1").arg( n ) : QString::number( n )
    if( m_timeFormat == SHOW_REMAINING )
        ms = m_totalTime - m_currentTime;
    else
        ms = m_currentTime;
    const int s  = ms / 1000;
    const int m  =  s / 60;
    const int h  =  m / 60;
    QString time = zeroPad( s % 60 ); //seconds
    time.prepend( QLatin1Char( ':' ) );
    time.prepend( zeroPad( m % 60 ) ); //minutes
    time.prepend( QLatin1Char( ':' ) );
    time.prepend( QString::number( h ) ); //hours
    if( m_timeFormat == SHOW_REMAINING )
        time.prepend(QLatin1Char( '-' ));
    setText( time );
}

void
TimeLabel::setCurrentTime( qint64 time )
{
    m_currentTime = time;
    updateTime();
}

void
TimeLabel::setTotalTime( qint64 time )
{
    m_totalTime = time;
    updateTime();
}

#include "timeLabel.moc"
