#include "SkyDetectorThread.h"
#include <QTime>
#include <QElapsedTimer>
#include <QDebug>

SkyDetectorThread::SkyDetectorThread()
{
	_abort		= false;
	_skyDetector = NULL;
}

void SkyDetectorThread::setParameters(const PARAMETERS &parm ){
	_parm = parm;
}

void SkyDetectorThread::setAbort(){
	_abort = true;
}

void SkyDetectorThread::run(){
	delete _skyDetector;

	_skyDetector = new SkyDetect( _parm );
	_skyDetector->detect();

}












