#ifndef CODEINE_FULLSCREENTOOLBARHANDLER_H
#define CODEINE_FULLSCREENTOOLBARHANDLER_H

#include <QObject>

class KMainWindow;
class QTimerEvent;
class KToolBar;

namespace Codeine 
{
    class FullScreenToolBarHandler : QObject
    {
        Q_OBJECT
        public:
            FullScreenToolBarHandler( KMainWindow *parent );
            bool eventFilter( QObject *o, QEvent *e );
            void timerEvent( QTimerEvent* );
        private:
            KToolBar *m_toolbar;
            int m_timer_id;
            bool m_stay_hidden_for_a_bit;
            QPoint m_home;
    };
}
#endif
