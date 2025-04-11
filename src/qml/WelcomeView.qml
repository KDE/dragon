// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>
// SPDX-FileCopyrightText: 2025 Nate Graham <nate@kde.org>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami

Kirigami.PlaceholderMessage {
    width: parent.width - (Kirigami.Units.largeSpacing * 4)
    anchors.centerIn: parent

    icon.name: "dragonplayer"

    text: i18nc("@title", "Welcome to Dragon Player")
    explanation: i18nc("@info", "Dragon Player is a simple video player. Open a video to get started:")

    helpfulAction: Kirigami.Action {
        text: i18nc("@action:button", "Open Video File or Network Stream")
        icon.name: appWindow.openAction.icon.name
        onTriggered: appWindow.openAction.trigger()
    }
}
