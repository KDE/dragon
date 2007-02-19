// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef MXCL_LIBRARY_H
#define MXCL_LIBRARY_H


namespace mxcl
{
   /// Allocate on stack, wait cursor will be shown during existance
   struct WaitCursor
   {
      WaitCursor();
     ~Qt::WaitCursor();
   };
}


/// almost always negates the need to #include <klocale.h> in implementations
#include <qstring.h>
QString i18n( const char *text );


/// very useful for QStringLists
#define foreach( x ) \
   for( QStringList::ConstIterator it = x.constBegin(), end = x.constEnd(); it != end; ++it )

#endif
