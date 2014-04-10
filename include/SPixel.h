#ifndef  __SPIXEL_H__
#define __SPIXEL_H__

#include <vector>
#include <algorithm>
#include <opencv2/core/core.hpp>

using namespace std;


typedef struct pix{
	int r;
	int c;
} PIX;

//typedef vector< pair<int, int> > PIXV;
typedef vector< PIX > PIXV;
typedef vector< int > ADJV;


class SPixel
{
public:

	SPixel( );
	~SPixel(void);

	void		addPixel(int r, int c);
	void		addAdj(int adj);

	PIXV		getPixelV() const;
	ADJV		getAdjV() const;

	int			getPixelVSize() const;
	int			getAdjVSize() const;
	void		setName( int name);
	int			getName() const;
	void		setMean(const cv::Scalar mean);
	cv::Scalar	getMean() const;
	void		computeBoundary();
	int			mLeft, mRight, mTop, mBottom;


private:
	int			mName;
	//int			mLeft, mRight, mTop, mBottom;

	PIXV		mPixelV;
	ADJV		mAdjV;
	cv::Scalar	mMean;

};

#endif
