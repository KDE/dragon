// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINEPLAYDIALOG_H
#define CODEINEPLAYDIALOG_H

#include <kurl.h>
#include <qdialog.h>

class KListView;
class Q3BoxLayout;
class Q3ListViewItem;

namespace Codeine
{
   class PlayDialog : public QDialog
   {
   Q_OBJECT
   public:
      PlayDialog( QWidget*, bool show_welcome_dialog = false );

      const KURL &url() const { return m_url; }

      enum DialogCode { FILE = QDialog::Accepted + 2, VCD, CDDA, DVD, RECENT_FILE };

   private slots:
      void done( Q3ListViewItem* );

   private:
      void createRecentFileWidget( Q3BoxLayout* );

      KURL m_url;
   };
}

#endif
