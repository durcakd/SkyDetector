#ifndef  __SPIXEL_H__
#define __SPIXEL_H__

#include <vector>
#include <algorithm>

using namespace std;


typedef struct pix{
	int x;
	int y;
} PIX;

//typedef vector< pair<int, int> > PIXV;
typedef vector< PIX > PIXV;
typedef vector< int > ADJV;


class SPixel
{
public:

	SPixel( );
	~SPixel(void);

	void	addPixel(int x, int y);
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
