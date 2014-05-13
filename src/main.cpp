#include <opencv2/highgui/highgui.hpp>

//#include "SkyDetect.h"
#include "SkyDetectorController.h"

#include <QApplication>

int main(int argc, char *argv[]){

	//SkyDetect skyDetect;
	//skyDetect.detect();
	QApplication app(argc, argv);

	SkyDetectorController skyControl( &app);

	return app.exec();


	//cv::waitKey( 0 );
}

