#include "SkyDetectordialog.h"
#include <QHeaderView>
#include <QDebug>
#include <QGridLayout>
#include <QFileDialog>


// constructor
SkyDetectorDialog::SkyDetectorDialog(QWidget *parent) :
	QDialog(parent)
{

//	_statusBar	= new QStatusBar;
//	_statusBar->setSizeGripEnabled(false);

	_startB		= new QPushButton(tr("Start"));
	_openFileB	= new QPushButton(tr("Open"));

	//triesPB = new QProgressBar;
	//triesPB->setRange( 0, NUMTESTS );
	//triesPB->setSizePolicy( QSizePolicy());


	// LAYOUTING

	//QGridLayout *paramLayout = new QGridLayout;
	//paramLayout->addWidget( popSizeL,0,0 );

	//parametersGroup = new QGroupBox("MEA parameters" );
	//parametersGroup->setLayout(paramLayout);


	// buttons layouting
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget( _openFileB );
	buttonLayout->addWidget( _startB );



	//setLayout( mainGridLayout );
	//layout()->setSizeConstraint(QLayout::SetFixedSize);
	setWindowTitle( tr("SkyDetector") );
	//
	//parametersGroup->hide();


	// contect slots
	connect( _openFileB, SIGNAL(clicked()),
			 this, SLOT(open()));
	connect( _startB, SIGNAL(clicked()),
			 this, SLOT(start()));

}


// PRIVATE SLOTS:
// open
bool SkyDetectorDialog::open(){
	const QString fileFilters = tr("Text files (*.txt)\n"
								   "SkyDetector files (*.sud)\n"
								   "All files (*)");

	QString fileName =
			QFileDialog::getOpenFileName(this, tr("Open"), ".", fileFilters);
	if (fileName.isEmpty()){
		//emit sentStatusMsg( tr("File name is wrong"), 5000 );
		return false;
	}

	//emit requestForReadFile(fileName);
	return true;
}


// start thread
void  SkyDetectorDialog::start(){

	if( thread.isRunning()){
		thread.setAbort();	// Tell the thread to abort

	} else {
		_startB->setText(tr("Stop"));
		thread.start();
	}
}


/*
// get result from thread
void  SkyDetectorDialog::threadDone(const QString msg){
	addStrToListWidged(msg);

	newTB->setEnabled( true );
	confirmTB->setEnabled( true );
	openFileTB->setEnabled( true );
	saveToFileTB->setEnabled( true );
	autoCB->setEnabled(true);

	startB->setText(tr("Start"));


}

*/
