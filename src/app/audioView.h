/***********************************************************************
 * Copyright 2009 David Edmundson <kde@davidedmundson.co.uk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy 
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************/
#ifndef AUDIOVIEW_H
#define AUDIOVIEW_H

#include <QGraphicsView>
#include <QPointer>
class ExpandingTextItem;
class QGraphicsGridLayout;
class QGraphicsPixmapItem;

namespace Plasma{
    class Label;
}
class BlockAnalyzer;

namespace Dragon
{

class AudioView : public QGraphicsView
{
    Q_OBJECT
    public:
       explicit AudioView(QWidget *parent);
       virtual ~AudioView();
       virtual void updateText();
    protected:
        void resizeEvent(QResizeEvent *event);
    private:
        QString checkForAmarokImage(const QString &, const QString &);
        
        QPointer<ExpandingTextItem> m_artist;
        QPointer<ExpandingTextItem> m_album;
        QPointer<ExpandingTextItem> m_track;
        QGraphicsPixmapItem* m_image;
        QPointer<QGraphicsWidget> m_widget;
        QGraphicsGridLayout* m_layout;
        BlockAnalyzer* m_analyzer;
};

}
#endif 
