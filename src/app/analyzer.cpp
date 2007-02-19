// (c) 2004 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include "analyzer.h"
#include "codeine.h"
#include "debug.h"
#include <math.h>         //interpolate()
#include <qevent.h>      //event()
//Added by qt3to4:
#include <QResizeEvent>
#include "xineEngine.h"

#include "fht.cpp"

template<class W>
Analyzer::Base<W>::Base( QWidget *parent, uint timeout )
        : W( parent, "Analyzer" )
        , m_timeout( timeout )
{}

template<class W> bool
Analyzer::Base<W>::event( QEvent *e )
{
    switch( e->type() ) {
    case QEvent::Hide:
        m_timer.stop();
        break;

    case QEvent::Show:
        m_timer.start( timeout() );
        break;

    default:
        ;
    }

    return QWidget::event( e );
}


Analyzer::Base2D::Base2D( QWidget *parent, uint timeout )
        : Base<QWidget>( parent, timeout )
{
    setWindowFlags( Qt::WNoAutoErase ); //no flicker
    connect( &m_timer, SIGNAL(timeout()), SLOT(draw()) );
}

void
Analyzer::Base2D::draw()
{
    switch( Codeine::engine()->state() ) {
    case Engine::Playing:
    {
        const Engine::Scope &thescope = Codeine::engine()->scope();
        static Analyzer::Scope scope( Analyzer::SCOPE_SIZE );

        for( int x = 0; x < Analyzer::SCOPE_SIZE; ++x )
            scope[x] = double(thescope[x]) / (1<<15);

        transform( scope );
        analyze( scope );

        scope.resize( Analyzer::SCOPE_SIZE );

        bitBlt( this, 0, 0, canvas() );
        break;
    }
    case Engine::Paused:
        break;

    default:
        erase();
    }
}

void
Analyzer::Base2D::resizeEvent( QResizeEvent* )
{
    m_canvas.resize( size() );
    m_canvas.fill( colorGroup().background() );
}



// Author:     Max Howell <max.howell@methylblue.com>, (C) 2003
// Copyright: See COPYING file that comes with this distribution

#include <qpainter.h>

Analyzer::Block::Block( QWidget *parent )
        : Analyzer::Base2D( parent, 20 )
{
    setMinimumWidth( 64 ); //-1 is padding, no drawing takes place there
    setMaximumWidth( 128 );

    //TODO yes, do height for width
}

void
Analyzer::Block::transform( Analyzer::Scope &scope ) //pure virtual
{
    static FHT fht( Analyzer::SCOPE_SIZE_EXP );

    for( uint x = 0; x < scope.size(); ++x )
        scope[x] *= 2;

    float *front = static_cast<float*>( &scope.front() );

    fht.spectrum( front );
    fht.scale( front, 1.0 / 40 );
}

#include <math.h>
void
Analyzer::Block::analyze( const Analyzer::Scope &s )
{
    canvas()->fill( colorGroup().foreground().light() );

    QPainter p( canvas() );
    p.setPen( colorGroup().background() );

    const double F = double(height()) / (log10( 256 ) * 1.1 /*<- max. amplitude*/);

    for( uint x = 0; x < s.size(); ++x )
        //we draw the blank bit
        p.drawLine( x, 0, x, int(height() - log10( s[x] * 256.0 ) * F) );
}

int
Analyzer::Block::heightForWidth( int w ) const
{
    return w / 2;
}

#include "analyzer.moc"