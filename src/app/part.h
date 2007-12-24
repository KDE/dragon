// Author:    Max Howell <max.howell@methylblue.com>, (C) 2005
// Copyright: See COPYING file that comes with this distribution

#ifndef CODEINE_PART_H
#define CODEINE_PART_H

#include <kparts/statusbarextension.h>
#include <kparts/part.h>
#include <kurl.h>

class KAboutData;
class QSlider;


namespace Codeine
{
   class Part : public KParts::ReadOnlyPart
   {
   public:
      Part(QWidget* parentWidget, QObject* parent, const QStringList& /*args*/ );

      virtual bool openFile() { return false; } //pure virtual in base class
      virtual bool openURL( const KUrl& );
      virtual bool closeURL();

      static KAboutData *createAboutData();

   private:
      KUrl m_url;
      KParts::StatusBarExtension *m_statusBarExtension;
      QSlider *m_slider;

      KStatusBar *statusBar() { return m_statusBarExtension->statusBar(); }

   };
}

#endif
