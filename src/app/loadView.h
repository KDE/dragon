#ifndef LOADVIEW_H
#define LOADVIEW_H
#include <QWidget>

#include <KUrl>
#include "ui_loadView.h"

namespace Dragon
{

class LoadView : public QWidget, private Ui_LoadView
{
    Q_OBJECT
    public:
      explicit LoadView(QWidget *parent);
      void setThumbnail(QWidget *object);
    signals:
      void loadUrl(KUrl);
      void openFilePressed();
      void openDVDPressed();
};

}
#endif 
