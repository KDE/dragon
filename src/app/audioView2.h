/*
    SPDX-FileCopyrightText: 2012 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

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
    explicit AudioView2(QWidget *parent = nullptr);
    ~AudioView2() override;

    void setupAnalyzer();

public Q_SLOTS:
    void enableDemo(bool enable);
    void update();
    
protected:
    void changeEvent(QEvent *e) override;
    
private:
    Ui::AudioView2 *ui;
};


} // namespace Dragon
#endif // DRAGON_AUDIOVIEW2_H
