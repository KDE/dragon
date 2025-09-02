// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>

#include <KAboutData>
#include <KCrash>
#include <KLocalizedQmlContext>
#include <KLocalizedString>

#include "dragon.h"
#include "renderer.h"

using namespace Qt::StringLiterals;

int main(int argc, char **argv)
{
    // Needs to be a QApplication rather than QGuiApplication so filedialog actually uses kio.
    QApplication app(argc, argv);

    if (Renderer::isAMD()) {
        // https://bugreports.qt.io/browse/QTBUG-138679
#if QT_VERSION >= QT_VERSION_CHECK(6, 12, 0)
#error "Reevaluate the use of QT_DISABLE_HW_TEXTURES_CONVERSION at a later point in time"
#endif
        qWarning() << "Detected AMD GPU, disabling HW Texture Conversion renderer as it is known to cause issues.";
        qputenv("QT_DISABLE_HW_TEXTURES_CONVERSION", "1");
    }

    KAboutData aboutData(u"dragonplayer"_s,
                         i18n("Dragon Player"),
                         Dragon::version,
                         i18n("A video player that has a usability focus"),
                         KAboutLicense::GPL_V2,
                         i18n("Copyright 2006, Max Howell\nCopyright 2007, Ian Monroe\nCopyright 2022 Harald Sitter"),
                         QString() /* otherText */,
                         u"https://commits.kde.org/dragon"_s);
    aboutData.setDesktopFileName(Dragon::desktopFileName);

    KAboutData::setApplicationData(aboutData);
    KCrash::initialize();

    QApplication::setWindowIcon(QIcon::fromTheme(u"dragonplayer"_s));

    auto engine = new QQmlApplicationEngine(&app); // on the heap for code similarity reasons with other projects, technically not necessary.
    KLocalization::setupLocalizedContext(engine);

    engine->loadFromModule("org.kde.dragon", "Main");
    if (engine->rootObjects().isEmpty()) {
        qWarning() << "Failed to load QML code. Note prior errors!";
        abort();
        return -1;
    }

    return app.exec();
}
