#include "SkyDetect.h"

#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include <QDebug>
#include <set>
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

	QString saveLocation = "C:\\Users\\Durcak\\Desktop\\SLICO\\";
	QString filename	 = "C:\\Users\\Durcak\\Desktop\\SLICO\\small.jpg";
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

	cv::Mat masked;
	//cv::Mat labMat = createSPLabelsMat( labels, width, height);

	initSPixelsFromLabels( labels, width, height);
	initSPixelAdj( width, height );

	//mSlicoRes.copyTo(masked, labMat);
	//cv::imshow("labMat", masked );
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

cv::Mat SkyDetect::createSPLabelsMat(const int*	labels, int width, int height)
{
	//cv::Mat matLab2(cv::Size(width, height), CV_16UC1);
	cv::Mat matLab2(cv::Size(width, height), CV_32SC1);
	cv::Mat matLab8(cv::Size(width, height), CV_8UC1);

	int idx = 0;

	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			matLab2.at<int>(i,j)= labels[idx++];
		}
	}

	/*for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			matLab2.at<unsigned short>(i,j)= (unsigned short) labels[idx++];
		}
	}*/
	/*for( int i = 0; i < height; i++){
		unsigned short* rowi = matLab2.ptr<unsigned short>(i);

		for( int j = 0; j < width; j++){
			rowi[j] = (unsigned short) labels[idx++];
		}
	}*/







	matLab2.convertTo(matLab8, CV_8UC1);

	cv::imshow("labdd", matLab8 );


	cv::Mat tresh1;
	cv::Mat tresh2;
	cv::Mat tresh3;

	cv::threshold(matLab8, tresh1, 80, 82, cv::THRESH_BINARY );
	//cv::threshold(matLab8, tresh2, 82, 255, cv::THRESH_BINARY );

	cv::imshow("tresh", tresh1 );

/*
	cv::bitwise_xor( tresh1, tresh2, tresh1 );

	cv::Mat strElmnt = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(11,11));
	cv::dilate(tresh1, tresh3, strElmnt);
	cv::bitwise_xor( tresh1, tresh3, tresh1 );

	cv::Mat masked, masked8;
	matLab2.copyTo(masked, tresh1);
	masked.convertTo(masked8, CV_8UC1);
	cv::imshow("labdd", masked );
*/
	//tresh1.convertTo(tresh1, CV_8UC1);

	return matLab8;


}

void SkyDetect::initSPixelsFromLabels( const int* labels, int width, int height)
{
	int idx;
	int r, c, i;
	int count = -1;
	SPixel *spixel;

	// count number of superpixels
	idx = 0;
	for( r = 0; r < height; r++){
		for( c = 0; c < width; c++){
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
	for( r = 0; r < height; r++){
		for( c = 0; c < width; c++){

			mSPV[labels[idx++]]->addPixel(r,c);

		}
	}

	// check
	//mSPV[0]->getPixelV();


}

void  SkyDetect::initSPixelAdj( int width, int height)
{
	int r, c, i, len, name;
	len = mSPV.size();
	qDebug() << "mSPV.size() = " << len;


	cv::Mat patern8(cv::Size(width, height), CV_8UC1);


	for( i = 0; i < len; i++){ // !!
		name = mSPV[i]->getName();
		PIXV::const_iterator it;
		PIXV pixels = mSPV[i]->getPixelV();

		/*for( it = pixels.begin(); it != pixels.end(); it++){
			matLab2.at<unsigned short>(it->x,it->y)= 120;
		}*/

		// any name cant be 0, so we identify superpixels accoding theirs names
		for( it = pixels.begin(); it != pixels.end(); it++){
			patern8.at<uchar>(it->x,it->y)= name;
		}
	}
	cv::imshow("patern", patern8);



	// create mask
	//for( i = 0; i < len; i++){ // !!
	for( i = 0; i < 50; i++){ // !!
		qDebug() << " "  << mSPV[i]->getName() << "   -   " << mSPV[i]->getPixelVSize() << "   " << mSPV[i]->getAdjVSize();
		//cv::Mat mask8(cv::Size(width, height), CV_8UC1);
		cv::Mat mask8   = cv::Mat::zeros(cv::Size(width, height), CV_8UC1);
		cv::Mat dmask8	= cv::Mat::zeros(cv::Size(width, height), CV_8UC1);


		PIXV::const_iterator it;
		PIXV pixels = mSPV[i]->getPixelV();

		for( it = pixels.begin(); it != pixels.end(); it++){
			mask8.at<uchar>(it->x,it->y)= 100;
		}

		cv::Mat strElmnt = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3,3));
		cv::dilate(mask8, dmask8, strElmnt);
		cv::bitwise_xor( mask8, dmask8, mask8 );

		cv::Mat masked8;
		patern8.copyTo(masked8, mask8);
		//
		//mSlicoRes.copyTo(masked8, mask8);
		cv::imshow("masked", masked8 );


		//cv::imshow("mask8", mask8);


		// get adjency
		set<int> adjSet;
		set<int>::const_iterator itAdj;
		int pix;
		for( r = 0; r < height; r++){
			for( c = 0; c < width; c++){
				pix = masked8.at<uchar>(r,c);
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
