#ifndef AUDIOVIEW_H
#define AUDIOVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <Phonon/MediaObject>

using Phonon::MediaObject;

namespace Codeine
{

class AudioView : public QWidget
{
    Q_OBJECT
    public:
       explicit AudioView(QWidget *parent);
       virtual ~AudioView();
       virtual void paintEvent(QPaintEvent* event);
       virtual void updateText();
    private:
       QString m_message;
};

}
#endif 
