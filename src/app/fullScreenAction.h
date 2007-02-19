// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include <ktoggleaction.h>

class KActionCollection;

/**
 * @class FullSCreenAction
 * @author Max Howell <max.howell@methylblue.com>
 * @short Adapted KToggleFullScreenAction, mainly because that class is shit
 */
class FullScreenAction : public KToggleAction
{
public:
    FullScreenAction( QWidget *window, KActionCollection* );

    virtual void setChecked( bool );
    virtual void setEnabled( bool );

protected:
    virtual bool eventFilter( QObject* o, QEvent* e );

private:
    QWidget *m_window;
    bool m_shouldBeDisabled;
    unsigned long m_state;
};
