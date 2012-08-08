/***********************************************************************
 * Copyright 2012 Harald Sitter <sitter@kde.org>
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

#ifndef DRAGON_AUDIOVIEW2_H
#define DRAGON_AUDIOVIEW2_H

#include <QWidget>

namespace Dragon {

namespace Ui {
class AudioView2;
}

class AudioView2 : public QWidget
{
    Q_OBJECT
    
public:
    explicit AudioView2(QWidget *parent = 0);
    ~AudioView2();

public slots:
    void enableDemo(bool enable);
    void update();
    
protected:
    void changeEvent(QEvent *e);
    
private:
    Ui::AudioView2 *ui;
};


} // namespace Dragon
#endif // DRAGON_AUDIOVIEW2_H
