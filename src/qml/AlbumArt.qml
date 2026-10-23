// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>

import QtQuick
import org.kde.kirigami as Kirigami
import QtQuick.Effects as Effects
import org.kde.kquickcontrolsaddons as KQA

Item {
    property alias source: imageItem.image
    readonly property bool valid: !imageItem.null

    Text {
        text: "FOO: " + parent.source
        font.pointSize: 30
        color: "magenta"
    }

    Effects.MultiEffect {
        source: ShaderEffectSource {
            sourceItem: imageItem
        }
        anchors.centerIn: parent
        width: Math.max(parent.width, parent.height)
        height: width
        // Try to match Plasma's Media Controller.
        brightness: -0.25
        saturation: 0.9
        autoPaddingEnabled: false
        blurEnabled: true
        blur: 1.0
        blurMax: 128
    }

    KQA.QImageItem {
        id: imageItem
        anchors.centerIn: parent
        width: nativeWidth
        height: nativeHeight
        scale: Math.min(1.0,
                        (parent.width - Kirigami.Units.gridUnit) / nativeWidth,
                        (parent.height - Kirigami.Units.gridUnit) / nativeHeight)
        smooth: true
    }
}