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


}












