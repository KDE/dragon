// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2025 Marco Martin <mart@kde.org>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC
import org.kde.kirigami as Kirigami
import QtMultimedia as Multimedia

QQC.Popup {
    id: toolbar
   // x: Math.round(parent.width / 2 - width / 2)
   // y: parent.height - height - Kirigami.Units.gridUnit * 2
  //  width: parent.width - Kirigami.Units.gridUnit * 4
    modal: false
    clip: false
    padding: Kirigami.Units.smallSpacing
    margins: Kirigami.Units.gridUnit * 2
    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary

    Binding {
        target: toolbar.contentItem?.Kirigami.Theme
        property: "colorSet"
        value: toolbar.Kirigami.Theme.colorSet
    }
    background: Kirigami.ShadowedRectangle {
        Kirigami.Theme.colorSet: toolbar.Kirigami.Theme.colorSet
        color: Qt.alpha(Kirigami.Theme.backgroundColor, 0.6);
        radius: Kirigami.Units.cornerRadius
        border {
            width: 1
            color: Qt.alpha(Kirigami.Theme.textColor, 0.2);
        }
        shadow {
            size: Kirigami.Units.gridUnit
            color: Qt.rgba(0, 0, 0, 0.25)
            yOffset: 2
        }
    }
    enter: Transition {
        NumberAnimation {
            property: "opacity"
            from: 0
            to: 1
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }
    exit: Transition {
        NumberAnimation {
            property: "opacity"
            from: 1
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }
}
