#ifndef  __SKYDETECT_H__
#define __SKYDETECT_H__

#include "SPixel.h"
#include <vector>

#include <opencv2\core\core.hpp>

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
	cv::Mat getResult();

	int		doSlico( const QString filename,
					 const QString saveLocation );

	void	createPicBuffer(const QString filename,
							unsigned int*&	imgBuffer,
							int&	width,
							int&	height) const;
	cv::Mat createSPLabelsMat( const int* labels, int width, int height);
	void  initSPixelsFromLabels( const int* labels, int width, int height);
	void  initSPixelAdj( int width, int height);





private:

	int		mSpcount;
	double	mCompactness;
	SLIC	*mSlico;

	cv::Mat mImageIn;
	cv::Mat mImage2;
	cv::Mat mImageRes;
	cv::Mat mSlicoRes;
	SPV mSPV;

};

#endif
