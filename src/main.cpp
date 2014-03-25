#include <opencv2/highgui/highgui.hpp>

#include "SkyDetect.h"

int main(void){

	SkyDetect skyDetect;
	skyDetect.detect();


	cv::waitKey( 0 );
	return 0;
}

