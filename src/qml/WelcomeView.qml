// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami

ColumnLayout {
    spacing: Kirigami.Units.gridUnit
    width: parent.width - (Kirigami.Units.largeSpacing * 4)
    anchors.centerIn: parent

    Kirigami.Icon {
        Layout.alignment: Qt.AlignHCenter
        source: "dragonplayer"
        implicitWidth: Kirigami.Units.iconSizes.enormous
        implicitHeight: implicitWidth
    }

    Kirigami.PlaceholderMessage {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        text: i18nc("@title", "Welcome to Dragon Player")
        explanation: i18nc("@info", "Dragon Player is a simple video player. Open a video file to get started:")
    }

    Flow {
        Layout.alignment: Qt.AlignHCenter
        Layout.maximumWidth: parent.width

        QQC2.ToolButton {
            icon.width: Kirigami.Units.iconSizes.huge
            icon.height: Kirigami.Units.iconSizes.huge
            display: QQC2.AbstractButton.TextUnderIcon
            action: appWindow.openAction
        }

        QQC2.ToolButton {
            icon.width: Kirigami.Units.iconSizes.huge
            icon.height: Kirigami.Units.iconSizes.huge
            display: QQC2.AbstractButton.TextUnderIcon
            action: Kirigami.Action {
                text: i18nc("@action:button", "Open Network Resource…")
                icon.name: "folder-network-symbolic"
                onTriggered: appWindow.openAction.trigger()
            }
        }
    }
}
