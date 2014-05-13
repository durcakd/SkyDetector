#include "SkyDetectorController.h"

// constructor
SkyDetectorController::SkyDetectorController(QApplication *app){
	_app = app;
	_skyDetectorDialog		= new SkyDetectorDialog();

	createConnections();

	_skyDetectorDialog->show();
}


// destructor
SkyDetectorController::~SkyDetectorController(){
	delete _skyDetectorDialog;
}

// create connections
void SkyDetectorController::createConnections(){
	QObject::connect( _skyDetectorDialog->getThread(), SIGNAL(finished()),
					  _skyDetectorDialog, SLOT(threadDone()) );
}
