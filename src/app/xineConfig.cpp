// (C) 2005 Max Howell (max.howell@methylblue.com)
// See COPYING file for licensing information

#include "debug.h"
#include <kapplication.h> // XineConfigDialog::ctor -> to get the iconloader
#include <kcombobox.h>
#include <kiconloader.h>  // XineConfigDialog::ctor
#include <klineedit.h>
#include <kseparator.h>
#include <kstdguiitem.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <q3scrollview.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qtooltip.h>
#include <q3vbox.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3Frame>
#include <QPixmap>
#include <Q3VBoxLayout>
#include <xine.h>
#include "xineConfig.h"

QString i18n(const char *text);


KDialogBase *XineConfigDialog::s_instance = 0;


namespace Codeine
{
   void
   showXineConfigurationDialog( QWidget *parent, xine_t *xine )
   {
      XineConfigDialog d( xine, parent );
      if( d.exec() == QDialog::Accepted )
         d.saveSettings();
   }
}


class TabWidget : public QTabWidget
{
public:
   TabWidget( QWidget *parent ) : QTabWidget( parent ) {}

   virtual QSize sizeHint() const
   {
      // Qt gives a stupid default sizeHint for this widget
      return QSize(
            reinterpret_cast<QWidget*>(tabBar())->sizeHint().width() + 5,
            QTabWidget::sizeHint().height() );
   }
};


///@class XineConfigDialog

XineConfigDialog::XineConfigDialog( xine_t *xine, QWidget *parent )
      : KDialogBase( parent, "xine_config_dialog",
               true, //modal
               i18n("Configure xine"), User1 | Stretch | Ok | Cancel,
               Ok, //default button
               false, //draw separator
               KStandardGuiItem::reset() )
      , m_xine( xine )
{
   DEBUG_BLOCK

   s_instance = this;
   const int METRIC = fontMetrics().width( 'x' );
   const int METRIC_3B2 = (3*METRIC)/2;

   Q3VBox *box = new Q3VBox( this );
   box->setSpacing( METRIC );
   setMainWidget( box );

   {
      Q3HBox *hbox = new Q3HBox( box );
      hbox->setSpacing( METRIC_3B2 );
      hbox->setMargin( METRIC_3B2 );
      QPixmap info = kapp->iconLoader()->loadIcon( "messagebox_info", K3Icon::NoGroup, K3Icon::SizeMedium, K3Icon::DefaultState, 0, true );
      QLabel *label = new QLabel( hbox );
      label->setPixmap( info );
      label->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
      label = new QLabel( i18n(
            "xine's defaults are usually sensible and should not require modification. "
            "However, full configurability is provided for your pleasure ;-)." ), hbox );
      label->setAlignment( QLabel::WordBreak | QLabel::AlignVCenter );
   }

   //FIXME after many hours I have discovered that this
   // widget somehow sets the minSize of this widget to 0,0
   // whenever you resize the widget. WTF?
   TabWidget *tabs = new TabWidget( box );


   class XineConfigEntryIterator {
      xine_t *m_xine;
      xine_cfg_entry_t m_entry;
      bool m_valid;
   public:
      XineConfigEntryIterator( xine_t *xine ) : m_xine( xine ) { m_valid = xine_config_get_first_entry( m_xine, &m_entry ); }
      inline XineConfigEntryIterator &operator++() { m_valid = xine_config_get_next_entry( m_xine, &m_entry ); return *this; }
      inline xine_cfg_entry_t *operator*() { return m_valid ? &m_entry : 0; }
   };


   Q3GridLayout *grid = 0;
   QString currentPage;
   Q3ScrollView *view = 0;
   parent = 0;

   for( XineConfigEntryIterator it( m_xine ); *it; ++it )
   {
      const QString pageName = QString::fromUtf8( (*it)->key ).section( '.', 0, 0 );

      if( (QStringList() << "ui" << "effects" << "subtitles").contains( pageName ) )
         continue;

      if( pageName != currentPage ) {
         if( view )
            //NOTE won't be executed for last tab
            view->viewport()->setMinimumWidth( grid->sizeHint().width() ); // seems necessary

         QString pageTitle = pageName;
         pageTitle[0] = pageTitle[0].upper();

         tabs->addTab( view = new Q3ScrollView, pageTitle );
         view->setResizePolicy( Q3ScrollView::AutoOneFit );
         view->setHScrollBarMode( Q3ScrollView::AlwaysOff );
         view->setFrameShape( Q3Frame::NoFrame );
         view->addChild( parent = new QWidget( view->viewport() ) );

         Q3BoxLayout *layout = new Q3VBoxLayout( parent, /*margin*/METRIC_3B2, /*spacing*/0 );

         parent = new Q3Frame( parent );
         static_cast<Q3Frame*>(parent)->setFrameStyle( Q3Frame::Panel | Q3Frame::Raised );
         static_cast<Q3Frame*>(parent)->setLineWidth( 2 );
         grid = new Q3GridLayout( parent, /*rows*/0, /*cols*/2, /*margin*/20, /*spacing*/int(METRIC*2.5) );
         grid->setColStretch( 0, 3 );
         grid->setColStretch( 1, 2 );

         layout->addWidget( parent, 0 );
         layout->addStretch( 1 );

         currentPage = pageName;
      }

      m_entrys.append( new XineConfigEntry( parent, grid, *it ) );
   }

   //finishing touches
   m_entrys.setAutoDelete( true );
   enableButton( Ok, false );
   enableButton( User1, false );

   Q_ASSERT( !isUnsavedSettings() );
}

