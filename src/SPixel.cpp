#include "SPixel.h"

#include <QDebug>

SPixel::SPixel()
{
	mClass = UNKNOWN;
}

SPixel::~SPixel(void)
{
	mPixelV.clear();
	mAdjV.clear();
}

void SPixel::setName( int name)
{
	mName = name;
}

int SPixel::getName() const
{
	return mName;
}

int SPixel::getPixelVSize() const
{
	return mPixelV.size();
}

int SPixel::getAdjVSize() const
{
	return mAdjV.size();

}

void SPixel::addPixel(int r, int c)
{
	PIX pix;
	pix.r = r;
	pix.c = c;

	//mPixelV.push_back( make_pair(x,y));
	mPixelV.push_back( pix );
}

void SPixel::computeBoundary()
{
	PIXV::const_iterator it;
	it = mPixelV.begin();

	mLeft	= it->c;
	mRight	= it->c;
	mTop	= it->r;
	mBottom = it->r;

	for( it = mPixelV.begin(); it != mPixelV.end(); it++){
		if( mLeft	> it->c)  mLeft		= it->c;
		if( mRight	< it->c)  mRight	= it->c;
		if( mTop	> it->r)  mTop		= it->r;
		if( mBottom < it->r)  mBottom	= it->r;
	}
}

void SPixel::addAdj(int adj)
{
	mAdjV.push_back(adj);
}

PIXV SPixel::getPixelV() const
{
	PIXV::const_iterator it;
	/*qDebug() << "SuperPixel: " << mName << "  pixs:";
	for( it = mPixelV.begin(); it != mPixelV.end(); it++){
		qDebug() << "  " << it->x << " " << it->y;
	}*/

	return mPixelV;
}
ADJV SPixel::getAdjV() const
{
	//ADJV::const_iterator it;
	//qDebug() << "SuperPixel: " << mName << "  adjs:";
	//for( it = mAdjV.begin(); it != mAdjV.end(); it++){
	//	qDebug() << "    " << *it;
	//}
	return mAdjV;
}

void SPixel::setMean(const cv::Scalar mean)
{
	mMean = mean;
}

cv::Scalar SPixel::getMean() const
{
	return mMean;
}

void SPixel::addToListSKY(int adj)
{
	mListSKY.push_back(adj);
}

void SPixel::addToListMAYBE(int adj)
{
	mListMAYBE.push_back(adj);
}

int	SPixel::getOneSkyNeighbourt()
{
	if( mListSKY.empty()){
		return -1;
	}

	int back = mListSKY.back();
	mListSKY.pop_back();
	return back;
}

bool SPixel::hasAdjMAYBE()
{
	return ! mListMAYBE.empty();
}

ADJV SPixel::getAdjvMAYBE() const
{
	return mListMAYBE;
}



