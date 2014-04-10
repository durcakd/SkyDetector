#ifndef  __SKYDETECT_H__
#define __SKYDETECT_H__

#include "SPixel.h"
#include <vector>

#include <opencv2/core/core.hpp>

#include <QString>

class SLIC;
//typedef unsigned int UINT;

using namespace std;

typedef vector< SPixel* > SPV;

class SkyDetect
{
public:

	SkyDetect( );
	~SkyDetect(void);

	int		detect();

	void	openImage(const QString filename);
	void	applyFiltersBefore();
	void	createPicBuffer( unsigned int*&	imgBuffer);
	int		doSlico( const QString filename, const QString saveLocation );

	void	initSPixelsFromLabels( const int* labels);
	void	createPattern();
	void	createPattern( const int* labels);
	void	initSPixelAdj16();

	void	mergeSP();
	void	classificateSP();


private:

	int		mSpcount;
	double	mCompactness;
	int*	mLabels;
	SLIC	*mSlico;
	int		mWidth;
	int		mHeight;
	int		mSize;

	cv::Mat mImageIn;
	cv::Mat mSlicoRes;
	cv::Mat mPattern16;




	cv::Mat mImage2;
	//cv::Mat mImageRes;
	SPV mSPV;

};

#endif
