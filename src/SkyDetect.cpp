#include "SkyDetect.h"
#include <iostream>

#include <opencv2\imgproc\imgproc.hpp>


SkyDetect::SkyDetect()
{
}

SkyDetect::~SkyDetect(void)
{
}

cv::Mat SkyDetect::getResult()
{
	return mImageRes;
}

int SkyDetect::detect()
{

	mImageRes = mImageIn;
	doSLICO();

	return 0;
}

int SkyDetect::doSLICO()
{
	
	vector<string> picvec(0);
	picvec.resize(0);
	getPictures(picvec);//user chooses one or more pictures
	string saveLocation = "C:\\Users\\Durcak\\Desktop\\SLIC";
	int numPics( picvec.size() );
	mSpcount = 200;
	mCompactness = 10.0;
	if( mSpcount < 100) mSpcount = 200;



	for( int k = 0; k < numPics; k++ )
	{
		UINT* img = NULL;
		int width(0);
		int height(0);

		getPictureBuffer( picvec[k], img, width, height );

		int sz = width*height;
		if( mSpcount > sz) { cout << "Number of superpixels exceeds number of pixels in the image" << endl; }

		int* labels = new int[sz];
		int numlabels(0);
		//return 0;
		cout << "krok 1 " << endl;
		mSlic.PerformSLICO_ForGivenK(img, width, height, labels, numlabels, mSpcount, mCompactness);//for a given number K of superpixels
		cout << "krok 2 " << endl;
		mSlic.DrawContoursAroundSegmentsTwoColors(img, labels, width, height);//for black-and-white contours around superpixels
		cout << "krok 3 " << endl;
		mSlic.SaveSuperpixelLabels(labels,width,height,picvec[k],saveLocation);
		cout << "krok 4 " << endl;
		if(labels) delete [] labels;
		
		cv::Mat  res( height, width, CV_8UC4, img );
		cout << "res";
		cv::imshow("res", res);
		cv::waitKey(0);

	}
	cout << "Done";
	return 0;
}

int SkyDetect::getPictures( vector<string>& picvec ){

	string name = "D:\\Fotky+Obrazky\\labut.jpg";
	picvec.push_back(name);
	return 0;
}

void SkyDetect::getPictureBuffer(
	string&			filename,
	UINT*&				imgBuffer,
	int&				width,
	int&				height)
{
	cv::Mat matImg;
	matImg = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
	
	height					= matImg.rows;
	width					= matImg.cols;
	long imgSize			= height*width;
	cout << "height:  " << height << endl;
	cout << "width:  " << width << endl;
	cout << "type:   " << matImg.type() << endl;
	cout << "depth:  " << matImg.depth() << endl;
	cv::imshow( "img1", matImg );
	//cv::waitKey(0);
	





	cv::Mat alpha(cv::Size(width, height), CV_8UC1);
    cv::Mat src(cv::Size(width, height),CV_8UC4);
	cv::Mat in[] = {  alpha, matImg };
    int from_to[] = { 0,3,  1,0,  2,1,  3,2 };
    cv::mixChannels( in, 2, &src, 1, from_to, 4 );

	cout << "type2:" << src.type() << endl;
	cout << "depth2:" << src.depth() << endl;
	
	cv::imshow( "img2", src );
	//cv::waitKey(0);
	
	imgBuffer = new UINT[imgSize];
	memcpy((void*) imgBuffer, src.data, imgSize*sizeof(UINT) );
	cv::waitKey(0);
	
}