#include "SpinBox.h"

// constructor
SpinBox::SpinBox(int value, QWidget *parent) :
	QSpinBox(parent)
{
	setRange( 0, 255 );
	setValue( value );
	setSingleStep( 1 );
}




