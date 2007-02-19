// (c) 2004 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINELISTVIEW_CPP
#define CODEINELISTVIEW_CPP

#include <k3listview.h>

namespace Codeine
{
   class ListView : public K3ListView
   {
   public:
      ListView( QWidget *parent ) : K3ListView( parent )
      {
         addColumn( QString::null, 0 );
         addColumn( QString::null );

         setResizeMode( LastColumn );
         setMargin( 2 );
         setSorting( -1 );
         setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
         setAllColumnsShowFocus( true );
         setItemMargin( 3 );
      }

      virtual QSize sizeHint() const
      {
         const QSize sh = K3ListView::sizeHint();

         return QSize( sh.width(),
            childCount() == 0
               ? 50
               : qMin( sh.height(), childCount() * (firstChild()->height()) + margin() * 2 + 4 + reinterpret_cast<QWidget*>(header())->height() ) );
      }
   };
}

#endif
