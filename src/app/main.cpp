/***********************************************************************
 * Copyright 2004  Max Howell <max.howell@methylblue.com>
 *           2007  Ian Monroe <ian@monroe.nu>
 * Copyright 2014 Lukáš Tinkl <lukas@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************/
#include "codeine.h"
#include "playerApplication.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>

#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>
#include <KDBusService>

int main( int argc, char **argv )
{
    Dragon::PlayerApplication app(argc, argv);
    app.setOrganizationDomain("org.kde");
#ifdef WITH_KCRASH_INIT
    KCrash::initialize();
#endif

    KLocalizedString::setApplicationDomain("dragonplayer");

    KAboutData aboutData( APP_NAME, i18n("Dragon Player"), QLatin1Literal(APP_VERSION),
                          i18n("A video player that has a usability focus"), KAboutLicense::GPL_V2,
                          i18n("Copyright 2006, Max Howell\nCopyright 2007, Ian Monroe"),
                          i18n("IRC:\nirc.freenode.net #dragonplayer\n\nFeedback:\nimonroe@kde.org"),
                          "http://multimedia.kde.org" );
    aboutData.addCredit( QStringLiteral("David Edmundson"), i18n("Improvements and polish") );
    aboutData.addCredit( QStringLiteral("Matthias Kretz"), i18n("Creator of Phonon") );
    aboutData.addCredit( QStringLiteral("Eugene Trounev"), i18n("Dragon Player icon") );
    aboutData.addCredit( QStringLiteral("Mike Diehl"), i18n("Handbook") );
    aboutData.addCredit( QStringLiteral("The Kaffeine Developers"), i18n("Great reference code") );
    aboutData.addCredit( QStringLiteral("Greenleaf"), i18n("Yatta happened to be the only video on my laptop to test with. :)") );
    aboutData.addCredit( QStringLiteral("Eike Hein"), i18n("MPRIS v2 support") );
    aboutData.addCredit( QStringLiteral("Lukáš Tinkl"), i18n("Port to KF5/Plasma 5"), QStringLiteral("lukas@kde.org") );

    KAboutData::setApplicationData(aboutData);
    KDBusService service(KDBusService::Unique);

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.addVersionOption();
    parser.addHelpOption();

    parser.addOption(QCommandLineOption("play-dvd", i18n("Play DVD Video")));
    parser.addPositionalArgument("url", i18n("Play 'URL'"), QStringLiteral("+[URL]"));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    QObject::connect(&service, &KDBusService::activateRequested, &app, &Dragon::PlayerApplication::slotActivateRequested);
    QObject::connect(&service, &KDBusService::openRequested, &app, &Dragon::PlayerApplication::slotOpenRequested);

    QList<QUrl> urls;
    const QStringList args = parser.positionalArguments();
    if (!args.isEmpty()) {
        urls.append(QUrl::fromUserInput(args.first()));
    }

    app.newInstance(parser.isSet("play-dvd"), urls);

    return app.exec();
}
