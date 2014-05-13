#ifndef  __SKYDETECT_H__
#define __SKYDETECT_H__

#include "SPixel.h"
#include <vector>
#include <list>

#include <opencv2/core/core.hpp>

#include <QString>
#include <constants.h>

class SLIC;
//typedef unsigned int UINT;

using namespace std;

typedef vector< SPixel* > SPV;

class SkyDetect
{
public:

	SkyDetect( const PARAMETERS &parm );
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
	void	classificate();
	int		classificateSp(int idxSP);
	void	createClassImage1();
	void	createClassImage2();

	void	classificate2();
	void	createSKYandMAYBELists();
	bool	similar(int is1, int is2);

private:

	PARAMETERS mParm;

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


	std::list< int > listMAYBE;
	int		mSKYCounter;

	cv::Mat mImage2;
	//cv::Mat mImageRes;
	SPV mSPV;

	double maxd;

};

#endif
