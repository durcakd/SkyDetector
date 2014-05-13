#include "LineEdit.h"
#include "QRegExpValidator"

// constructor
LineEdit::LineEdit(double value, QWidget *parent) :
	QLineEdit(parent)
{
	QRegExp regExp("[0-9]{1,3}(\\.[0-9])?");
	setValidator(new QRegExpValidator(regExp, this));
	setText( QString::number( value ));
	setFixedSize(50,20);
}


double LineEdit::getdValue() const
{
	return text().toDouble();
}


