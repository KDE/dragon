// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINE_FULLSCREENACTION_H
#define CODEINE_FULLSCREENACTION_H

#include <ktoggleaction.h>

class KActionCollection;

/**
 * @class FullSCreenAction
 * @author Ian Monroe
 * @short Just calls Phonon::VideoWidget->setFullScreen essentionally
 */
class FullScreenAction : public KToggleAction
{
Q_OBJECT
public:
    FullScreenAction( QWidget *window, KActionCollection* );

/*    virtual void setChecked( bool );
    virtual void setEnabled( bool );

protected:
    virtual bool eventFilter( QObject* o, QEvent* e );

private:
    QWidget *m_window;
    bool m_shouldBeDisabled;
    unsigned long m_state; */
};

#endif
