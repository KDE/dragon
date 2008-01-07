#ifndef TIMELABEL_H
#define TIMELABEL_H

#include <QLabel>

class TimeLabel : public QLabel
{
    Q_OBJECT
    public:
       explicit TimeLabel( QWidget *parent );
       virtual void mousePressEvent( QMouseEvent * );
       virtual void updateTime();
       enum TimeFormats { SHOW_REMAINING,SHOW_COMPLETED };
       TimeFormats timeFormat;
    public slots:
       void setCurrentTime( qint64 );
       void setTotalTime( qint64 );
    private:
       qint64 m_currentTime;
       qint64 m_totalTime;
};

#endif 
