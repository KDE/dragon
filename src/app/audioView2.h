#ifndef DRAGON_AUDIOVIEW2_H
#define DRAGON_AUDIOVIEW2_H

#include <QWidget>

namespace Dragon {

namespace Ui {
class AudioView2;
}

class AudioView2 : public QWidget
{
    Q_OBJECT
    
public:
    explicit AudioView2(QWidget *parent = 0);
    ~AudioView2();

public slots:
    void enableDemo(bool enable);
    void update();
    
protected:
    void changeEvent(QEvent *e);
    
private:
    Ui::AudioView2 *ui;
};


} // namespace Dragon
#endif // DRAGON_AUDIOVIEW2_H
