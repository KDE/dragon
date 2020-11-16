/*
    SPDX-FileCopyrightText: 2003-2005 Max Howell <max.howell@methylblue.com>                       *

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BLOCKANALYZER_H
#define BLOCKANALYZER_H

#include "analyzerBase.h"

#include <QColor>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QPixmap>

class QResizeEvent;
class QMouseEvent;
class QPalette;


/**
 * @author Max Howell
 */

class BlockAnalyzer : public Analyzer::Base2D
{
public:
    explicit BlockAnalyzer( QWidget* );
    ~BlockAnalyzer() override;

    // Signed ints because most of what we compare them against are ints
    static const int HEIGHT      = 2;
    static const int WIDTH       = 4;
    static const int MIN_ROWS    = 48; // arbitrary
    static const int MIN_COLUMNS = 128; // arbitrary
    static const int MAX_COLUMNS = 128; // must be 2**n
    static const int FADE_SIZE   = 90;

protected:
    void transform( QVector<float>& ) override;
    void analyze( const QVector<float>& ) override;
    void paintEvent( QPaintEvent* ) override;
    void resizeEvent( QResizeEvent* ) override;
    void paletteChange( const QPalette& );

    void drawBackground();
    void determineStep();

private:
    QPixmap* bar() { return &m_barPixmap; }

    uint m_columns, m_rows;      //number of rows and columns of blocks
    uint m_y;                    //y-offset from top of widget
    QPixmap m_barPixmap;
    QPixmap m_topBarPixmap;
    QVector<float> m_scope;               //so we don't create a vector every frame
    std::vector<float> m_store;  //current bar heights
    std::vector<float> m_yscale;

    //FIXME why can't I namespace these? c++ issue?
    std::vector<QPixmap> m_fade_bars;
    std::vector<uint>    m_fade_pos;
    std::vector<int>     m_fade_intensity;
    QPixmap              m_background;

    float m_step; //rows to fall per frame
};

#endif
