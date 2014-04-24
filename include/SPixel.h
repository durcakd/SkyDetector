#ifndef  __SPIXEL_H__
#define __SPIXEL_H__

#include <vector>
#include <algorithm>
#include <opencv2/core/core.hpp>

using namespace std;

struct compare  {
	bool operator()(const int& l, const int& r) {
		return l > r;
	}
};

typedef struct pix{
	int r;
	int c;
} PIX;

//typedef vector< pair<int, int> > PIXV;
typedef vector< PIX > PIXV;
typedef vector< int > ADJV;


const enum { UNKNOWN, SKY, NO_SKY, MAYBE };

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

	void		addToListSKY(int adj);
	void		addToListMAYBE(int adj);
	int			getOneSkyNeighbourt();



	int			mLeft, mRight, mTop, mBottom;  // need getter
	int			mClass;							// need getter, setter

private:
	int			mName;
	//int			mLeft, mRight, mTop, mBottom;

	PIXV		mPixelV;
	ADJV		mAdjV;
	cv::Scalar	mMean;

	ADJV		mListSKY;
	ADJV		mListMAYBE;


};

#endif
