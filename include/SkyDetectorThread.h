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
	void done(const QString msg);
	//void sendProgress( int tryStep );
	//void sentStatusMsg( const QString &msg, int timeout = 0 ) const;

protected:
	void run();

private:
	bool _abort;
	//PARAMETERS _parm;

};

#endif
