// (c) 2004 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include "codeine.h"
#include "mainWindow.h"

#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>
#include <KLocalizedString>

static KAboutData aboutData( APP_NAME, 0,
        ki18n(PRETTY_NAME), APP_VERSION,
        ki18n("A video player that has a usability focus"), KAboutData::License_GPL_V2,
        ki18n("Copyright 2006, Max Howell\nCopyright 2007, Ian Monroe"), KLocalizedString(),
        "http://www.methylblue.com/codeine/",
        "ian@monroe.nu" );

int
main( int argc, char **argv )
{
    //we need to do this, says adrianS from SuSE
//    if( !XInitThreads() )
//        return 1;

    aboutData.addCredit( ki18n("Mike Diehl"), ki18n("Handbook") );
    aboutData.addCredit( ki18n("The Kaffeine Developers"), ki18n("Great reference code") );
    aboutData.addCredit( ki18n("Greenleaf"), ki18n("Yatta happened to be the only video on my laptop to test with. :)") );
    aboutData.addCredit( ki18n("David Vignoni"), ki18n("The current Codeine icon") );


    KCmdLineArgs::init( argc, argv, &aboutData );

    KCmdLineOptions options;
    options.add("+[URL]", ki18n( "Play 'URL'" ));
    options.add("play-dvd", ki18n( "Play DVD Video" ));
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication application;
    int returnValue;

    {
        Codeine::MainWindow mainWindow;
        mainWindow.show();

        returnValue = application.exec();
    }

    return returnValue;
}
