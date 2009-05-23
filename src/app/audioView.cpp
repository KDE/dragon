/***********************************************************************
 * Copyright 2008  David Edmundson <kde@davidedmundson.co.uk>
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

#include "audioView.h"
#include "theStream.h"

namespace Dragon
{

AudioView::AudioView( QWidget *parent) 
    : QWidget( parent )
{
   m_message.clear();
   setAutoFillBackground(true);
   QPalette pal;
   pal.setColor( QPalette::Window, Qt::black );
   setPalette( pal );
}

void
AudioView::updateText()
{
    m_message = TheStream::prettyTitle();
    repaint();
}

void
AudioView::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);
  painter.setPen(Qt::white);
  painter.drawText(rect(), Qt::AlignCenter, m_message);
}


AudioView::~AudioView()
{
}

}
#include "audioView.moc"
