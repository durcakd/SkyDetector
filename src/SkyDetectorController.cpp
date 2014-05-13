#include "SkyDetectorController.h"

// constructor
SkyDetectorController::SkyDetectorController(QApplication *app){
	_app = app;
	_skyDetectorDialog		= new SkyDetectorDialog();

	createConnections();   // create connecions

	// initial open file
	//_skyDetectorTableModel->openFile(":images/s04b.txt");

	_skyDetectorDialog->show();
}




// destructor
SkyDetectorController::~SkyDetectorController(){
	delete _skyDetectorDialog;
}




// create connections
void SkyDetectorController::createConnections(){
	//QObject::connect( mSkyDetectorDialog->getThread(), SIGNAL(done(QString)),
	//				  mSkyDetectorDialog, SLOT(threadDone(QString)));


	// progress in statusBar
	//QObject::connect( mSkyDetectorDialog->getThread(), SIGNAL(sentStatusMsg(QString,int)),
	//				  mSkyDetectorDialog->getStatusBar(), SLOT(showMessage(QString,int)) );
	//QObject::connect( mSkyDetectorDialog, SIGNAL(sentStatusMsg(QString,int)),
	//				  mSkyDetectorDialog->getStatusBar(), SLOT(showMessage(QString,int)) );
	//QObject::connect( mSkyDetectorTableModel, SIGNAL(sentStatusMsg(QString,int)),
	//				  mSkyDetectorDialog->getStatusBar(), SLOT(showMessage(QString,int)) );
	//QObject::connect( resultEmiter, SIGNAL(sentStatusMsg(QString,int)),
	//				  mSkyDetectorDialog->getStatusBar(), SLOT(showMessage(QString,int)) );

	// progress
	//QObject::connect( mSkyDetectorDialog->getThread(), SIGNAL(sendProgress(int)),
	//				  mSkyDetectorDialog->getTriesPB(), SLOT(setValue(int)) );

}
