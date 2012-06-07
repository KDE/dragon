// Author:     Max Howell <max.howell@methylblue.com>,    (C) 2004
// Maintainer: Martin Sandsmark <sandsmark@samfundet.no>, (C) 2009
// Copyright:  See the COPYING file shipped with this distribution

#ifndef ANALYZERBASE_H
#define ANALYZERBASE_H

#ifdef __FreeBSD__
#include <sys/types.h>
#endif

#include "fht.h"     //stack allocated and convenience
#include <qpixmap.h> //stack allocated and convenience
#include <qtimer.h>  //stack allocated
#include <qwidget.h> //baseclass
#include <vector>    //included for convenience

#include <phonon/audiodataoutput.h>

class QEvent;
class QPaintEvent;
class QResizeEvent;


namespace Analyzer {

typedef std::vector<float> Scope;

class Base : public QWidget
{
    Q_OBJECT

public slots:
    void drawFrame(const QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > &thescope);

protected:
    Base(QWidget*, uint = 7);
    ~Base() { delete m_fht; }

    int  resizeExponent(int);
    int  resizeForBands(int);
    virtual void transform(QVector<float>&);
    virtual void analyze(const QVector<float>&) = 0;
    virtual void paused();
public slots:
    void demo();
protected:
    FHT    *m_fht;
};


class Base2D : public Base
{
Q_OBJECT
public:
    const QPixmap *canvas()     const { return &m_canvas; }

// private slots:
//     void draw() { drawFrame(); bitBlt( this, 0, 0, canvas() ); }

    void enableDemo(bool enable) { enable ? timer.start() : timer.stop(); }


protected:
    Base2D( QWidget*, uint scopeSize = 7);


    QPixmap     *canvas() { return &m_canvas; }
    void    eraseCanvas() { m_canvas.fill(Qt::transparent); }

    void paintEvent( QPaintEvent* );
    void resizeEvent( QResizeEvent* );


protected slots:
    virtual void init() {}

private:
    QPixmap m_canvas;
    QTimer timer;
};

class Factory
{
    //Currently this is a rather small class, its only purpose
    //to ensure that making changes to analyzers will not require
    //rebuilding the world!

    //eventually it would be better to make analyzers pluggable
    //but I can't be arsed, nor can I see much reason to do so
    //yet!
public:
    static QWidget* createAnalyzer(QWidget*);
    static QWidget* createPlaylistAnalyzer(QWidget *);
};


void interpolate(const QVector<float>&, QVector<float>&);
void initSin(QVector<float>&, const uint = 6000);

} //END namespace Analyzer

using Analyzer::Scope;

#endif
