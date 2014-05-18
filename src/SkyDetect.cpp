#include "SkyDetect.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QFileInfo>
#include <QDebug>
#include <set>
#include <queue>

#include "SLIC.h"


SkyDetect::SkyDetect( const PARAMETERS &parm )
{
	mParm			= parm;
	mSlico			= new SLIC();
	mSpcount		= mParm.spcount;
	mCompactness	= mParm.compactness;
	mLabels			= NULL;

	mSKYCounter = 0;
}

SkyDetect::~SkyDetect(void)
{
	if(mLabels){
		delete [] mLabels;
	}
	delete mSlico;
}

int SkyDetect::detect()
{
	qDebug() << "DETECT ..... ";
	QString filename	 = mParm.fileName;

	openImage( filename );
	applyFiltersBefore();

	doSlico( filename);

	initSPixelsFromLabels( mLabels);
	createPattern();
	initSPixelAdj16();

	//classificate();
	classificateTOP();
	//createClassImage1();
	classificate2();
	solveClouds();

	createClassImage2();

	saveResultImg( mResultImg, filename );

	qDebug() << "DONE ";
	cv::waitKey(0);

	return 0;
}

int SkyDetect::doSlico( const QString filename )
{
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
	//mSlico->PerformSLICO_ForGivenStepSize( imgBuf, mWidth, mHeight,
	//									   mLabels, numlabels,
	//									   mSpcount, mCompactness ); //for a given number K of superpixels
	qDebug() << " - PerformSLICO_ForGivenK:	Done";

	mSlico->DrawContoursAroundSegmentsTwoColors( imgBuf, mLabels,
												 mWidth, mHeight );//for black-and-white contours around superpixels
	qDebug() << " - DrawContoursAroundSegmentsTwoColors: Done";

	mSlicoRes = cv::Mat( mHeight, mWidth, CV_8UC4, imgBuf );

	//cv::imshow("SLICO result image", mSlicoRes );
	//cv::waitKey(1);

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

void SkyDetect::saveResultImg(const cv::Mat img, const QString filename) const
{
	QFileInfo file(filename);
	QString outFileName = filename.left(filename.lastIndexOf(".")) + "_out2.jpg";
	qDebug() << "OUTPUT: " << outFileName;
	cv::imwrite( outFileName.toStdString(), img );
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

}

void SkyDetect::createPattern()
{
	cv::Mat pattern16(cv::Size( mWidth, mHeight), CV_16UC1);
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

	//cv::Mat pattern8(cv::Size( mWidth, mHeight), CV_8UC1);
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
	int counter = mParm.spcount;
	for( its = mSPV.begin(); its != mSPV.end(); its++){
		if(counter-- % 101 == 0 ) qDebug() << "SP " << counter;
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
		//(*its)->setMean( cv::mean( mSlicoRes, mask8 ));  // old

		cv::Scalar mean, stdDev;
		cv::meanStdDev(mSlicoRes, mean, stdDev, mask8);
		(*its)->setStdDev( stdDev);  // new
		(*its)->setMean( mean);  // new
		(*its)->createMeanHSV();

		//cv::Scalar meanHSV = (*its)->getMeanHSV();
		//qDebug() << "mean: " << meanHSV.val[0] << " " << meanHSV.val[1] << " " << meanHSV.val[2] <<  "         "  << stdDev.val[0] << " " << stdDev.val[1] << " " << stdDev.val[2];
		//cv::Mat test  = cv::Mat( mWidth, mHeight, CV_8UC3, mean );
		//cv::imshow("mean", test );



		// dilate mast
		cv::dilate(mask8, dmask8, strElmnt);
		cv::bitwise_xor( mask8, dmask8, mask8 );

		// mask pattern
		cv::Mat masked16;
		mPattern16.copyTo(masked16, mask8);




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
	qDebug() << "Prepare done, detecting ...";
}


////   Detecting   //////////////////////////////////////////////////////


void SkyDetect::classificateTOP()
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
		else{
			mSPV[is]->mClass = MAYBE;
		}
	}

	// while pqueue is not empty, classifite superpixel in it
	while( !pqueue.empty() ){
		// get index of superpixel,   clasifite it,
		// if sky, add its neighbours into queue
		int idxSP = pqueue.top();
		pqueue.pop();

		//cv::Scalar meanHSV = mSPV[idxSP]->getMeanHSV();
		//cv::Scalar stdDev = mSPV[idxSP]->getStdDev();
		//qDebug() << "TOP mean: " << meanHSV.val[0] << " " << meanHSV.val[1] << " " << meanHSV.val[2] <<  "         "  << stdDev.val[0] << " " << stdDev.val[1] << " " << stdDev.val[2];



		// classi
		mSPV[idxSP]->mClass = SKY;

		/*
		int isSky = mSPV[idxSP]->mClass;
		if( isSky == UNKNOWN ){
			isSky = classificateSp( idxSP );
		}*/
	}
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


void SkyDetect::solveClouds()
{
	std::priority_queue<int, vector<int>, compare> pqueue;

	// get indexes of superpixels that are not SKY and are on border
	int is;
	for( is = 0; is < mSPV.size(); is++){
		// is on the top of image
		bool isBorder = false;
		if( mSPV[is]->mTop == 0 ){
			isBorder = true;
		} else if( mSPV[is]->mLeft == 0){
			isBorder = true;
		} else if( mSPV[is]->mRight == mWidth - 1){
			isBorder = true;
		} else if( mSPV[is]->mBottom == mHeight - 1 ){
			isBorder = true;
		}

		if( isBorder && mSPV[is]->mClass != SKY ){
			pqueue.push( is );
			mSPV[is]->mClass = NO_SKY2;
		}
	}

	int isSky;
	// while pqueue is not empty,
	while( !pqueue.empty() ){
		int idxSP = pqueue.top();
		pqueue.pop();

		// get adjencies
		ADJV::const_iterator ita;
		ADJV adjencies = mSPV[idxSP]->getAdjV();
		for( ita = adjencies.begin(); ita != adjencies.end(); ita++){
			isSky = mSPV[*ita]->mClass;
			if( isSky != NO_SKY2  &&  isSky != SKY ){
				pqueue.push(*ita);
				mSPV[*ita]->mClass = NO_SKY2;
			}
		}
	}

	// if is not NO_SKY2 -> will be SKY
	for( is = 0; is < mSPV.size(); is++){
		if( mSPV[is]->mClass != NO_SKY2 ){
			mSPV[is]->mClass = SKY;
		}
	}
}

int	SkyDetect::classificateSp(int idxSP)
{

	cv::Scalar mean = mSPV[idxSP]->getMeanHSV();

	cv::Mat meanMat( 1, 1, CV_8UC3, mean );
	cv::Mat mask( 1, 1, CV_8UC1 );

	//cv::cvtColor( meanMat, meanMat, CV_BGR2HSV );

	// SKY
	cv::inRange( meanMat, cv::Scalar(mParm.sky1, mParm.sky2, mParm.sky3), cv::Scalar(mParm.sky4, mParm.sky5, mParm.sky6), mask);
	if( ! mask.at<uchar>(0,0) == 0 ){
		//qDebug() << "SKY";
		mSPV[idxSP]->mClass = SKY;
		return SKY;

	}

	// MAYBE
	cv::inRange( meanMat, cv::Scalar(mParm.maybe1, mParm.maybe2, mParm.maybe3), cv::Scalar(mParm.maybe4, mParm.maybe5, mParm.maybe6), mask);
	if( ! mask.at<uchar>(0,0) == 0 ){
		//qDebug() << "MAYBE";
		mSPV[idxSP]->mClass = MAYBE;
		return MAYBE;
	}

	// white
	cv::inRange( meanMat, cv::Scalar(mParm.white1, mParm.white2, mParm.white3), cv::Scalar(mParm.white4, mParm.white5, mParm.white6), mask);
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

	double coef = (b1+b2)/512 - 0.5;
	coef = coef < 0.0 ? 0.0 : coef;
	//qDebug() << "coef = " << coef;

	// 150,50,100
	bool isSim = (dr < mParm.sim1a +coef*150)  &&  (dg < mParm.sim1b+coef*20)  &&  (db < mParm.sim1c +coef*100);
	if (!isSim) {isSim = (dr < mParm.sim2a)  &&  (dg < mParm.sim2b)  &&  (db < mParm.sim2c); }
	if (!isSim) {isSim = (dr < mParm.sim3a)  &&  (dg < mParm.sim3b)  &&  (db < mParm.sim3c); }

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
	//cv::imshow( "1 SKY    " , resMean  );
	//cv::imshow( "1 MAYBE ", resMeanMaybe  );

}

void SkyDetect::createClassImage2()
{
	cv::Mat resMean = cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC3);
	cv::Mat resMeanMaybe = cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC3);
	cv::Mat resMeanNo2 = cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC3);
	cv::Mat pattern= cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC3);

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
				pattern.data[ndx + 0] = mean.val[0];
				pattern.data[ndx + 1] = mean.val[1];
				pattern.data[ndx + 2] = mean.val[2];

			}
		}
	}

	cv::imshow( "Mean " , pattern  );
	//cv::imshow( "2 Result" , resMean  );
	createResultImg( resMean );

	//cv::imshow( "resultMean Class 2 with MAYBE ", resMeanMaybe  );
	//cv::imshow( "resultMean Class 2 with NO_SKY2 ", resMeanNo2  );

}

void SkyDetect::createResultImg( const cv::Mat resSPimg )
{

	mResultImg = cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC3);
	cv::Mat mask = cv::Mat::zeros(cv::Size( mWidth, mHeight), CV_8UC1);
	cv::Mat dmask = cv::Mat(cv::Size( mWidth, mHeight), CV_8UC1);

	cv::threshold(resSPimg, mask, 0, 255, cv::THRESH_BINARY );

	cv::Mat strElmnt = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7,7));

	// dilate mast
	cv::dilate(mask, dmask, strElmnt);
	cv::bitwise_xor( mask, dmask, mask );
	cv::bitwise_not( mask, mask);

	cv::imshow( "Mask" , mask  );

	mImageIn.copyTo(mResultImg, mask);

	cv::imshow( "Result" , mResultImg  );

}
