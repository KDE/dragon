// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINE_VOLUME_ACTION_H
#define CODEINE_VOLUME_ACTION_H

#include <ktoggleaction.h>
class KActionCollection;
class VolumeSlider;

class VolumeAction : public KToggleAction
{
    Q_OBJECT

    QWidget *m_anchor;
    VolumeSlider *m_widget;

    virtual bool eventFilter( QObject *o, QEvent *e );

    virtual QWidget* createWidget( QWidget* parent );

private slots:
    void toggled( bool );
    void sliderMoved( int );
    void sliderReleased() { setChecked( false ); toggled( false ); }

public:
    VolumeAction( KToolBar *anchor, QObject *ac );
};

#endif
