#ifndef Q_mainfilter_h
#define Q_mainfilter_h

#include "ui_mainfilter.h"

class filtermainWindow : public QDialog
{
	Q_OBJECT

public:
	filtermainWindow();
	void buildActiveFilterList(void);

	Ui_mainFilterDialog ui;
	QListWidget *availableList;
	QListWidget *activeList;

public slots:
	void VCD(bool b);
	void DVD(bool b);
	void SVCD(bool b);
	void halfD1(bool b);
	void add(bool b);
	void up(bool b);
	void down(bool b);
	void remove(bool b);
	void configure(bool b);
	void partial(bool b);
	void activeDoubleClick( QListWidgetItem  *item);
	void allDoubleClick( QListWidgetItem  *item);
	void filterFamilyClick(QListWidgetItem *item);
	void filterFamilyClick(int  item);

private:
	void setSelected(int sel);
	void displayFamily(uint32_t family);
	void setupFilters(void);
};
#endif	// Q_mainfilter_h
