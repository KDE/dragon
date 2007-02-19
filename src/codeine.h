// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINE_H
#define CODEINE_H

// try to keep this file light. It gets included by
// practically every implementation and many headers

namespace Engine
{
   enum State
   {
      Uninitialised = 0,
      Empty = 1,
      Loaded = 2,
      Playing = 4,
      Paused = 8,
      TrackEnded = 16
   };
}

class QWidget;

namespace Analyzer
{
   static const int SCOPE_SIZE_EXP = 9;
   static const int SCOPE_SIZE = 1 << SCOPE_SIZE_EXP;
}

namespace Codeine
{
   QWidget *mainWindow(); //defined in mainWindow.cpp
}

/// used by mainWindow.h and xineEngine.h
int main( int, char** );

#define APP_VERSION "1.0.1"
#define APP_NAME "codeine"
#define PRETTY_NAME "Codeine"

#endif
