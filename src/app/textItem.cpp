/***********************************************************************
* Copyright 2010  Ian Monroe <ian@monroe.nu>
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

#include "textItem.h"

#include <KDebug>

#include <QTextDocument>
#include <QSizeF>
#include <QWidget>

ExpandingTextItem::ExpandingTextItem(QGraphicsWidget* parent)
    : QGraphicsTextItem(parent), QGraphicsLayoutItem()
{

}

ExpandingTextItem::~ExpandingTextItem()
{ }

QSizeF 
ExpandingTextItem::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
{
    //credit to: psih128
    // http://www.qtcentre.org/threads/16533-Subclassing-QGraphicsTextItem-and-QGraphicsLayoutItem
    switch (which) {
        case Qt::MinimumSize:
            return QSizeF(0, 0);
        case Qt::PreferredSize:
            return document()->size();                                                                                            
        case Qt::MaximumSize:
            return QSizeF(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        default:
            qWarning("TextItem::sizeHint(): Don't know how to handle the value of 'which'");
            break;      
    }
    return constraint;
    
}

void
ExpandingTextItem::setGeometry(const QRectF& rect)
{
    QFont theFont = font();
    
    int size = qMin(static_cast<int>(rect.height()), 40);
    
    theFont.setPixelSize(size);
    QFontMetricsF fm(theFont);
    while(fm.width(toPlainText()) > rect.width())
    {
        size -= 5;
        theFont.setPixelSize(size);
        fm = QFontMetricsF(theFont);      
    }
    
    setFont(theFont);
    setPos(rect.topLeft());
}

void
ExpandingTextItem::updateGeometry()
{
    QGraphicsLayoutItem::updateGeometry();
}

void
ExpandingTextItem::setPlainText(const QString& text)
{
    QGraphicsTextItem::setPlainText(text);
    updateGeometry();
}

#include "textItem.moc"

