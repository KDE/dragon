// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINE_VOLUME_ACTION_H
#define CODEINE_VOLUME_ACTION_H

#include <kactionclasses.h>

class VolumeAction : public KToggleAction
{
   Q_OBJECT

   QWidget *m_anchor;
   class VolumeSlider *m_widget;

   virtual bool eventFilter( QObject *o, QEvent *e );

   virtual int plug( QWidget*, int );

private slots:
   void toggled( bool );
   void sliderMoved( int );
   void sliderReleased() { setChecked( false ); toggled( false ); }

public:
   VolumeAction( KToolBar *anchor, KActionCollection *ac );
};

#endif
