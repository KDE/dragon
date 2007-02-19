// (c) 2004 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINECONFIG_H
#define CODEINECONFIG_H

#include <kconfig.h>
#include <kglobal.h>

namespace Codeine
{
   static KSharedConfig::Ptr config( const QString &group )
   {
      KSharedConfig::Ptr instance = KGlobal::config();
      instance->setGroup( group );
      return instance;
   }
}

#endif
