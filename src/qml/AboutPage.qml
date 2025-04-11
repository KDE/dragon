// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

import QtQuick
import org.kde.kirigami as Kirigami
import org.kde.coreaddons as KCoreAddons

Kirigami.AboutPage {
    aboutData: KCoreAddons.AboutData
    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.ToolBar
}
