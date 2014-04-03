#include "SkyDetect.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QDebug>
#include <set>
#include "SLIC.h"


SkyDetect::SkyDetect()
{
	mSlico = new SLIC();
	mSpcount = 300





			;
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

	QString saveLocation = "C:\\Users\\Durcak\\Desktop\\SLICO\\";
	QString filename	 = "C:\\Users\\Durcak\\Desktop\\SLICO\\09.jpg";
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

	createPicBuffer( filename, imgBuf);

	width			= mWidth;
	height			= mHeight;

	int size = mWidth * mHeight ;

	if( mSpcount < 200){
		mSpcount = 200;
	}
	if( mSpcount > size) {
		qDebug() << "Number of superpixels exceeds number of pixels in the image";
	}

	int* labels = new int[size];
	int numlabels = 0;



	mSlico->PerformSLICO_ForGivenK(
				imgBuf, mWidth, mHeight, labels, numlabels, mSpcount, mCompactness ); //for a given number K of superpixels
	qDebug() << " - PerformSLICO_ForGivenK:	Done";

	mSlico->DrawContoursAroundSegmentsTwoColors(
				imgBuf, labels, mWidth, mHeight );//for black-and-white contours around superpixels
	qDebug() << " - DrawContoursAroundSegmentsTwoColors: Done";

	mSlico->SaveSuperpixelLabels(
				labels, mWidth, mHeight, stdFilename, stdSaveLoc);
	qDebug() << " - SaveSuperpixelLabels: Done";



	mSlicoRes = cv::Mat( mHeight, mWidth, CV_8UC4, imgBuf );
	cv::imshow("SLICO result image", mSlicoRes );
	cv::waitKey(0);

	//cv::Mat labMat = createSPLabelsMat( labels, width, height);

	initSPixelsFromLabels( labels);
	//
	createPattern();
	initSPixelAdj16();

	cv::waitKey(0);

	if(labels){
		delete [] labels;
	}

	return 0;
}


void SkyDetect::createPicBuffer( const QString	filename, unsigned int*& imgBuffer )
{
	cv::Mat inMat;
	inMat = cv::imread( filename.toStdString(), CV_LOAD_IMAGE_COLOR);

	mHeight					= inMat.rows;
	mWidth					= inMat.cols;
	long imgSize			= mHeight * mWidth;

	qDebug() << "INPUT: " << filename;
	qDebug() << "type:  " << inMat.type() << "  " << mWidth << "x" << mHeight;

	// create ARGB 4x8bits picture
	cv::Mat alpha(cv::Size( mWidth, mHeight), CV_8UC1);
	cv::Mat outMat(cv::Size( mWidth, mHeight),CV_8UC4);
	cv::Mat in[] = {  alpha, inMat };
	int from_to[] = { 0,3,  1,0,  2,1,  3,2 };
	cv::mixChannels( in, 2, &outMat, 1, from_to, 4 );

	qDebug() << "type2: " << outMat.type();
	//cv::imshow( "Input Image2", outMat );


	imgBuffer = new UINT[imgSize];
	memcpy((void*) imgBuffer, outMat.data, imgSize*sizeof(UINT) );
}


void SkyDetect::initSPixelsFromLabels(const int* labels)
{
	int idx;
	int r, c, i;
	int count = -1;
	SPixel *spixel;

	// count number of superpixels
	idx = 0;
	for( r = 0; r < mHeight; r++){
		for( c = 0; c < mWidth; c++){
			if( count < labels[idx]){
				count = labels[idx];
			}
			idx++;
		}
	}

	// create superpixels object
	for( i = 0; i <= count; i++){
		spixel = new SPixel;
		// any name cant be 0
		spixel->setName( i+1 );
		mSPV.push_back( spixel );
	}

	// init pixels of superpixels
	idx = 0;
	for( r = 0; r < mHeight; r++){
		for( c = 0; c < mWidth; c++){

			mSPV[labels[idx++]]->addPixel(r,c);

		}
	}

	// check
	//mSPV[0]->getPixelV();


}



void  SkyDetect::createPattern( const int* labels)
{

}

void  SkyDetect::createPattern()
{
	cv::Mat pattern16(cv::Size( mWidth, mHeight), CV_16UC1);
	cv::Mat pattern8(cv::Size( mWidth, mHeight), CV_8UC1);
	int		name;


	qDebug() << "mSPV.size() = " << mSPV.size();

	SPV::const_iterator ist;
	for( ist = mSPV.begin(); ist != mSPV.end(); ist++){
		name = (*ist)->getName();

		PIXV pixels = (*ist)->getPixelV();

		// any name cant be 0, so we identify superpixels accoding theirs names
		PIXV::const_iterator ipt;
		for( ipt = pixels.begin(); ipt != pixels.end(); ipt++){
			pattern16.at<unsigned short>(ipt->x, ipt->y) = name;
		}
	}

	mPattern16 = pattern16;
	pattern16.convertTo( pattern8, CV_8UC1);
	cv::imshow("Pattern8", pattern8);
}


void  SkyDetect::initSPixelAdj16()
{
	int r, c, i, len, name;




	// create mask
	//for( i = 0; i < len; i++){ // !!
	for( i = 0; i < 5; i++){ // !!
		qDebug() << " "  << mSPV[i]->getName() << "   -   " << mSPV[i]->getPixelVSize() << "   " << mSPV[i]->getAdjVSize();
		//cv::Mat mask8(cv::Size(width, height), CV_8UC1);
		cv::Mat mask8   = cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC1);
		cv::Mat dmask8	= cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC1);


		PIXV::const_iterator it;
		PIXV pixels = mSPV[i]->getPixelV();

		for( it = pixels.begin(); it != pixels.end(); it++){
			mask8.at<uchar>(it->x,it->y)= 100;
		}

		cv::Mat strElmnt = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3,3));
		cv::dilate(mask8, dmask8, strElmnt);
		cv::bitwise_xor( mask8, dmask8, mask8 );

		cv::Mat masked8;
		cv::Mat masked16;
		mPattern16.copyTo(masked16, mask8);
		masked16.convertTo(masked8, CV_8UC1);
		//
		//mSlicoRes.copyTo(masked8, mask8);
		cv::imshow("masked", masked8 );


		//cv::imshow("mask8", mask8);


		// get adjency
		set<int> adjSet;
		set<int>::const_iterator itAdj;
		int pix;
		for( r = 0; r < mHeight; r++){
			for( c = 0; c < mWidth; c++){
				pix = masked16.at<unsigned short>(r,c);
				if( pix != 0  &&  adjSet.find(pix) == adjSet.end() ){
					adjSet.insert( pix);
					mSPV[i]->addAdj(pix);

				}
			}
		}
		for( itAdj = adjSet.begin();
			 itAdj != adjSet.end(); itAdj++){
			qDebug() << *itAdj;
		}

		cv::waitKey(0);
	}
}


