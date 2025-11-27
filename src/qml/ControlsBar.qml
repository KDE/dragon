// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import QtMultimedia as Multimedia
import org.kde.coreaddons as KCoreAddons
import org.kde.ki18n

OverlayPopup {
    id: toolbar
    readonly property bool anyMenusOpen: menuButton.menu.opened || volumeButton.popup.opened
    readonly property int leftRightOffset: Math.round(rightControls.implicitWidth - leftControls.implicitWidth) / 2
    property alias volumeButton: volumeButton
    property alias toolbarHandler: toolbarHandler
    required property Multimedia.MediaPlayer player
    property alias seekSlider: seekSlider
    property bool stackedMode: false

    closePolicy: QQC2.Popup.NoAutoClose

    contentItem: ColumnLayout {
        // Stacked UI
        ColumnLayout {
            spacing: Kirigami.Units.mediumSpacing
            visible: toolbar.stackedMode
            Item {
                Layout.fillWidth: true
                implicitHeight: Math.max(leftControls.implicitHeight,
                                         playerControls.implicitHeight,
                                         rightControls.implicitHeight)
                LayoutItemProxy {
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                    }
                    target: leftControls
                }
                LayoutItemProxy {
                    // Use anchors instead of a layout otherwise it would be
                    // impossible to *actually* center playerControls
                    anchors.centerIn: parent
                    target: playerControls
                }
                LayoutItemProxy {
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    target: rightControls
                }
            }
            LayoutItemProxy {
                target: positionDurationUI
            }
        }
        // Linear UI
        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing
            visible: !toolbar.stackedMode
            LayoutItemProxy {
                target: leftControls
            }
            LayoutItemProxy {
                target: playerControls
            }
            LayoutItemProxy {
                target: positionDurationUI
            }
            LayoutItemProxy {
                target: rightControls
            }
        }

        // Left control container
        RowLayout {
            id: leftControls
            spacing: Kirigami.Units.smallSpacing

            VolumeButton {
                id: volumeButton
                audioOutput: toolbar.player.audioOutput
            }
        }
    }

    // Seek/play/pause button container
    RowLayout {
        id: playerControls
        spacing: Kirigami.Units.smallSpacing

        IconToolButton {
            icon {
                width: Kirigami.Units.iconSizes.small
                height: Kirigami.Units.iconSizes.small
            }
            icon.name: "media-seek-backward-symbolic"
            onClicked: seek(seekSlider.value - 5000, true)
        }
        IconToolButton {
            action: togglePauseAction
        }
        IconToolButton {
            icon {
                width: Kirigami.Units.iconSizes.small
                height: Kirigami.Units.iconSizes.small
            }
            icon.name: "media-seek-forward-symbolic"
            onClicked: seek(seekSlider.value + 5000, true)
        }
    }

    // Right control container
    RowLayout {
        id: rightControls
        spacing: Kirigami.Units.smallSpacing

        IconToolButton {
            action: fullscreenAction
        }
        IconToolButton {
            id: menuButton

            icon.name: "open-menu-symbolic"

            text: KI18n.i18nc("@action:button", "Application Menu")
            QQC2.ToolTip.text: text
            QQC2.ToolTip.visible: hovered
                            && text.length > 0
                            && display === QQC2.AbstractButton.IconOnly
                            && !pressed
                            && !menu.visible

            down: pressed || menu.visible
            Accessible.role: Accessible.ButtonMenu

            onPressed: {
                if (!menuButton.menu.visible) {
                    menuButton.menu.open()
                } else {
                    menuButton.menu.dismiss()
                }
            }

            readonly property T.Menu menu: QQC2.Menu {
                // Aligned with overlay panel border
                x: Qt.application.layoutDirection === Qt.RightToLeft ? - toolbar.padding : -width + menuButton.width + toolbar.padding
                y: -height - toolbar.padding - Kirigami.Units.smallSpacing

                Kirigami.Action {
                    text: appWindow.openAction.text
                    icon.name: appWindow.openAction.icon.name
                    onTriggered: appWindow.openAction.trigger()
                }

                Kirigami.Action {
                    text: KI18n.i18nc("@action:button stop playback", "Stop")
                    enabled: !player.stopped
                    icon.name: "media-playback-stop"
                    onTriggered: player.stop()
                    shortcut: "S"
                }

                Kirigami.Action {
                    text: player.audioOutput.muted ? KI18n.i18nc("@action:button", "Unmute") : KI18n.i18nc("@action:button", "Mute")
                    icon.name: player.audioOutput.muted ? "player-volume" : "player-volume-muted"
                    onTriggered: player.audioOutput.muted = !player.audioOutput.muted
                    shortcut: "M"
                }

                QQC2.MenuSeparator {}

                QQC2.Menu {
                    icon.name: "add-subtitle-symbolic"
                    title: KI18n.i18nc("@action:button video subtitle", "Subtitles")
                    enabled: count > 0
                    Repeater {
                        id: subtitleRepeater
                        readonly property int prependedItems: 1
                        model: [{
                            keys: () => [0],
                            stringValue: () => KI18n.i18nc("@action:button selector for no subtitle", "None")
                        }].concat(player.subtitleTracks)
                        delegate: QQC2.RadioDelegate {
                            required property var modelData
                            required property int index
                            text: {
                                const hasTitle = modelData?.keys()?.includes(0)
                                const hasLanguage = modelData?.keys()?.includes(6)
                                if (hasTitle && hasLanguage) {
                                    return KI18n.i18nc("@action:button subtitle selector %1 is usually a language (e.g. chinese) and %2 is usually a subtitle (e.g. Traditional)",
                                                "%1 [%2]",
                                                modelData.stringValue(6),
                                                modelData.stringValue(0))
                                }
                                if (hasLanguage) {
                                    return modelData.stringValue(6)
                                }
                                return modelData.stringValue(0)
                            }
                            checked: player.activeSubtitleTrack === index - subtitleRepeater.prependedItems
                            onToggled: player.activeSubtitleTrack = index - subtitleRepeater.prependedItems
                        }
                    }
                }

                QQC2.Menu {
                    icon.name: "text-speak-symbolic"
                    title: KI18n.i18nc("@action:button track selector", "Audio Track")
                    enabled: count > 0
                    Repeater {
                        model: player.audioTracks
                        delegate: QQC2.RadioDelegate {
                            required property var modelData
                            required property int index
                            text: modelData.stringValue(6)
                            checked: player.activeAudioTrack === index
                            onToggled: player.activeAudioTrack = index
                        }
                    }
                }
                QQC2.Menu {
                    icon.name: "kdenlive-add-clip-symbolic"
                    title: KI18n.i18nc("@action:button track selector", "Video Track")
                    enabled: count > 0
                    Repeater {
                        model: player.videoTracks
                        delegate: QQC2.RadioDelegate {
                            required property var modelData
                            required property int index
                            text: modelData.stringValue(6)
                            checked: player.activeVideoTrack === index
                            onToggled: player.activeVideoTrack = index
                        }
                    }
                }

                QQC2.MenuSeparator {}

                Kirigami.Action {
                    icon.name: "dragonplayer"
                    text: KI18n.i18nc("@action opens about app page", "About")
                    onTriggered: pageStack.layers.push("AboutPage.qml")
                }
            }
        }
    }


    // Position slider and time
    Item {
        // This can't be directly the RowLayout, because of a behavior of LayoutItemProxy,
        // that breaks the nested Layout.fillWidth
        id: positionDurationUI
        Layout.fillWidth: true
        implicitWidth: seekLayout.implicitWidth
        implicitHeight: seekLayout.implicitHeight
        RowLayout {
            id: seekLayout
            anchors.fill: parent

            spacing: Kirigami.Units.smallSpacing

            // Current position
            QQC2.Label {
                Layout.alignment: Qt.AlignVCenter
                text: KCoreAddons.Format.formatDuration(player.position)
            }

            QQC2.Slider {
                id: seekSlider

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                property Timer seekTimer: Timer {
                    property int position: -1
                    interval: 50
                    onTriggered: toolbar.player.position = position
                }

                // Note that hovering is handled by the toolbar not the slider!
                hoverEnabled: false
                from: 0
                to: toolbar.player.duration
                onMoved: {
                    // Delay seeks ever so slightly to prevent vaapi from falling over because we allocate too many frames when
                    // the user seeks vigorously.
                    seekTimer.position = value
                    seekTimer.restart()
                }
                wheelEnabled: true
                enabled: toolbar.player.playbackState !== Multimedia.MediaPlayer.StoppedState && toolbar.player.seekable

                Behavior on opacity {
                    NumberAnimation { duration: Kirigami.Units.shortDuration }
                }

                Binding {
                    target: seekSlider
                    property: 'value'
                    value: toolbar.player.position
                    when: !seekSlider.pressed
                    delayed: true
                }
            }

            // Total duration
            QQC2.Label {
                Layout.alignment: Qt.AlignVCenter
                text: KCoreAddons.Format.formatDuration(player.duration)
            }
        }
    }

    HoverHandler {
        id: toolbarHandler
        margin: Kirigami.Units.gridUnit
        onHoveredChanged: activeTimer.restart()
    }
}
