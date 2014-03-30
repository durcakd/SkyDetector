#include "SPixel.h"

#include <QDebug>

SPixel::SPixel()
{


}

SPixel::~SPixel(void)
{
	mPixelV.clear();
	mAdjV.clear();
}

void SPixel::addPixel(int x, int y)
{
	PIX pix;
	pix.x = x;
	pix.y = y;

	//mPixelV.push_back( make_pair(x,y));
	mPixelV.push_back( pix );
}

void SPixel::addAdj(int adj)
{
	mAdjV.push_back(adj);
}

PIXV SPixel::getPixelV() const
{
	PIXV::const_iterator it;
	qDebug() << "SuperPixel: " << mName << "  pixs:";
	for( it = mPixelV.begin(); it != mPixelV.end(); it++){
		qDebug() << "  " << it->x << " " << it->y;
	}

	return mPixelV;
}
ADJV SPixel::getAdjV() const
{
	ADJV::const_iterator it;
	qDebug() << "SuperPixel: " << mName << "  adjs:";
	for( it = mAdjV.begin(); it != mAdjV.end(); it++){
		qDebug() << "    " << *it;
	}
	return mAdjV;
}

void SPixel::setName( int name)
{
	mName = name;
}

