#ifndef KU_DATETOOL_H
#define KU_DATETOOL_H

#include "includes.h"

class KDateCtl: public QObject
{
Q_OBJECT
public:
	int x;
	int y;

	int iday;
	int imonth;
	int iyear;

	QLineEdit *day;
	QComboBox *month;
	QLineEdit *year;
	QCheckBox *isempty;
        QLabel *label;


	KDateCtl(QWidget *parent, const char *name, const char *checkTitle,
		 const char *title, long int adate, int ax, int ay);
	void setDate(long int adate);
	long int getDate();	
	void setFont(const QFont &f);
private:
	void updateControls();
	int validate(const char *text);
protected slots:
	void isEmptyToggled(bool state);
	void dayChanged(const char *text);
	void monthChanged(int data);
	void yearChanged(const char *text);
};

#endif // K_DATETOOL_H