void
XineConfigDialog::slotHelp()
{
   /// HACK called when a widget's input value changes

   const bool b = isUnsavedSettings();
   enableButton( Ok, b );
   enableButton( User1, b );
}

void
XineConfigDialog::slotUser1()
{
   for( Q3PtrListIterator<XineConfigEntry> it( m_entrys ); *it != 0; ++it )
      (*it)->reset();

   slotHelp();
}

bool
XineConfigDialog::isUnsavedSettings() const
{
   for( Q3PtrListIterator<XineConfigEntry> it( m_entrys ); *it != 0; ++it )
      if( (*it)->isChanged() )
         return true;

   return false;
}

#include <qdir.h>
void
XineConfigDialog::saveSettings()
{
   for( XineConfigEntry *entry = m_entrys.first(); entry; entry = m_entrys.next() )
      if( entry->isChanged() )
         entry->save( m_xine );

   xine_config_save( m_xine, QFile::encodeName( QDir::homePath() + "/.xine/config" ) );
}


///@class XineConfigEntry

XineConfigEntry::XineConfigEntry( QWidget *parent, Q3GridLayout *grid, xine_cfg_entry_t *entry )
      : m_widget( 0 )
      , m_key( entry->key )
      , m_string( entry->str_value )
      , m_number( entry->num_value )
{
   QWidget *&w = m_widget;
   const char *signal = 0;
   const int row = grid->numRows();

   QString description_text = QString::fromUtf8( entry->description );
   description_text[0] = description_text[0].upper();

   switch( entry->type )
   {
   case XINE_CONFIG_TYPE_STRING: {
      w = new KLineEdit( m_string, parent );
      signal = SIGNAL(textChanged( const QString& ));
      break;
   }
   case XINE_CONFIG_TYPE_ENUM: {
      w = new KComboBox( parent );
      for( int i = 0; entry->enum_values[i]; ++i )
         ((KComboBox*)w)->insertItem( QString::fromUtf8( entry->enum_values[i] ) );
      ((KComboBox*)w)->setCurrentItem( m_number );
      signal = SIGNAL(activated( int ));
      break;
   }
   case XINE_CONFIG_TYPE_RANGE:
   case XINE_CONFIG_TYPE_NUM: {
      w = new QSpinBox(
               qMin( m_number, entry->range_min ), // xine bug, sometimes the min and max ranges
               qMax( m_number, entry->range_max ), // are both 0 even though this is bullshit
               1, parent );
      ((QSpinBox*)w)->setValue( m_number );
      signal = SIGNAL(valueChanged( int ));
      break;
   }
   case XINE_CONFIG_TYPE_BOOL: {
      w = new QCheckBox( description_text, parent );
      ((QCheckBox*)w)->setChecked( m_number );

      connect( w, SIGNAL(toggled( bool )), XineConfigDialog::instance(), SLOT(slotHelp()) );
      QToolTip::add( w, "<qt>" + QString::fromUtf8( entry->help ) );
      grid->addMultiCellWidget( w, row, row, 0, 1 );
      return; //no need for a description label
   }
   default:
      ;
   }

   connect( w, signal, XineConfigDialog::instance(), SLOT(slotHelp()) );

   QLabel *description = new QLabel( parent );
   description->setBuddy( description_text + ':' );
   description->setAlignment( Qt::TextWordWrap | Qt::AlignVCenter );

   const QString tip = "<qt>" + QString::fromUtf8( entry->help );
   QToolTip::add( w, tip );
   QToolTip::add( description, tip );

//   grid->addWidget( description, row, 0, Qt::AlignVCenter );
   grid->addWidget( w, row, 1, Qt::AlignTop );
}

bool
XineConfigEntry::isChanged() const
{
   #define _( x ) static_cast<x*>(m_widget)

   switch( classType( m_widget->className() ) ) {
      case LineEdit: return _(KLineEdit)->text().utf8() != m_string;
      case ComboBox: return _(KComboBox)->currentItem() != m_number;
      case SpinBox:  return _(QSpinBox)->value() != m_number;
      case CheckBox: return _(QCheckBox)->isChecked() != m_number;
   }
   return false;
}

void
XineConfigEntry::reset()
{
   // this is because we only get called by the XineConfigDialog reset button
   // and we don't want to cause a check for Ok/Reset button enabled state for
   // every XineConfigEntry
   m_widget->blockSignals( true );

   switch( classType( m_widget->className() ) ) {
      case LineEdit: _(KLineEdit)->setText( m_string ); break;
      case ComboBox: _(KComboBox)->setCurrentItem( m_number ); break;
      case SpinBox:  _(QSpinBox)->setValue( m_number ); break;
      case CheckBox: _(QCheckBox)->setChecked( (bool)m_number ); break;
   }
   m_widget->blockSignals( false );
}

void
XineConfigEntry::save( xine_t *xine )
{
   xine_cfg_entry_t ent;

   if( xine_config_lookup_entry( xine, key(), &ent ) )
   {
      switch( classType( m_widget->className() ) ) {
         case LineEdit: m_string = _(KLineEdit)->text().utf8(); break;
         case ComboBox: m_number = _(KComboBox)->currentItem(); break;
         case SpinBox:  m_number = _(QSpinBox)->value(); break;
         case CheckBox: m_number = _(QCheckBox)->isChecked(); break;
      }

      ent.str_value = qstrdup( m_string );
      ent.num_value = m_number;

      debug() << "Saving setting: " << key() << endl;
      xine_config_update_entry( xine, &ent );
   }
   else
      Debug::warning() << "Couldn't save: " << key() << endl;

   #undef _
}
