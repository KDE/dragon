// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include "mxcl.library.h"
#include <qapplication.h>
#include <kcursor.h>


namespace mxcl
{
   WaitCursor::WaitCursor()
   {
      QApplication::setOverrideCursor( KCursor::waitCursor() );
   }

   WaitCursor::~WaitCursor()
   {
      QApplication::restoreOverrideCursor();
   }
}
