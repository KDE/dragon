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

#ifndef TEXTITEM_H
#define TEXTITEM_H

#include <QGraphicsWidget>
#include <QGraphicsTextItem>

/**
 * A QGV text widget which will automatically change its font size based on 
 * the space available.
 **/
class ExpandingTextItem : public QGraphicsTextItem, public QGraphicsLayoutItem
{
  Q_OBJECT
  Q_INTERFACES(QGraphicsLayoutItem)
  public:
    ExpandingTextItem(QGraphicsWidget* parent = 0);
    ~ExpandingTextItem();
    void setPlainText(const QString& text);
    void setGeometry(const QRectF& rect);
  protected:
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSizeF()) const;
    virtual void updateGeometry();
};

#endif // TEXTITEM_H
