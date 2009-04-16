/************************************************************************
 * Copyright 2005  Max Howell <max.howell@methylblue.com>
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

#ifndef DRAGONPLAYER_EXTERN_H
#define DRAGONPLAYER_EXTERN_H

#include <QMenu>
//I think this whole file is deprecated - Dave
extern "C"
{
   typedef struct xine_s xine_t;
}

class QWidget;

namespace Dragon
{
   class VideoWindow;
   

   void showVideoSettingsDialog( QWidget* );
   void showXineConfigurationDialog( QWidget*, xine_t* );
   void insertAspectRatioMenuItems( QMenu* );
}

#endif
