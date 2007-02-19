// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINE_TOOLBAR_H
#define CODEINE_TOOLBAR_H

#include <ktoolbar.h>
//Added by qt3to4:
#include <QEvent>


class MouseOverToolBar : public KToolBar
{
   virtual bool eventFilter( QObject*, QEvent* );

public:
   MouseOverToolBar( QWidget *parent );
};

#endif
