#ifndef LOADVIEW_H
#define LOADVIEW_H
#include <QWidget>
#include <KUrl>
#include "ui_loadView.h"

namespace Codeine
{

class LoadView : public QWidget, private Ui_LoadView
{
    Q_OBJECT
    public:
      explicit LoadView(QWidget *parent);
    signals:
      void loadUrl(KUrl);
      void openFilePressed();
      void openDVDPressed();
};

}
#endif 
