/*
    SPDX-FileCopyrightText: 2008 David Edmundson <kde@davidedmundson.co.uk>
    SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "loadView.h"
#include "codeine.h"

#include <QAction>
#include <QIcon>
#include <QToolButton>

namespace Dragon
{

LoadView::LoadView(QWidget *parent)
    : QWidget(parent)
    , Ui_LoadView()
{
    setupUi(this);

    const auto largeSize = 128;
    m_icon->setPixmap(QIcon::fromTheme(QStringLiteral("dragonplayer")).pixmap(largeSize));
}

void LoadView::setToolbarActions(const QList<QAction *> &actions)
{
    for (const auto &action : actions) {
        if (!action->isVisible()) {
            continue;
        }
        auto button = new QToolButton(this);
        button->setDefaultAction(action);
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        horizontalLayout->insertWidget(horizontalLayout->count() - 1, button);
    }
}

} // namespace Dragon
