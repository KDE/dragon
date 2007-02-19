// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINE_EXTERN_H
#define CODEINE_EXTERN_H

extern "C"
{
   typedef struct xine_s xine_t;
}

class QPopupMenu;
class QWidget;

namespace Codeine
{
   class VideoWindow;
   class XineEngine;

   VideoWindow* const engine(); //defined in xineEngine.h
   VideoWindow* const videoWindow(); //defined in xineEngine.h

   void showVideoSettingsDialog( QWidget* );
   void showXineConfigurationDialog( QWidget*, xine_t* );
   void insertAspectRatioMenuItems( QPopupMenu* );
}

#endif
