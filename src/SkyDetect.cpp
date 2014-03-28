#include "SkyDetect.h"

#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include <QDebug>

#include "SLIC.h"

SkyDetect::SkyDetect()
{
	mSlico = new SLIC();
	mSpcount = 255;
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

	QString saveLocation = "C:\\Users\\Durcak\\Desktop\\SLICO";
	QString filename	 = "C:\\Users\\Durcak\\Desktop\\SLICO\\flower.jpg";
	doSlico( filename, saveLocation);

	return 0;
}

int SkyDetect::doSlico( const QString filename, const QString saveLocation )
{

	string stdSaveLoc   = saveLocation.toStdString();
	string stdFilename  = filename.toStdString();

	unsigned int *imgBuf		= NULL;
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



	mSlico->PerformSLICO_ForGivenK(
				imgBuf, width, height, labels, numlabels, mSpcount, mCompactness ); //for a given number K of superpixels
	qDebug() << " - PerformSLICO_ForGivenK:	Done";

	mSlico->DrawContoursAroundSegmentsTwoColors(
				imgBuf, labels, width, height );//for black-and-white contours around superpixels
	qDebug() << " - DrawContoursAroundSegmentsTwoColors: Done";

	mSlico->SaveSuperpixelLabels(
				labels, width, height, stdFilename, stdSaveLoc);
	qDebug() << " - SaveSuperpixelLabels: Done";



	mSlicoRes = cv::Mat( height, width, CV_8UC4, imgBuf );
	cv::imshow("SLICO result image", mSlicoRes );
	cv::waitKey(0);
	cv::Mat labMat = createSPLabelsMat( labels, width, height);
	cv::imshow("labMat", labMat );
	cv::waitKey(0);

	if(labels){
		delete [] labels;
	}

	return 0;
}


void SkyDetect::createPicBuffer(
		const QString	filename,
		unsigned int*&			imgBuffer,
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

cv::Mat SkyDetect::createSPLabelsMat( const int*	labels, const int	width, const int	height)
{
	cv::Mat matLab2(cv::Size(width, height), CV_16UC1);
	cv::Mat matLab8(cv::Size(width, height), CV_8UC1);

	int idx = 0;

	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			matLab2.at<unsigned short>(i,j)= (unsigned short) labels[idx++];
		}
	}

	/*for( int i = 0; i < height; i++){
		unsigned short* rowi = matLab2.ptr<unsigned short>(i);

		for( int j = 0; j < width; j++){
			rowi[j] = (unsigned short) labels[idx++];
		}
	}*/
	matLab2.convertTo(matLab8, CV_8UC1);


	return matLab8;


}

