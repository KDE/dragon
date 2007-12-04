// (c) 2004 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINECONFIG_H
#define CODEINECONFIG_H

#include <kconfig.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>
#include <kglobal.h>

namespace Codeine
{
   static KConfigGroup config( const QString &group )
   {
      return KConfigGroup( KGlobal::config(), group );
   }
}

#endif
