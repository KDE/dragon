// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include <KLocale>
#include <KGuiItem>
#include <KPushButton>

#include <QApplication>
#include <QEvent>
#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QTimerEvent>

#include "adjustSizeButton.h"
#include "extern.h"
#include "theStream.h"
#include "videoWindow.h" //videoWindow()


QString i18n( const char *text );

namespace Codeine
{
    AdjustSizeButton::AdjustSizeButton( QWidget *parent )
            : QFrame( parent )
            , m_counter( 0 )
            , m_stage( 1 )
            , m_offset( 0 )
    {
        parent->installEventFilter( this );

        setPalette( QApplication::palette() ); //videoWindow has different palette
        setFrameStyle( QFrame::Plain | QFrame::Box );

        m_preferred = new KPushButton( KGuiItem( i18n("Preferred Scale"), "viewmag" ), this );
        connect( m_preferred, SIGNAL(clicked()), Codeine::mainWindow(), SLOT(adjustSize()) );
        connect( m_preferred, SIGNAL(clicked()), SLOT(deleteLater()) );

        m_oneToOne = new KPushButton( KGuiItem( i18n("Scale 100%"), "viewmag1" ), this );
        connect( m_oneToOne, SIGNAL(clicked()), (QObject*)videoWindow(), SLOT(resetZoom()) );
        connect( m_oneToOne, SIGNAL(clicked()), SLOT(deleteLater()) );

        QBoxLayout *hbox = new QHBoxLayout( this );
        hbox->setMargin( 8 );
        hbox->setSpacing( 6 );
        QBoxLayout *vbox = new QVBoxLayout( this );
        hbox->addLayout( vbox );
        vbox->addWidget( new QLabel( i18n( "<b>Adjust video scale?" ), this ) );
        vbox->addWidget( m_preferred );
        vbox->addWidget( m_oneToOne );
        hbox->addWidget( m_thingy = new QFrame( this ) );

        m_thingy->setFixedWidth( fontMetrics().width( "X" ) );
        m_thingy->setFrameStyle( QFrame::Plain | QFrame::Box );
        {
            QPalette palette;
            QPalette thisPalette = this->palette();
            palette.setColor( m_thingy->backgroundRole(), palette.color( QPalette::Window ).darker() );
            m_thingy->setPalette(palette);
        }

        QEvent e( QEvent::Resize );
        eventFilter( 0, &e );

        adjustSize();
        show();

        m_timerId = startTimer( 5 );
    }

    void
    AdjustSizeButton::timerEvent( QTimerEvent* )
    {
        QFrame *&h = m_thingy;

        switch( m_stage )
        {
        case 1: //raise
            move();
            m_offset++;

            if( m_offset > height() )
                killTimer( m_timerId ),
                m_timerId = startTimer( 40 ),
                m_stage = 2;

            break;

        case 2: //fill in pause timer bar
            if( m_counter < h->height() - 3 )
                QPainter( h ).fillRect( 2, 2, h->width() - 4, m_counter
                    , palette().color( QPalette::Active, QPalette::Highlight ) );

            if( !testAttribute(Qt::WA_UnderMouse)  )
                m_counter++;

            if( m_counter > h->height() + 5 ) //pause for 360ms before lowering
                m_stage = 3,
                killTimer( m_timerId ),
                m_timerId = startTimer( 6 );

            break;

        case 3: //lower
            if( testAttribute(Qt::WA_UnderMouse)  ) {
                m_stage = 1;
                m_counter = 0;
                m_thingy->repaint();
                break; }

            m_offset--;
            move();

            if( m_offset < 0 )
                deleteLater();
        }
    }

    bool
    AdjustSizeButton::eventFilter( QObject */*o*/, QEvent *e )
    {
        if( e->type() == QEvent::Resize ) {
            const QSize preferredSize = TheStream::profile().readEntry<QSize>( "Preferred Size", QSize() );
            const QSize defaultSize = TheStream::defaultVideoSize();
            const QSize parentSize = parentWidget()->size();

            m_preferred->setEnabled( preferredSize.isValid() && parentSize != preferredSize && defaultSize != preferredSize );
            m_oneToOne->setEnabled( defaultSize != parentSize );

            move();

            if( !m_preferred->isEnabled() && !m_oneToOne->isEnabled() && m_counter == 0 )
                deleteLater();
        }

        return false;
    }
}
#include "adjustSizeButton.moc"
