#ifndef TIMELABEL_H
#define TIMELABEL_H

#include <QLabel>

class TimeLabel : public QLabel
{
    Q_OBJECT
    public:
       explicit TimeLabel( QWidget *parent);
       virtual void mousePressEvent( QMouseEvent * );
       virtual void updateTime();
       enum TimeFormats {SHOW_REMAINING,SHOW_COMPLETED};
       TimeFormats timeFormat;

       virtual void newCurrentTime(qint64);
       virtual void newTotalTime(qint64);
    private:
       qint64 currentTime,totalTime;
};

#endif 
