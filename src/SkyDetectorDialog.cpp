#include "SkyDetectordialog.h"
#include <QHeaderView>
#include <QDebug>
#include <QGridLayout>
#include <QFileDialog>


// constructor
SkyDetectorDialog::SkyDetectorDialog(QWidget *parent) :
	QDialog(parent)
{
	_startB		= new QPushButton(tr("Start"));
	_openFileB	= new QPushButton(tr("Open"));

	_fileName = "";
	_spcountLE = new QLineEdit("1000");
	_compactnessLE = new QLineEdit("10.0");
	_sky1SB		= new SpinBox(100);
	_sky2SB		= new SpinBox(80);
	_sky3SB		= new SpinBox(130);
	_sky4SB		= new SpinBox(120);
	_sky5SB		= new SpinBox(255);
	_sky6SB		= new SpinBox(255);

	_maybe1SB	= new SpinBox(85);
	_maybe2SB	= new SpinBox(0);
	_maybe3SB	= new SpinBox(100);
	_maybe4SB	= new SpinBox(165);
	_maybe5SB	= new SpinBox(255);
	_maybe6SB	= new SpinBox(255);

	_white1SB	= new SpinBox(0);
	_white2SB	= new SpinBox(0);
	_white3SB	= new SpinBox(150);
	_white4SB	= new SpinBox(255);
	_white5SB	= new SpinBox(255);
	_white6SB	= new SpinBox(255);

	_sim1aLE	= new LineEdit(8.5);
	_sim1bLE	= new LineEdit(5);
	_sim1cLE	= new LineEdit(15);
	_sim2aLE	= new LineEdit(100);
	_sim2bLE	= new LineEdit(5);
	_sim2cLE	= new LineEdit(50);

	// LAYOUTING
	QGridLayout *paramLayout1 = new QGridLayout;
	QGridLayout *paramLayout2 = new QGridLayout;
	QGridLayout *paramLayout3 = new QGridLayout;

	paramLayout1->addWidget( new QLabel(tr("Sky")),0,0);
	paramLayout1->addWidget( _sky1SB,1,0);
	paramLayout1->addWidget( _sky2SB,1,1);
	paramLayout1->addWidget( _sky3SB,1,2);
	paramLayout1->addWidget( _sky4SB,2,0);
	paramLayout1->addWidget( _sky5SB,2,1);
	paramLayout1->addWidget( _sky6SB,2,2);
	paramLayout1->addWidget( new QLabel(tr("Maybe")),3,0);
	paramLayout1->addWidget( _maybe1SB,4,0);
	paramLayout1->addWidget( _maybe2SB,4,1);
	paramLayout1->addWidget( _maybe3SB,4,2);
	paramLayout1->addWidget( _maybe4SB,5,0);
	paramLayout1->addWidget( _maybe5SB,5,1);
	paramLayout1->addWidget( _maybe6SB,5,2);
	paramLayout1->addWidget( new QLabel(tr("White")),6,0);
	paramLayout1->addWidget( _white1SB,7,0);
	paramLayout1->addWidget( _white2SB,7,1);
	paramLayout1->addWidget( _white3SB,7,2);
	paramLayout1->addWidget( _white4SB,8,0);
	paramLayout1->addWidget( _white5SB,8,1);
	paramLayout1->addWidget( _white6SB,8,2);
	paramLayout2->addWidget( new QLabel(tr("Sim 1 ")),0,0);
	paramLayout2->addWidget( _sim1aLE,1,0);
	paramLayout2->addWidget( _sim1bLE,1,1);
	paramLayout2->addWidget( _sim1cLE,1,2);
	paramLayout2->addWidget( new QLabel(tr("Sim 2 ")),3,0);
	paramLayout2->addWidget( _sim2aLE,4,0);
	paramLayout2->addWidget( _sim2bLE,4,1);
	paramLayout2->addWidget( _sim2cLE,4,2);

	paramLayout3->addWidget( new QLabel(tr("SP count")),0,0);
	paramLayout3->addWidget( _spcountLE,0,1);
	paramLayout3->addWidget( new QLabel(tr("Compact")),1,0);
	paramLayout3->addWidget( _compactnessLE,1,1);
	paramLayout3->addWidget( _openFileB,2,0);
	paramLayout3->addWidget( _startB,2,1);


	QVBoxLayout *rightLayout = new QVBoxLayout;
	rightLayout->addLayout(paramLayout2);
	rightLayout->addStretch();
	rightLayout->addLayout(paramLayout3);

	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addLayout(paramLayout1);
	mainLayout->addLayout(rightLayout);

	setLayout( mainLayout );
	layout()->setSizeConstraint(QLayout::SetFixedSize);
	setWindowTitle( tr("SkyDetector") );




	// contect slots
	connect( _openFileB, SIGNAL(clicked()),
			 this, SLOT(open()));
	connect( _startB, SIGNAL(clicked()),
			 this, SLOT(start()));

}


// PRIVATE SLOTS:
// open
bool SkyDetectorDialog::open(){
	const QString fileFilters = tr("JPG (*.jpg)\n"
								   "NPG (*.npg)\n"
								   "BMP (*.bmp)\n"
								   "All files (*)");

	QString fileName =
			QFileDialog::getOpenFileName(this, tr("Open"), ".", fileFilters);
	if (fileName.isEmpty()){
		return false;
	}
	_fileName = fileName;
	return true;
}


// start thread
void  SkyDetectorDialog::start(){
	prepareParam();

	if( _thread.isRunning()){
		_thread.setAbort();	// Tell the thread to abort

	} else {
		_startB->setText(tr("Stop"));
		_thread.start();
	}
}

void  SkyDetectorDialog::threadDone(){
	_startB->setText(tr("Start"));
}

void SkyDetectorDialog::prepareParam()
{
	_parm.fileName		= _fileName;
	_parm.spcount		= _spcountLE->text().toInt();
	_parm.compactness	= _compactnessLE->text().toDouble();
	_parm.sky1		= _sky1SB->value();
	_parm.sky2		= _sky2SB->value();
	_parm.sky3		= _sky3SB->value();
	_parm.sky4		= _sky4SB->value();
	_parm.sky5		= _sky5SB->value();
	_parm.sky6		= _sky6SB->value();
	_parm.maybe1	= _maybe1SB->value();
	_parm.maybe2	= _maybe2SB->value();
	_parm.maybe3	= _maybe3SB->value();
	_parm.maybe4	= _maybe4SB->value();
	_parm.maybe5	= _maybe5SB->value();
	_parm.maybe6	= _maybe6SB->value();
	_parm.white1	= _white1SB->value();
	_parm.white2	= _white2SB->value();
	_parm.white3	= _white3SB->value();
	_parm.white4	= _white4SB->value();
	_parm.white5	= _white5SB->value();
	_parm.white6	= _white6SB->value();
	_parm.sim1a		= _sim1aLE->getdValue();
	_parm.sim1b		= _sim1bLE->getdValue();
	_parm.sim1c		= _sim1cLE->getdValue();
	_parm.sim2a		= _sim2aLE->getdValue();
	_parm.sim2b		= _sim2bLE->getdValue();
	_parm.sim2c		= _sim2cLE->getdValue();
}
