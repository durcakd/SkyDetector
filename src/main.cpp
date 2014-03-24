
#include <string>

using namespace std;
//using namespace cv;

#include "SkyDetect.h"


int main(void){
	string filepath = "D:\\Fotky+Obrazky\\labut.jpg";
	//Mat imgIn, imgOut;
	//imgIn = imread(filepath, CV_LOAD_IMAGE_COLOR);
	//imgOut = imgIn;

	SkyDetect skyDetect;
	skyDetect.detect();
	//imgOut = skyDetect.getResult();


	//imshow("imageIn", imgIn);
//	imshow("imageOut", imgOut);
	cv::waitKey( 0 );
	return 0;
}

