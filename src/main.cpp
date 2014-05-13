#include <QApplication>
#include "SkyDetectorController.h"


int main(int argc, char *argv[]){

	QApplication app(argc, argv);

	SkyDetectorController skyControl( &app);

	return app.exec();
}

