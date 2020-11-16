/*
    SPDX-FileCopyrightText: 2005 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2008 David Edmundson <kde@davidedmundson.co.uk>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "timeLabel.h"

#include <QFontDatabase>

#include <KConfigGroup>
#include <KSharedConfig>

TimeLabel::TimeLabel( QWidget *parent )
    : QLabel( QLatin1String( " 0:00:00 " ), parent )
    , m_currentTime( 0 )
{
    setFont( QFontDatabase::systemFont(QFontDatabase::FixedFont) );
    setAlignment( Qt::AlignCenter );
    setMinimumSize( sizeHint() );
    KConfigGroup config(KSharedConfig::openConfig(), "General" );
    m_timeFormat = static_cast<TimeFormats>( config.readEntry<int>( "TimeFormat", static_cast<int>( SHOW_COMPLETED ) ) );
}

TimeLabel::~TimeLabel()
{
    KConfigGroup config(KSharedConfig::openConfig(), "General" );
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
