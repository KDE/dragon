// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINE_VIDEO_SETTINGS_H
#define CODEINE_VIDEO_SETTINGS_H

#include "codeine.h"
#include <kdialog.h>


namespace Codeine
{
   class VideoSettingsDialog : public KDialog
   {
      VideoSettingsDialog(); //disable
      VideoSettingsDialog( const VideoSettingsDialog& ); //disable
      VideoSettingsDialog &operator=( const VideoSettingsDialog& ); //disable

   public:
      VideoSettingsDialog( QWidget *parent );

      static void stateChanged( QWidget *parent, Engine::State );
   };
}

#endif
