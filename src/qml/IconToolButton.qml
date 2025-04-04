// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2025 Harald Sitter <sitter@kde.org>

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

QQC2.ToolButton {
    display: QQC2.AbstractButton.IconOnly
    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
    QQC2.ToolTip.visible: hovered
    QQC2.ToolTip.text: (action as Kirigami.Action)?.tooltip
}
