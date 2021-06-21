/*
    SPDX-FileCopyrightText: 2004 Max Howell <max.howell@methylblue.com>
    SPDX-FileCopyrightText: 2007 Ian Monroe <ian@monroe.nu>
    SPDX-FileCopyrightText: 2014 Lukáš Tinkl <lukas@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "codeine.h"
#include "playerApplication.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QDir>
#include <QUrl>

#include <KAboutData>
#include <KConfigGroup>
#include <KCrash>
#include <KSharedConfig>
#include <KLocalizedString>
#include <KDBusService>

int main( int argc, char **argv )
{
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    Dragon::PlayerApplication app(argc, argv);
    KCrash::initialize();

    KLocalizedString::setApplicationDomain("dragonplayer");

    KAboutData aboutData( QStringLiteral(APP_NAME),
                          i18n("Dragon Player"),
                          QStringLiteral(APP_VERSION),
                          i18n("A video player that has a usability focus"), KAboutLicense::GPL_V2,
                          i18n("Copyright 2006, Max Howell\nCopyright 2007, Ian Monroe"),
                          QString(),
                          QStringLiteral("https://commits.kde.org/dragon"));
    aboutData.setDesktopFileName(QStringLiteral("org.kde.dragonplayer"));
    aboutData.addCredit( QStringLiteral("David Edmundson"), i18n("Improvements and polish") );
    aboutData.addCredit( QStringLiteral("Matthias Kretz"), i18n("Creator of Phonon") );
    aboutData.addCredit( QStringLiteral("Eugene Trounev"), i18n("Dragon Player icon") );
    aboutData.addCredit( QStringLiteral("Mike Diehl"), i18n("Handbook") );
    aboutData.addCredit( QStringLiteral("The Kaffeine Developers"), i18n("Great reference code") );
    aboutData.addCredit( QStringLiteral("Greenleaf"), i18n("Yatta happened to be the only video on my laptop to test with. :)") );
    aboutData.addCredit( QStringLiteral("Eike Hein"), i18n("MPRIS v2 support") );
    aboutData.addCredit( QStringLiteral("Lukáš Tinkl"), i18n("Port to KF5/Plasma 5"), QStringLiteral("lukas@kde.org") );

    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    const QCommandLineOption playDvDOption(QStringLiteral("play-dvd"), i18n("Play DVD Video"));
    parser.addOption(playDvDOption);
    parser.addPositionalArgument(QStringLiteral("url"), i18n("Play 'URL'"), QStringLiteral("+[URL]"));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    const bool multiple = KSharedConfig::openConfig()->group("KDE").readEntry("MultipleInstances", QVariant(false)).toBool();
    KDBusService service(multiple ? KDBusService::Multiple : KDBusService::Unique);
    QObject::connect(&service, &KDBusService::activateRequested, &app, &Dragon::PlayerApplication::slotActivateRequested);
    QObject::connect(&service, &KDBusService::openRequested, &app, &Dragon::PlayerApplication::slotOpenRequested);

    QList<QUrl> urls;
    const QStringList args = parser.positionalArguments();
    if (!args.isEmpty()) {
        urls.append(QUrl::fromUserInput(args.first(),
                                        QDir::currentPath(),
                                        QUrl::AssumeLocalFile));
    }

    app.newInstance(parser.isSet(playDvDOption), urls);

    return app.exec();
}
