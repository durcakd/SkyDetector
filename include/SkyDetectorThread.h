#ifndef SKYDETECTORTHREAD_H
#define SKYDETECTORTHREAD_H

#include <QThread>
#include "constants.h"
#include "SkyDetect.h"


class SkyDetectorThread : public QThread
	{
	Q_OBJECT
public:
	explicit SkyDetectorThread();

	void setParameters( const PARAMETERS &parm);
	void setAbort();

protected:
	void run();

private:
	bool _abort;
	PARAMETERS _parm;
	SkyDetect *_skyDetector;
};

#endif
