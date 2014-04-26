#include "SkyDetect.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QDebug>
#include <set>
#include <queue>

#include "SLIC.h"


SkyDetect::SkyDetect()
{
	mSlico			= new SLIC();
	mSpcount		= 1000;
	mCompactness	= 10.0;
	mLabels			= NULL;

	maxd		= 30.0;
	mSKYCounter = 0;
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
	QString filename	 = "C:\\Users\\Durcak\\Desktop\\SLICO\\05.jpg";

	openImage( filename );
	applyFiltersBefore();

	doSlico( filename, saveLocation);

	initSPixelsFromLabels( mLabels);
	createPattern();
	initSPixelAdj16();

	//mergeSP();
	classificate();
	createClassImage1();
	classificate2();
	createClassImage2();

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



		/*
		cv::Mat meanMat( 1, 1, CV_8UC3, mean );
		qDebug() << meanMat.at<uchar>(0,0);
		cv::cvtColor( meanMat, meanMat, CV_BGR2HSV );
		qDebug() << meanMat.at<uchar>(0,0)  << "         " << "";


		mSlicoRes.copyTo(masked16, dmask8);
		masked16.convertTo(masked8, CV_8UC1);
		cv::imshow("masked", masked8 );
		cv::waitKey(0);

		mPattern16.copyTo(masked16, mask8);
*/

		// get adjacency
		set<int> adjSet;
		set<int>::const_iterator itAdj;
		int pix;

		int left	= (*its)->mLeft	  - 1;
		int right	= (*its)->mRight  + 2;
		int top		= (*its)->mTop    - 1;
		int bottom	= (*its)->mBottom + 2;
		if( left	< 0)		left = 0;
		if( right	>= mWidth)	right = mWidth;
		if( top		< 0)		top = 0;
		if( bottom	>= mHeight) bottom = mHeight;

		for( r = top; r < bottom; r++){
			for( c = left; c < right; c++){

				pix = masked16.at<unsigned short>(r,c);
				if( pix != 0  &&  adjSet.find(pix) == adjSet.end() ){
					adjSet.insert( pix);

				}
			}
		}
		// will be sorted
		for( itAdj = adjSet.begin(); itAdj != adjSet.end(); itAdj++){

			// real index in mSPV   !!!!!
			(*its)->addAdj( *itAdj - 1);
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
			int ndx = meanMat.step[0]*r + meanMat.step[1]*c;
			meanMat.data[ndx + 0] = mean.val[0];
			meanMat.data[ndx + 1] = mean.val[1];
			meanMat.data[ndx + 2] = mean.val[2];

		}
	}

	cv::imshow("meanMat", meanMat);

	cv::cvtColor( meanMat, meanMatHsv, CV_BGR2HSV );
	cv::inRange( meanMatHsv, cv::Scalar(100, 90, 140), cv::Scalar(125, 255, 255), meanMask);
	meanMat.copyTo(meanMatRes, meanMask);

	cv::imshow("meanMasked", meanMatRes);

}

void SkyDetect::classificate()
{
	std::priority_queue<int, vector<int>, compare> pqueue;

	// get indexes of superpixels that are on the top of image
	int is;
	for( is = 0; is < mSPV.size(); is++){
		// is on the top of image
		if( mSPV[is]->mTop == 0 ){
			pqueue.  push( is );
			//qDebug() << "is0     " << is;
		}
	}

	// while pqueue is not empty, classifite superpixel in it
	while( !pqueue.empty() ){
		// get index of superpixel,   clasifite it,
		// if sky, add its neighbours into queue



		int idxSP = pqueue.top();
		pqueue.pop();



		// classi
		// mSPV[idxSP]->isSky();
		int isSky = mSPV[idxSP]->mClass;

		if( isSky == UNKNOWN ){
			//qDebug() << "from eueue: " << idxSP;

			isSky = classificateSp( idxSP );

			if( isSky == SKY || isSky == MAYBE){
				// get adjencies
				ADJV::const_iterator ita;
				ADJV adjencies = mSPV[idxSP]->getAdjV();
				for( ita = adjencies.begin(); ita != adjencies.end(); ita++){

					isSky = mSPV[*ita]->mClass;
					if( isSky == UNKNOWN ){
						pqueue.push(*ita);
					}
				}
			}
		}
	}
}

