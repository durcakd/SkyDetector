#ifndef SKYDETECTORTHREAD_H
#define SKYDETECTORTHREAD_H

#include <QThread>


class SkyDetectorThread : public QThread
	{
	Q_OBJECT
public:
	explicit SkyDetectorThread();

	//void setParameters( const PARAMETERS &parm);
	void setAbort();

signals:

protected:
	void run();

private:
	bool _abort;
	//PARAMETERS _parm;

};

#endif
