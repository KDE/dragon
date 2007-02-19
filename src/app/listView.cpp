// (c) 2004 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINELISTVIEW_CPP
#define CODEINELISTVIEW_CPP

#include <klistview.h>

namespace Codeine
{
   class ListView : public KListView
   {
   public:
      ListView( QWidget *parent ) : KListView( parent )
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
         const QSize sh = KListView::sizeHint();

         return QSize( sh.width(),
            childCount() == 0
               ? 50
               : QMIN( sh.height(), childCount() * (firstChild()->height()) + margin() * 2 + 4 + reinterpret_cast<QWidget*>(header())->height() ) );
      }
   };
}

#endif