int	SkyDetect::classificateSp(int idxSP)
{

	cv::Scalar mean = mSPV[idxSP]->getMean();

	cv::Mat meanMat( 1, 1, CV_8UC3, mean );
	cv::Mat mask( 1, 1, CV_8UC1 );

	cv::cvtColor( meanMat, meanMat, CV_BGR2HSV );

	// SKY
	cv::inRange( meanMat, cv::Scalar(100, 80, 130), cv::Scalar(120, 255, 255), mask);
	if( ! mask.at<uchar>(0,0) == 0 ){
		//qDebug() << "SKY";
		mSPV[idxSP]->mClass = SKY;
		return SKY;

	}

	// MAYBE
	cv::inRange( meanMat, cv::Scalar(85, 0, 100), cv::Scalar(165, 255, 255), mask);
	if( ! mask.at<uchar>(0,0) == 0 ){
		//qDebug() << "MAYBE";
		mSPV[idxSP]->mClass = MAYBE;
		return MAYBE;
	}

	// white
	cv::inRange( meanMat, cv::Scalar(0, 0, 150), cv::Scalar(255, 255, 255), mask);
	if( ! mask.at<uchar>(0,0) == 0 ){
		//qDebug() << "MAYBE";
		mSPV[idxSP]->mClass = MAYBE;
		return MAYBE;
	}

	//qDebug() << "NO_SKY";
	mSPV[idxSP]->mClass = NO_SKY;
	return NO_SKY;
}



// ===========================================================

void SkyDetect::createSKYandMAYBELists()
{
	int isSky;
	int is;
	std::list< int >::const_iterator its;

	for( its = listMAYBE.cbegin(); its != listMAYBE.cend(); its++ ){
		// get adjencies
		ADJV::const_iterator ita;
		ADJV adjencies = mSPV[*its]->getAdjV();

		for( ita = adjencies.cbegin(); ita != adjencies.cend(); ita++){
			//qDebug() << "--";
			isSky = mSPV[*ita]->mClass;
			if( isSky == SKY ){
				mSPV[*its]->addToListSKY(*ita);
				mSKYCounter++;

			} else if( isSky == MAYBE ){
				mSPV[*its]->addToListMAYBE(*ita);
			}

		}

	}

}


void SkyDetect::classificate2()
{
	int sum = 0;
	// create list of all MAYBE
	int is, adj;
	bool isSimilar = false;

	for( is = 0; is < mSPV.size(); is++){
		if( mSPV[is]->mClass == MAYBE){
			listMAYBE.push_back( is );
			//qDebug() << "is0     " << is;
			mSPV[is]->createMeanHSV();
		} else if(mSPV[is]->mClass == SKY) {
			mSPV[is]->createMeanHSV();
		}
	}

	// create SKY list & MAYBE list for all MAYBE in listMAYBE
	createSKYandMAYBELists();


	// while exist least one MAYBE with least one SKY
	while( mSKYCounter > 0 ){

		int is = listMAYBE.front();
		listMAYBE.pop_front();

		while( -1 < (adj = mSPV[is]->getOneSkyNeighbourt()) ){

			// exist SKY neighbourt
			mSKYCounter--;
			isSimilar = similar( is, adj );

			if( isSimilar ){
				// classifite it as SKY
				mSPV[is]->mClass = SKY;

				// add his to his adj as SKY
				ADJV::const_iterator ita;
				ADJV adjMAYBE = mSPV[is]->getAdjvMAYBE();
				for( ita = adjMAYBE.begin(); ita != adjMAYBE.end(); ita++){
					if(mSPV[*ita]->mClass == MAYBE){
						mSPV[*ita]->addToListSKY( is );
						mSKYCounter++;
					}
				}

				while( -1 != mSPV[is]->getOneSkyNeighbourt() ){
					mSKYCounter--;
				}

			}
		}

		if( mSPV[is]->mClass == MAYBE ){
			if( mSPV[is]->hasAdjMAYBE() ){
				// therte is a chance, so add it back to listMAYBE,
				listMAYBE.push_back( is );

			} else {
				// classificate it as NO_SKY2
				mSPV[is]->mClass = NO_SKY2;
			}
		}

		//createClassImage2();
		//qDebug() << "(" << is << ")  " <<  mSKYCounter <<   "   ---------------------------------------------";
		//cv::waitKey(0);

	}
}

