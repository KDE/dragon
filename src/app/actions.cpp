// Copyright 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include "actions.h"
#include "debug.h"
#include "mxcl.library.h"
#include <qtoolbutton.h>
#include "xineEngine.h"

namespace Codeine
{
   PlayAction::PlayAction( QObject *receiver, const char *slot, KActionCollection *ac )
         : KToggleAction( i18n("Play"), "player_play", Qt::Key_Space, receiver, slot, ac, "play" )
   {}

   void
   PlayAction::setChecked( bool b )
   {
      if( videoWindow()->state() == Engine::Empty && sender() && QCString(sender()->className()) == "KToolBarButton" ) {
         // clicking play when empty means open PlayMediaDialog, but we have to uncheck the toolbar button
         // as KDElibs sets that checked automatically..
         ((QToolButton*)sender())->setOn( false );
      }
      else
         KToggleAction::setChecked( b );
   }
}
