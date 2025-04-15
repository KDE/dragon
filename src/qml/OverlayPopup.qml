// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2025 Marco Martin <mart@kde.org>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC
import org.kde.kirigami as Kirigami
import QtMultimedia as Multimedia

QQC.Popup {
    id: popup

    modal: false
    clip: false
    padding: Kirigami.Units.smallSpacing
    margins: Kirigami.Units.gridUnit * 2
    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary

    Binding {
        target: popup.contentItem?.Kirigami.Theme
        property: "colorSet"
        value: popup.Kirigami.Theme.colorSet
    }
    background: Kirigami.ShadowedRectangle {
        Kirigami.Theme.colorSet: popup.Kirigami.Theme.colorSet
        color: Qt.alpha(Kirigami.Theme.backgroundColor, 0.6);
        // This to make the radius of the buttons near the borders exactly concentric,
        // otherwise it looks very janky
        radius: Kirigami.Units.cornerRadius + popup.padding
        border {
            width: 1
            color: Qt.alpha(
                Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.4),
                0.6);
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
