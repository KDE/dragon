// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import QtMultimedia as Multimedia

QQC2.ToolBar {
    id: toolbar
    readonly property var hiddenInset: -contentHeight
    property alias volumeButton: volumeButton
    property alias toolbarHandler: toolbarHandler
    required property Multimedia.MediaPlayer player
    property alias seekSlider: seekSlider

    visible: topInset !== hiddenInset

    Behavior on topInset {
        NumberAnimation { duration: Kirigami.Units.veryShortDuration }
    }

    Behavior on topPadding {
        NumberAnimation { duration: Kirigami.Units.veryShortDuration }
    }

    RowLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.mediumSpacing

        IconToolButton {
            action: appWindow.openAction
        }

        Kirigami.Separator {
            Layout.fillHeight: true
            width: 1
        }

        IconToolButton {
            action: togglePauseAction
        }

        QQC2.Slider {
            id: seekSlider

            Layout.fillWidth: true

            // Note that hovering is handled by the toolbar not the slider!
            hoverEnabled: false
            from: 0
            to: player.duration
            onMoved: videoPage.seek(value, true)
            wheelEnabled: true
            enabled: !toolbar.player.stopped && toolbar.player.seekable

            Behavior on opacity {
                NumberAnimation { duration: Kirigami.Units.shortDuration }
            }

            Binding {
                target: seekSlider
                property: 'value'
                value: player.position
                when: !seekSlider.pressed
                delayed: true
            }
        }

        IconToolButton {
            action: fullscreenAction
        }

        VolumeButton {
            id: volumeButton
            audioOutput: toolbar.player.audioOutput
        }

        IconToolButton {
            id: menuButton
            readonly property var menu: QQC2.Menu {
                topMargin: menuButton.height

                Kirigami.Action {
                    text: i18nc("@action:button stop playback", "Stop")
                    enabled: !player.stopped
                    icon.name: "media-playback-stop"
                    onTriggered: player.stop()
                    shortcut: "S"
                }

                Kirigami.Action {
                    text: player.audioOutput.muted ? i18nc("@action:button", "Unmute") : i18nc("@action:button", "Mute")
                    icon.name: player.audioOutput.muted ? "player-volume" : "player-volume-muted"
                    onTriggered: player.audioOutput.muted = !player.audioOutput.muted
                    shortcut: "M"
                }

                QQC2.Menu {
                    title: i18nc("@action:button video subtitle", "Subtitle")
                    enabled: count > 0
                    Repeater {
                        id: subtitleRepeater
                        readonly property int prependedItems: 1
                        model: [{
                            keys: () => [0],
                            stringValue: () => i18nc("@action:button selector for no subtitle", "None")
                        }].concat(player.subtitleTracks)
                        delegate: QQC2.RadioButton {
                            required property var modelData
                            required property int index
                            text: {
                                const hasTitle = modelData?.keys()?.includes(0)
                                const hasLanguage = modelData?.keys()?.includes(6)
                                if (hasTitle && hasLanguage) {
                                    return i18nc("@action:button subtitle selector %1 is usually a language (e.g. chinese) and %2 is usually a subtitle (e.g. Traditional)",
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
                    title: i18nc("@action:button track selector", "Audio Track")
                    enabled: count > 0
                    Repeater {
                        model: player.audioTracks
                        delegate: QQC2.RadioButton {
                            required property var modelData
                            required property int index
                            text: modelData.stringValue(6)
                            checked: player.activeAudioTrack === index
                            onToggled: player.activeAudioTrack = index
                        }
                    }
                }
                QQC2.Menu {
                    title: i18nc("@action:button track selector", "Video Track")
                    enabled: count > 0
                    Repeater {
                        model: player.videoTracks
                        delegate: QQC2.RadioButton {
                            required property var modelData
                            required property int index
                            text: modelData.stringValue(6)
                            checked: player.activeVideoTrack === index
                            onToggled: player.activeVideoTrack = index
                        }
                    }
                }
                Kirigami.Action {
                    icon.name: "dragonplayer"
                    text: i18nc("@action opens about app page", "About")
                    onTriggered: { pageStack.layers.push("qrc:/ui/AboutPage.qml") }
                }
            }
            action: Kirigami.Action {
                text: i18nc("@action:button", "Application Menu")
                icon.name: "open-menu-symbolic"
                onTriggered: menuButton.menu.open()
                tooltip: text
            }
        }
    }

    HoverHandler {
        id: toolbarHandler
        margin: Kirigami.Units.gridUnit
        onHoveredChanged: activeTimer.restart()
    }
}
