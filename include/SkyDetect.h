#ifndef  __SKYDETECT_H__
#define __SKYDETECT_H__

#include <vector>

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

#include <QString>

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
	int		doSLICO( const QString filename,
					 const QString saveLocation );
	void	createPicBuffer(const QString filename,
							UINT*&	imgBuffer,
							int&	width,
							int&	height) const;

private:
	int mSpcount;
	double mCompactness;
	SLIC mSlic;


	cv::Mat mImageIn;
	cv::Mat mImage2;
	cv::Mat mImageRes;
};

#endif
