// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef CODEINEPLAYDIALOG_H
#define CODEINEPLAYDIALOG_H

#include <KUrl>
#include <QDialog>

class K3ListView;
class Q3ListViewItem;
class QBoxLayout;

namespace Codeine
{
   class PlayDialog : public QDialog
   {
   Q_OBJECT
   public:
      PlayDialog( QWidget*, bool show_welcome_dialog = false );

      const KUrl &url() const { return m_url; }

      enum DialogCode { FILE = QDialog::Accepted + 2, VCD, CDDA, DVD, RECENT_FILE };

   private slots:
      void done( Q3ListViewItem* );

   private:
      void createRecentFileWidget( QBoxLayout* );

      KUrl m_url;
   };
}

#endif
