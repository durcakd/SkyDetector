#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>

class LineEdit : public QLineEdit
	{
	Q_OBJECT

public:
	explicit LineEdit(double value, QWidget *parent = 0);

	double getdValue() const;


	};

#endif