bool SkyDetect::similar(int is1, int is2)
{

	cv::Scalar mean1 = mSPV[is1]->getMeanHSV();
	cv::Scalar mean2 = mSPV[is2]->getMeanHSV();
	//qDebug() << "1meanHSV: " << mean1.val[0] << " " << mean1.val[1] << " " << mean1.val[2];
	//qDebug() << "2meanHSV: " << mean2.val[0] << " " << mean2.val[1] << " " << mean2.val[2];

	double r1 = mean1.val[0];
	double g1 = mean1.val[1];
	double b1 = mean1.val[2];

	double r2 = mean2.val[0];
	double g2 = mean2.val[1];
	double b2 = mean2.val[2];

	double dr, dg, db;

	if( r1 > r2)	dr = r1-r2;
	else			dr = r2-r1;
	if( g1 > g2)	dg = g1-g2;
	else			dg = g2-g1;
	if( b1 > b2)	db = b1-b2;
	else			db = b2-b1;

	bool isSim = (dr < 8.5)  &&  (dg < 5)  &&  (db < 15);
	if (!isSim) {isSim = (dr < 100)  &&  (dg < 5)  &&  (db < 50); }

	return isSim;


	//return dr + dg + db < 13;
}

void SkyDetect::createClassImage1()
{
	cv::Mat resMean = cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC3);
	cv::Mat resMeanMaybe = cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC3);

	SPV::const_iterator its;
	for( its = mSPV.begin(); its != mSPV.end(); its++){
		if( (*its)->mClass == SKY  ||  (*its)->mClass == MAYBE ){

			cv::Scalar mean = (*its)->getMean();
			PIXV pixels = (*its)->getPixelV();

			// any name cant be 0, so we identify superpixels accoding theirs names
			PIXV::const_iterator ipt;
			for( ipt = pixels.begin(); ipt != pixels.end(); ipt++){

				int ndx = resMean.step[0]* ipt->r + resMean.step[1]* ipt->c;
				if( (*its)->mClass != MAYBE ){
					resMean.data[ndx + 0] = mean.val[0];
					resMean.data[ndx + 1] = mean.val[1];
					resMean.data[ndx + 2] = mean.val[2];
				}
				if( (*its)->mClass == MAYBE ){
					resMeanMaybe.data[ndx + 0] = mean.val[0];
					resMeanMaybe.data[ndx + 1] = mean.val[1];
					resMeanMaybe.data[ndx + 2] = mean.val[2];
				}

			}
		}
	}
	cv::imshow( "resultMean Class 1  " , resMean  );
	cv::imshow( "resultMean Class 1 with MAYBE ", resMeanMaybe  );

}

void SkyDetect::createClassImage2()
{
	cv::Mat resMean = cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC3);
	cv::Mat resMeanMaybe = cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC3);
	cv::Mat resMeanNo2 = cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC3);

	SPV::const_iterator its;
	for( its = mSPV.begin(); its != mSPV.end(); its++){
		if( (*its)->mClass == SKY  ||  (*its)->mClass == MAYBE  || (*its)->mClass == NO_SKY2){

			cv::Scalar mean = (*its)->getMean();
			PIXV pixels = (*its)->getPixelV();

			// any name cant be 0, so we identify superpixels accoding theirs names
			PIXV::const_iterator ipt;
			for( ipt = pixels.begin(); ipt != pixels.end(); ipt++){

				int ndx = resMean.step[0]* ipt->r + resMean.step[1]* ipt->c;
				if( (*its)->mClass == SKY ){
					resMean.data[ndx + 0] = mean.val[0];
					resMean.data[ndx + 1] = mean.val[1];
					resMean.data[ndx + 2] = mean.val[2];
				}
				if( (*its)->mClass == MAYBE ){
					resMeanMaybe.data[ndx + 0] = mean.val[0];
					resMeanMaybe.data[ndx + 1] = mean.val[1];
					resMeanMaybe.data[ndx + 2] = mean.val[2];
				}
				if( (*its)->mClass == NO_SKY2 ){
					resMeanNo2.data[ndx + 0] = mean.val[0];
					resMeanNo2.data[ndx + 1] = mean.val[1];
					resMeanNo2.data[ndx + 2] = mean.val[2];
				}

			}
		}
	}


	cv::imshow( "resultMean Class 2  " , resMean  );
	//cv::imshow( "resultMean Class 2 with MAYBE ", resMeanMaybe  );
	//cv::imshow( "resultMean Class 2 with NO_SKY2 ", resMeanNo2  );

}
