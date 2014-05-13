#include "SkyDetectorThread.h"
#include <QTime>
#include <QElapsedTimer>
#include <QDebug>

SkyDetectorThread::SkyDetectorThread()
{
	_abort		= false;
}

/*void SkyDetectorThread::setParameters(const PARAMETERS &parm,  int *givenData , bool autoParams){
	_parm = parm;
}*/

void SkyDetectorThread::setAbort(){
	_abort = true;
}

void SkyDetectorThread::run(){
	QTime time;
	time.start();

	//emit sendProgress( countTrials );
	//emit sentStatusMsg( tr("Processing...")  );
	_abort = false;

	double runTime = time.elapsed() / 1000.0;

	QString msg;
	msg = tr("Solution not found, try again (") + QString::number(runTime, 'f', 2) + tr(" s)") ;
	if( _abort ){ msg = tr("Aborted"); }
	//emit done( msg);
	//emit sentStatusMsg( msg );
}












