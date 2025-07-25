// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import QtQuick.Window as Window
import org.kde.coreaddons as KCoreAddons
import Qt.labs.animation
import QtMultimedia as Multimedia

import org.kde.dragon as Dragon

Kirigami.Page {
    id: videoPage

    property var storedVisibility: null
    property alias player: player
    property alias videoContainer: videoContainer
    required property QtObject fullscreenAction

    // Depending on whether the About page is on the stack we'll either want a header (about page) or not (video page)
    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None
    leftPadding: 0
    topPadding: 0
    rightPadding: 0
    bottomPadding: 0

    onVisibleChanged: {
        // Pause when we open the about layer
        if (!visible) {
            player.pause()
        }
    }

    Kirigami.Action {
        id: togglePauseAction
        text: player.paused || player.stopped ? i18nc("@action:button", "Play") : i18nc("@action:button", "Pause")
        icon.name: player.paused || player.stopped ? "media-playback-start" : "media-playback-pause"
        onTriggered: player.stopped ? player.play() : player.togglePause()
        tooltip: text
    }

    Kirigami.Action {
        id: fullscreenAction
        text: visibility === Window.Window.FullScreen ? i18nc("@action:button", "Exit Fullscreen") : i18nc("@action:button", "Enter Fullscreen")
        icon.name: "view-fullscreen"
        checkable: true
        checked: visibility === Window.Window.FullScreen
        onTriggered: videoPage.toggleFullscreen()
        shortcut: "F"
        tooltip: text
        fromQAction: videoPage.fullscreenAction
    }

    ColumnLayout {
        id: toolbarLayout
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        z: 2 // so we are on top of the video item!
        spacing: 0

        Kirigami.Separator {
            visible: !videoContainer.visible
            Layout.fillWidth: true
        }

        Kirigami.InlineMessage {
            Layout.fillWidth: true
            type: Kirigami.MessageType.Warning
            showCloseButton: true
            position: Kirigami.InlineMessage.Position.Header
            visible: text.length > 0
            text: {
                if (!video.videoSink.Backend.ffmpeg) {
                    return xi18nc("@info %2 is the name of a distribution",
`Dragon Player only supports the QtMultimedia library’s “ffmpeg” backend, but a different one is currently in use.
<nl/><nl/>
Please <link url="%1">contact the %2 developers</link> about this issue.`,
                                  KCoreAddons.KOSRelease.supportUrl, KCoreAddons.KOSRelease.name)
                }

                if (Dragon.Sandbox.inside && !Dragon.Sandbox.ffmpegFull) {
                    return xi18nc("@info",
`Not all video codecs are installed. Video playback support may be less reliable than expected.
Please install ffmpeg-full by running:
<para><command>flatpak install org.freedesktop.Platform.ffmpeg-full//24.08</command></para>`)
                }

                if (!Dragon.Sandbox.ffmpegFull) {
                    return xi18nc("@info %2 is the name of a distribution",
`Could not locate full support for the H.264 video codec; video playback support may be less reliable than expected.
<nl/><nl/>
Please <link url="%1">contact the %2 developers</link> about this issue, or to find out how to install the missing video codecs.`,
                                  KCoreAddons.KOSRelease.supportUrl, KCoreAddons.KOSRelease.name)
                }

                return ""
            }
            onLinkActivated: (url) => Qt.openUrlExternally(url)
        }

        Kirigami.InlineMessage {
            id: errorMessage
            Layout.fillWidth: true
            type: Kirigami.MessageType.Error
            position: Kirigami.InlineMessage.Position.Header
            visible: text.length > 0
            text: player.errorString
        }
    }

    function cancelFullscreen() {
        if (storedVisibility !== null) {
            visibility = storedVisibility
            storedVisibility = null
        }
    }

    function fullscreen() {
        if (storedVisibility === null) {
            storedVisibility = visibility
            visibility = Window.Window.FullScreen
            return true
        }
        return false
    }

    function toggleFullscreen() {
        if (fullscreen()) {
            return
        } else {
            cancelFullscreen()
        }
    }

    function seek(time, precise) {
        player.seek(time, precise)
    }

    WelcomeView {}

    Rectangle {
        id: videoContainer
        anchors.top: toolbarLayout.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        color: "black"
        visible: (player.source.toString()?.length > 0) ?? false // source should never be nullish so .toString() always works!

        Multimedia.VideoOutput {
            id: video
            anchors.fill: parent
        }

        Multimedia.AudioOutput {
            id: audio
            BoundaryRule on volume {
                id: volumeBoundaryRule
                minimum: 0
                maximum: 100
                overshootFilter: BoundaryRule.Peak
            }
            onVolumeChanged: (volume) => volumeWheel.rotation = volume
        }

        Multimedia.MediaPlayer {
            id: player
            videoOutput: video
            audioOutput: audio

            readonly property bool paused: playbackState == Multimedia.MediaPlayer.PausedState
            readonly property bool stopped: playbackState == Multimedia.MediaPlayer.StoppedState

            function seek(target) {
                position = target
                activeTimer.restart()
                seekInteractionTimer.restart()
            }

            function togglePause() {
                if (paused || stopped) {
                    // https://bugreports.qt.io/browse/QTBUG-135851
                    audioOutput = null
                    audioOutput = audio
                    play()
                } else {
                    pause()
                }
                activeTimer.restart()
            }

            onSourceChanged: activeTimer.restart()
        }

        WheelHandler {
            id: volumeWheel
            onWheel: player.audioOutput.volume = rotation
            // Do not let the scroll direction be inverted by natural scrolling. It'd be weird to move down but the volume goes up.
            invertible: false
            rotationScale: 0.01
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            rotation: player.audioOutput.volume
            onActiveChanged: {
                if (!active) {
                    rotation = Qt.binding(() => player.audioOutput.volume)
                    volumeBoundaryRule.returnToBounds()
                }
            }
        }

        MouseArea {
            property Timer clickTimer: Timer {
                interval: 200
                repeat: false
                onTriggered: {
                    if (videoPage.player.playbackState === Multimedia.MediaPlayer.PlayingState) {
                        videoPage.player.pause()
                    } else {
                        videoPage.player.play()
                    }
                }
            }

            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            propagateComposedEvents: true
            onDoubleClicked: {
                clickTimer.stop()
                videoPage.toggleFullscreen()
            }
            onPressed: clickTimer.start()
        }

        MouseArea {
            id: mainHoverHandler

            property var initialPoint: null
            readonly property var resetTimer: Timer {
                interval: Kirigami.Units.veryShortDuration
                repeat: false
                running: false
                onTriggered: parent.initialPoint = null
            }

            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            hoverEnabled: true
            propagateComposedEvents: true
            onPositionChanged: (event) => {
                const position = Qt.point(event.x, event.y)
                if (initialPoint === null) {
                    initialPoint = position
                    resetTimer.restart()
                    return
                } else {
                    const distance = Math.sqrt((initialPoint.x - position.x) ** 2 + (initialPoint.y - position.y) ** 2);
                    if (distance > Kirigami.Units.gridUnit) { // FIXME this should somehow relate to window size
                        activeTimer.restart()
                    }
                }
                resetTimer.restart()
            }
        }

        Rectangle {
            id: volumeDisplay

            readonly property color displayColor: "white"
            property var volume: player.audioOutput.volume
            property var hideTimer: Timer {
                interval: Kirigami.Units.humanMoment
                repeat: false
                onTriggered: volumeDisplay.opacity = 0
            }

            visible: opacity > 0
            opacity: 0
            implicitWidth: Kirigami.Units.gridUnit
            implicitHeight: video.height / 1.5
            border.color: displayColor
            border.width: 2
            anchors.right: video.right
            anchors.rightMargin: Kirigami.Units.gridUnit * 2
            y: video.height / 2 - height / 2
            color: "transparent"

            onVolumeChanged: {
                if (toolbar.volumeButton.popup.visible) {
                    return
                }
                opacity = 1
                volumeDisplay.hideTimer.restart()
            }

            Behavior on opacity {
                NumberAnimation { duration: Kirigami.Units.shortDuration }
            }

            Rectangle {
                width: parent.width - (parent.border.width * 2)
                height: parent.height * player.audioOutput.volume - (parent.border.width * 2)
                color: volumeDisplay.displayColor
                anchors.left: parent.left
                anchors.leftMargin: 2
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 2
            }
        }

        OverlayPopup {
            id: timeItem
            anchors.centerIn: parent

            visible: seekInteractionTimer.isActive

            contentItem: Kirigami.Heading  {
                Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                // Sizing is a bit complicated. We want the text to be as large as possible but not larger than the videoContainer.
                // What we do here is calculate an invisible heading that will be just right, except for the fact that it will span
                // the entire width. We later take the calculated fontInfo to set the final font size and actual width required.
                // This makes the heading as high as possible but as wide as necessary.
                property Kirigami.Heading fittingHeader: Kirigami.Heading  {
                    fontSizeMode: Text.Fit
                    wrapMode: Text.NoWrap
                    width: video.contentRect.width * 0.5

                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 10
                    minimumPixelSize: 2 // Kirigami.Theme.defaultFont.pointSize * 1.50

                    text: i18nc("@info overlay on top of video. %1 is the amount of time played %2 is the total duration of the video",
                                "%1 / %2",
                                KCoreAddons.Format.formatDuration(player.position),
                                KCoreAddons.Format.formatDuration(player.duration))
                }

                font.pointSize: fittingHeader.fontInfo.pointSize
                width: fittingHeader.fontInfo.width
                text: fittingHeader.text
            }
        }

        Timer {
            id: seekInteractionTimer
            interval: Kirigami.Units.humanMoment
            repeat: false
            // Not binding to running as after a restart it will go false then true again
            property bool isActive: false
            onRunningChanged: {
                if (running) {
                    isActive = true
                }
            }
            onTriggered: isActive = false
        }
        Timer {
            id: activeTimer
            interval: Kirigami.Units.humanMoment
            repeat: false
            property bool isActive: false
            onRunningChanged: {
                if (running) {
                    isActive = true
                }
            }
            onTriggered: isActive = false
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton // do not steal events we are purely visual
            visible: !activeTimer.isActive
            cursorShape: Qt.BlankCursor
        }

        ControlsBar {
            id: toolbar

            readonly property int switchWidth: Kirigami.Units.gridUnit * 30

            x: Math.round(parent.width / 2 - width / 2)
            y: Math.round(parent.height - height)
            width: stackedMode ? parent.width - margins * 2 : Math.max(switchWidth - margins * 2, parent.width * 0.7)
            visible: videoContainer.visible
                    && (activeTimer.isActive
                        || anyMenusOpen
                        || toolbarHandler.hovered)

            stackedMode: parent.width < switchWidth
            player: player
            WheelHandler {
                // Do not let the scroll direction be inverted by natural scrolling. It'd be weird to move down but the volume goes up.
                invertible: false
                rotationScale: 0.01
                acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
                onWheel: player.audioOutput.volume = rotation
                rotation: audioOutput.volume
            }
        }

    }

    Shortcut {
        // ApplicationShortcut needed to not be stolen by OverlayPopup instances
        context: Qt.ApplicationShortcut
        sequences: ["Space", Qt.Key_MediaPlay]
        onActivated: togglePauseAction.trigger()
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: StandardKey.Cancel
        onActivated: videoPage.cancelFullscreen()
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: "Left"
        onActivated: videoPage.seek(toolbar.seekSlider.value - 5000, true)
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: "Right"
        onActivated: videoPage.seek(toolbar.seekSlider.value + 5000, true)
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: "Ctrl+Left"
        onActivated: videoPage.seek(toolbar.seekSlider.value - 60000, true)
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: "Ctrl+Right"
        onActivated: videoPage.seek(toolbar.seekSlider.value + 60000, true)
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: "Ctrl+Alt+Left"
        onActivated: videoPage.seek(toolbar.seekSlider.value - (5 * 60000), true)
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: "Ctrl+Alt+Right"
        onActivated: videoPage.seek(toolbar.seekSlider.value + (5 * 60000), true)
    }
}
