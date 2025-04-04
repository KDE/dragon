// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

import QtQuick
import QtQuick.Dialogs as Dialogs
import QtCore as Core

import org.kde.kirigami as Kirigami
import org.kde.config as KConfig

import org.kde.dragon

Kirigami.ApplicationWindow {
    id: appWindow

    property PlayerPage playerPage: PlayerPage {
        fullscreenAction: mpris2.fullscreenAction
    }

    property MPRIS2 mpris2: MPRIS2 {
        id: mpris2
        player: appWindow.playerPage.player

        property Kirigami.Action kQuit: Kirigami.Action {
            fromQAction: mpris2.quitAction
            onTriggered: appWindow.close()
        }

        property Kirigami.Action kRaise: Kirigami.Action {
            fromQAction: mpris2.raiseAction
            onTriggered: appWindow.raise()
        }
    }

    property FileOpen fileDialog: FileOpen {
        onAccepted: {
            playerPage.player.source = selectedUrl
            playerPage.player.play()
        }
    }

    property Core.Settings setttings: Core.Settings {
        property alias lastFolder: appWindow.fileDialog.currentFolder
    }

    property Kirigami.Action openAction: Kirigami.Action {
        text: i18nc("@action:button open file dialog", "Open…")
        icon.name: "document-open"
        onTriggered: appWindow.fileDialog.open(appWindow)
        tooltip: text
    }

    title: {
        const title = playerPage.player.metaData.value(0)
        if (title) {
            return title
        }
        return playerPage.player.source
    }
    minimumWidth: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.gridUnit * 30
    minimumHeight: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.gridUnit * 22
    pageStack.initialPage: playerPage

    KConfig.WindowStateSaver {
        configGroupName: "MainWindow"
    }

    Component.onCompleted: {
        if (Application.arguments.length < 2) {
            return
        }
        playerPage.player.source = Qt.resolvedUrl(Application.arguments[1], new QtObject() /* don't resolve relative to qrc:/ (the engine's base url) */)
        playerPage.player.play()
    }
}
