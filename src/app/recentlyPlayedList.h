#ifndef RECENTLYPLAYEDLIST_H
#define RECENTLYPLAYEDLIST_H
#include <KListWidget>
#include <KConfigGroup>


class RecentlyPlayedList : public KListWidget
{
  Q_OBJECT
  public:
	explicit RecentlyPlayedList(QWidget*);
  private:
	virtual void contextMenuEvent(QContextMenuEvent*);
	virtual void loadEntries();
	KConfigGroup* configGroup;
  public slots:
	virtual void removeEntry();
	virtual void clearList();
};


#endif
