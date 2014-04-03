#ifndef  __SPIXEL_H__
#define __SPIXEL_H__

#include <vector>
#include <algorithm>

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

	void	addPixel(int r, int c);
	void	addAdj(int adj);

	PIXV	getPixelV() const;
	ADJV	getAdjV() const;

	int		getPixelVSize() const;
	int		getAdjVSize() const;
	void	setName( int name);
	int		getName() const;


private:
	int		mName;
	PIXV	mPixelV;
	ADJV	mAdjV;
};

#endif
