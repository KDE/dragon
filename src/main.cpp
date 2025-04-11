// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>

#include <KAboutData>
#include <KCrash>
#include <KLocalizedQmlContext>
#include <KLocalizedString>

#include "dragon.h"

using namespace Qt::StringLiterals;

int main(int argc, char **argv)
{
    // Needs to be a QApplication rather than QGuiApplication so filedialog actually uses kio.
    QApplication app(argc, argv);

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
