#include "SkyDetect.h"
#include <iostream>

#include <opencv2\imgproc\imgproc.hpp>

#include <QDebug>


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
		UINT *imgBuf	= NULL;
		int	width	= 0;
		int height	= 0;

		createPicBuffer( QString::fromStdString(picvec[k]), imgBuf, width, height );

		int sz = width*height;
		if( mSpcount > sz) { cout << "Number of superpixels exceeds number of pixels in the image" << endl; }

		int* labels = new int[sz];
		int numlabels(0);
		//return 0;
		qDebug() << "krok 1 " << endl;
		mSlic.PerformSLICO_ForGivenK(imgBuf, width, height, labels, numlabels, mSpcount, mCompactness);//for a given number K of superpixels
		qDebug() << "krok 2 " << endl;
		mSlic.DrawContoursAroundSegmentsTwoColors(imgBuf, labels, width, height);//for black-and-white contours around superpixels
		qDebug() << "krok 3 " << endl;
		mSlic.SaveSuperpixelLabels(labels,width,height,picvec[k],saveLocation);
		qDebug() << "krok 4 " << endl;
		if(labels) delete [] labels;

		cv::Mat  resMat( height, width, CV_8UC4, imgBuf );
		cout << "res";
		cv::imshow("res", resMat );
		cv::waitKey(0);

	}
	return 0;
}

int SkyDetect::getPictures( vector<string>& picvec ){

	string name = "D:\\Fotky+Obrazky\\labut.jpg";
	picvec.push_back(name);
	return 0;
}

void SkyDetect::createPicBuffer(
	const QString	filename,
	UINT*&			imgBuffer,
	int&			width,
	int&			height) const
{
	cv::Mat inMat;
	inMat = cv::imread( filename.toStdString(), CV_LOAD_IMAGE_COLOR);

	height					= inMat.rows;
	width					= inMat.cols;
	long imgSize			= height * width;

	qDebug() << "INPUT: " << filename;
	qDebug() << "type:  " << inMat.type() << "  " << width << "x" << height;
	//cv::imshow( "Input Image", inMat );



	// create ARGB 4x8bits picture
	cv::Mat alpha(cv::Size(width, height), CV_8UC1);
	cv::Mat outMat(cv::Size(width, height),CV_8UC4);
	cv::Mat in[] = {  alpha, inMat };
	int from_to[] = { 0,3,  1,0,  2,1,  3,2 };
	cv::mixChannels( in, 2, &outMat, 1, from_to, 4 );

	qDebug() << "type2: " << outMat.type();
	//cv::imshow( "Input Image2", outMat );
	//cv::waitKey(1);


	imgBuffer = new UINT[imgSize];
	memcpy((void*) imgBuffer, outMat.data, imgSize*sizeof(UINT) );
}
