// Copyright 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include <qpopupmenu.h>
#include <xine.h>

QString i18n( const char *text );


namespace Codeine
{
   void
   insertAspectRatioMenuItems( QPopupMenu *menu )
   {
      menu->insertItem( i18n( "Determine &Automatically" ), XINE_VO_ASPECT_AUTO );
      menu->insertSeparator();
      menu->insertItem( i18n( "&Square (1:1)" ), XINE_VO_ASPECT_SQUARE );
      menu->insertItem( i18n( "&4:3" ), XINE_VO_ASPECT_4_3 );
      menu->insertItem( i18n( "Ana&morphic (16:9)" ), XINE_VO_ASPECT_ANAMORPHIC );
      menu->insertItem( i18n( "&DVB (2.11:1)" ), XINE_VO_ASPECT_DVB );

      menu->setItemChecked( XINE_VO_ASPECT_AUTO, true );
   }
}
