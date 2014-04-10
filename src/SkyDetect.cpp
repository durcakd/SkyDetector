#include "SkyDetect.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QDebug>
#include <set>
#include "SLIC.h"


SkyDetect::SkyDetect()
{
	mSlico			= new SLIC();
	mSpcount		= 500;
	mCompactness	= 1.0;
	mLabels			= NULL;
}

SkyDetect::~SkyDetect(void)
{
	if(mLabels){
		delete [] mLabels;
	}
}

int SkyDetect::detect()
{
	QString saveLocation = "C:\\Users\\Durcak\\Desktop\\SLICO\\";
	QString filename	 = "C:\\Users\\Durcak\\Desktop\\SLICO\\12.jpg";

	openImage( filename );
	applyFiltersBefore();

	doSlico( filename, saveLocation);

	initSPixelsFromLabels( mLabels);
	createPattern();
	initSPixelAdj16();

	mergeSP();

	cv::waitKey(0);

	return 0;
}

int SkyDetect::doSlico( const QString filename, const QString saveLocation )
{
	string stdSaveLoc   = saveLocation.toStdString();
	string stdFilename  = filename.toStdString();

	unsigned int *imgBuf = NULL;
	createPicBuffer( imgBuf);

	if( mSpcount < 200){
		mSpcount = 200;
	}
	if( mSpcount > mSize) {
		qDebug() << "Number of superpixels exceeds number of pixels in the image";
	}

	mLabels = new int[mSize];
	int numlabels = 0;

	mSlico->PerformSLICO_ForGivenK( imgBuf, mWidth, mHeight,
									mLabels, numlabels,
									mSpcount, mCompactness ); //for a given number K of superpixels
	qDebug() << " - PerformSLICO_ForGivenK:	Done";

	mSlico->DrawContoursAroundSegmentsTwoColors( imgBuf, mLabels,
												 mWidth, mHeight );//for black-and-white contours around superpixels
	qDebug() << " - DrawContoursAroundSegmentsTwoColors: Done";

	mSlicoRes = cv::Mat( mHeight, mWidth, CV_8UC4, imgBuf );

	cv::imshow("SLICO result image", mSlicoRes );
	cv::waitKey(1);

	return 0;
}


void SkyDetect::openImage( const QString filename)
{
	qDebug() << "INPUT: " << filename;
	mImageIn	= cv::imread( filename.toStdString(), CV_LOAD_IMAGE_COLOR);
	mHeight		= mImageIn.rows;
	mWidth		= mImageIn.cols;
	mSize		= mHeight * mWidth;

	qDebug() << "type:  " << mImageIn.type() << "  " << mWidth << "x" << mHeight;

}

void SkyDetect::applyFiltersBefore()
{
	//mImageIn
	//cv::medianBlur(mImageIn, mImageIn, 3);
}

void SkyDetect::createPicBuffer( unsigned int*& imgBuffer )
{
	// create ARGB 4x8bits picture
	cv::Mat alpha(cv::Size( mWidth, mHeight), CV_8UC1);
	cv::Mat outMat(cv::Size( mWidth, mHeight),CV_8UC4);
	cv::Mat in[] = {  alpha, mImageIn };
	int from_to[] = { 0,3,  1,0,  2,1,  3,2 };
	cv::mixChannels( in, 2, &outMat, 1, from_to, 4 );

	//qDebug() << "type2: " << outMat.type();
	//cv::imshow( "Input Image2", outMat );


	imgBuffer = new UINT[mSize];
	memcpy((void*) imgBuffer, outMat.data, mSize*sizeof(UINT) );
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

	SPV::const_iterator ist;
	for( ist = mSPV.begin(); ist != mSPV.end(); ist++){
		(*ist)->computeBoundary();
	}

	// check
	//mSPV[0]->getPixelV();
}

void SkyDetect::createPattern( const int* labels)
{

}

void SkyDetect::createPattern()
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
			pattern16.at<unsigned short>(ipt->r, ipt->c) = name;
		}
	}

	mPattern16 = pattern16;
	//pattern16.convertTo( pattern8, CV_8UC1);
	//cv::imshow("Pattern8", pattern8);
	//cv::waitKey(1);
}

