// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

import QtQuick
import QtQuick.Controls as QQC2
import QtMultimedia as Multimedia

import org.kde.kirigami as Kirigami

IconToolButton {
    id: volumeButton

    required property Multimedia.AudioOutput audioOutput

    icon.name: {
        if (volumeButton.audioOutput.muted || volumeButton.audioOutput.volume == 0.0) {
            return "audio-volume-muted"
        }
        if (volumeButton.audioOutput.volume > 0.66) {
            return "audio-volume-high"
        }
        if (volumeButton.audioOutput.volume > 0.33) {
            return "audio-volume-medium"
        }
        if (volumeButton.audioOutput.volume > 0) {
            return "audio-volume-low"
        }
        return "player-volume"
    }

    text: i18nc("@action:button open volume slider popup", "Show volume controls")
    QQC2.ToolTip.text: text
    QQC2.ToolTip.visible: hovered
                       && text.length > 0
                       && display === QQC2.AbstractButton.IconOnly
                       && !pressed
                       && !popup.visible

    down: pressed || popup.visible
    Accessible.role: Accessible.ButtonMenu

    onPressed: {
        if (!popup.visible) {
            popup.open()
        } else {
            popup.close()
        }
    }

    readonly property QQC2.Popup popup : QQC2.Popup {
        y: volumeButton.height
        QQC2.Slider {
            orientation: Qt.Vertical
            from: 0.0
            to: 1.0
            value: volumeButton.audioOutput.volume
            wheelEnabled: false
            onMoved: volumeButton.audioOutput.volume = value
        }
        onOpenedChanged: {
            // Do not force the action to uncheck if the user is interacting with the button.
            // This prevents a cyclic toggle when the popup loses focus because it is getting closed by an explicit
            // click on the button.
            if (!hoverHandler.hovered) {
                volumeButton.checked = opened
            }
        }
    }

    HoverHandler {
        id: hoverHandler
    }
}
