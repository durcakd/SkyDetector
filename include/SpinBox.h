#ifndef SPINBOX_H
#define SPINBOX_H

#include <QSpinBox>

class SpinBox : public QSpinBox
	{
	Q_OBJECT

public:
	explicit SpinBox(int value, QWidget *parent = 0);

	};

#endif
