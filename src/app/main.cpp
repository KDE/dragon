/***********************************************************************
 * Copyright 2004  Max Howell <max.howell@methylblue.com>
 *           2007  Ian Monroe <ian@monroe.nu>
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

#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>
#include <KLocalizedString>

static KAboutData aboutData( APP_NAME, 0,
        ki18n("Dragon Player"), APP_VERSION,
        ki18n("A video player that has a usability focus"), KAboutData::License_GPL_V2,
        ki18n("Copyright 2006, Max Howell\nCopyright 2007, Ian Monroe"), ki18n("IRC:\nirc.freenode.net #dragonplayer\n\nFeedback:\nimonroe@kde.org"),
        "http://multimedia.kde.org" );

int
main( int argc, char **argv )
{
    aboutData.addCredit( ki18n("David Edmundson"), ki18n("Improvements and polish") );
    aboutData.addCredit( ki18n("Matthias Kretz"), ki18n("Creator of Phonon") );
    aboutData.addCredit( ki18n("Eugene Trounev"), ki18n("Dragon Player icon") );
    aboutData.addCredit( ki18n("Mike Diehl"), ki18n("Handbook") );
    aboutData.addCredit( ki18n("The Kaffeine Developers"), ki18n("Great reference code") );
    aboutData.addCredit( ki18n("Greenleaf"), ki18n("Yatta happened to be the only video on my laptop to test with. :)") );
    aboutData.addCredit( ki18n("Eike Hein"), ki18n("MPRIS v2 support") );

    KCmdLineArgs::init( argc, argv, &aboutData );

    KCmdLineOptions options;
    options.add("+[URL]", ki18n( "Play 'URL'" ));
    options.add("play-dvd", ki18n( "Play DVD Video" ));
    KCmdLineArgs::addCmdLineOptions( options );
    KUniqueApplication::addCmdLineOptions();

    Dragon::PlayerApplication application;
    return application.exec();
}
