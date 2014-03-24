#ifndef  __SKYDETECT_H__
#define __SKYDETECT_H__

#include <vector>

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

#include "SLIC.h"
typedef unsigned int UINT;

using namespace std;

class SkyDetect
{
public:
	SkyDetect( );
	~SkyDetect(void);

	int		detect();
	cv::Mat getResult();
	int		doSLICO();
	int		getPictures( vector<string>& picvec );
	void	getPictureBuffer( string&		filename,
								UINT*&		imgBuffer,
								int&		width,
								int&		height);

private:
	int mSpcount;
	double mCompactness;
	SLIC mSlic;


	cv::Mat mImageIn;
	cv::Mat mImage2;
	cv::Mat mImageRes;
};

#endif
