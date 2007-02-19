// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#ifndef XINECONFIG_H
#define XINECONFIG_H

#include <kdialogbase.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3CString>

class KComboBox;
class KLineEdit;
class QCheckBox;
class Q3GridLayout;
class QSpinBox;

typedef struct xine_s xine_t;
typedef struct xine_cfg_entry_s xine_cfg_entry_t;


///stores a single config entry of the config file

class XineConfigEntry : public QObject
{
   enum ClassType { LineEdit, ComboBox, SpinBox, CheckBox };

   QWidget *m_widget;
   Q3CString m_key;
   Q3CString m_string;
   int      m_number;

   static inline ClassType classType( const Q3CString &name )
   {
      return name == "KLineEdit" ? LineEdit
           : name == "KComboBox" ? ComboBox
           : name == "QSpinBox" ? SpinBox : CheckBox;
   }

public:
   XineConfigEntry( QWidget *parent, Q3GridLayout*, xine_cfg_entry_t* );

   bool isChanged() const;
   void save( xine_t* );
   void reset();

   inline const Q3CString &key() const { return m_key; }
};


class XineConfigDialog : public KDialogBase
{
   static KDialogBase *s_instance;

   Q3PtrList<XineConfigEntry> m_entrys;
   xine_t *m_xine;

public:
   XineConfigDialog( xine_t *xine, QWidget *parent );

   bool isUnsavedSettings() const;
   void saveSettings();

   static KDialogBase *instance() { return s_instance; }

protected:
   virtual void slotUser1();
   virtual void slotHelp();
};

#endif
