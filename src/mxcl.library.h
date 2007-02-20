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
     ~WaitCursor();
   };
}

#endif
