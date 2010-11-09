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

#ifndef DRAGONPLAYERACTIONS_H
#define DRAGONPLAYERACTIONS_H

#include <KDualAction>      //baseclass
#include <KToggleAction>    //baseclass
#include <KActionCollection> //convenience

namespace Dragon
{
   KActionCollection *actionCollection(); ///defined in mainWindow.cpp, part.cpp
   QAction *action( const char* ); ///defined in mainWindow.cpp, part.cpp
   inline KToggleAction *toggleAction( const char *name ) { return (KToggleAction*)action( name ); }

   class PlayAction : public KDualAction
   {
   Q_OBJECT
   public:
      PlayAction( QObject *receiver, const char *slot, KActionCollection* );
      void setPlaying( bool playing );
   };

   class VolumeAction : public KToggleAction
   {
   Q_OBJECT
   public:
        VolumeAction( QObject *receiver, const char *slot, KActionCollection* );
   private slots:
        void mutedChanged( bool );
   };
}

#endif