void SkyDetect::initSPixelAdj16()
{
	int r, c;

	cv::Mat strElmnt = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3,3));


	// for all superpixels
	SPV::const_iterator its;
	for( its = mSPV.begin(); its != mSPV.end(); its++){

		//qDebug() << " "  << (*its)->getName() << "   -   " << (*its)->getPixelVSize() << "   " << (*its)->getAdjVSize();

		// create mask
		cv::Mat mask8   = cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC1);
		cv::Mat dmask8	= cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC1);

		PIXV::const_iterator itp;
		PIXV pixels = (*its)->getPixelV();

		for( itp = pixels.begin(); itp != pixels.end(); itp++){
			mask8.at<uchar>(itp->r,itp->c)= 100;
		}

		// compute mean
		(*its)->setMean( cv::mean( mSlicoRes, mask8 ));


		cv::Scalar mean = (*its)->getMean();
		qDebug() << "mean: " << mean.val[0] << " " << mean.val[1] << " " << mean.val[2];
		//cv::Mat test  = cv::Mat( mWidth, mHeight, CV_8UC3, mean );
		//cv::imshow("mean", test );



		// dilate mast
		cv::dilate(mask8, dmask8, strElmnt);
		cv::bitwise_xor( mask8, dmask8, mask8 );

		// mask pattern
		cv::Mat masked8;
		cv::Mat masked16;
		mPattern16.copyTo(masked16, mask8);

		//masked16.convertTo(masked8, CV_8UC1);
		//cv::imshow("masked", masked8 );



		// get adjacency
		set<int> adjSet;
		set<int>::const_iterator itAdj;
		int pix;
		for( r = 0; r < mHeight; r++){
			for( c = 0; c < mWidth; c++){

				pix = masked16.at<unsigned short>(r,c);
				if( pix != 0  &&  adjSet.find(pix) == adjSet.end() ){
					adjSet.insert( pix);
				}
			}
		}
		// will be sorted
		for( itAdj = adjSet.begin(); itAdj != adjSet.end(); itAdj++){
			(*its)->addAdj(pix);
			//qDebug() << *itAdj;

		}
		adjSet.clear();


		//cv::waitKey(0);
	}

	//cv::cvtColor( mSlicoRes, mSlicoRes, CV_HSV2RGB );
}

void SkyDetect::mergeSP()
{

	cv::Mat meanMat(cv::Size( mWidth, mHeight), CV_8UC3);
	cv::Mat meanMatHsv(cv::Size( mWidth, mHeight), CV_8UC3);
	cv::Mat meanMask = cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC1);
	cv::Mat meanMatRes(cv::Size( mWidth, mHeight), CV_8UC3);
	int r, c;

	SPV::const_iterator its;
	for( its = mSPV.begin(); its != mSPV.end(); its++){
		cv::Scalar mean = (*its)->getMean();
		//qDebug() << "mean: " << mean.val[0] << " " << mean.val[1] << " " << mean.val[2];
		//cv::Mat test  = cv::Mat( mWidth, mHeight, CV_8UC3, mean );
		//cv::imshow("mean", test );

		PIXV pixels = (*its)->getPixelV();

		// any name cant be 0, so we identify superpixels accoding theirs names
		PIXV::const_iterator ipt;
		for( ipt = pixels.begin(); ipt != pixels.end(); ipt++){
			//meanMat.at<unsigned short>(ipt->r, ipt->c) = name;
			r = ipt->r;
			c = ipt->c;
			meanMat.data[meanMat.step[0]*r + meanMat.step[1]* c + 0] = mean.val[0];
			meanMat.data[meanMat.step[0]*r + meanMat.step[1]* c + 1] = mean.val[1];
			meanMat.data[meanMat.step[0]*r + meanMat.step[1]* c + 2] = mean.val[2];

		}
	}

	cv::imshow("meanMat", meanMat);

	cv::cvtColor( meanMat, meanMatHsv, CV_BGR2HSV );
	cv::inRange( meanMatHsv, cv::Scalar(90, 0, 110), cv::Scalar(130, 255, 255), meanMask);
	meanMat.copyTo(meanMatRes, meanMask);

	cv::imshow("meanMasked", meanMatRes);

}
