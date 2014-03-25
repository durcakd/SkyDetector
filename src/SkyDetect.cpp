#include "SkyDetect.h"
#include <iostream>

#include <opencv2\imgproc\imgproc.hpp>

#include <QDebug>


SkyDetect::SkyDetect()
{
	mSpcount = 300;
	mCompactness = 10.0;


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

	QString saveLocation = "C:\\Users\\Durcak\\Desktop\\SLIC";
	QString filename	 = "D:\\Fotky+Obrazky\\labut.jpg";
	doSLICO( filename, saveLocation);

	return 0;
}

int SkyDetect::doSLICO( const QString filename, const QString saveLocation )
{

	string stdSaveLoc   = saveLocation.toStdString();
	string stdFilename  = filename.toStdString();

	UINT *imgBuf		= NULL;
	int	width			= 0;
	int height			= 0;

	createPicBuffer( filename, imgBuf, width, height );

	int size = width * height;

	if( mSpcount < 200){
		mSpcount = 200;
	}
	if( mSpcount > size) {
		qDebug() << "Number of superpixels exceeds number of pixels in the image";
	}

	int* labels = new int[size];
	int numlabels = 0;



	mSlic.PerformSLICO_ForGivenK( imgBuf, width, height, labels, numlabels, mSpcount, mCompactness ); //for a given number K of superpixels
	qDebug() << " - PerformSLICO_ForGivenK:	Done";

	mSlic.DrawContoursAroundSegmentsTwoColors( imgBuf, labels, width, height );//for black-and-white contours around superpixels
	qDebug() << " - DrawContoursAroundSegmentsTwoColors: Done";

	mSlic.SaveSuperpixelLabels( labels, width, height, stdFilename, stdSaveLoc);
	qDebug() << " - SaveSuperpixelLabels: Done";

	if(labels){
		delete [] labels;
	}


	cv::Mat  resMat( height, width, CV_8UC4, imgBuf );
	cv::imshow("SLICO result image", resMat );
	cv::waitKey(0);

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
