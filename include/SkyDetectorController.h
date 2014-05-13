#ifndef SKYDETECTORCONTROLLER_H
#define SKYDETECTORCONTROLLER_H

#include <QApplication>

#include "SkyDetectorDialog.h"
#include "SkyDetect.h"

//(double[]);
//Q_DECLARE_METATYPE(double[] );

class SkyDetectorController
	{
public:
	SkyDetectorController(QApplication *app);
	~SkyDetectorController();

private:
	void createConnections();

	QApplication *_app;
	SkyDetectorDialog *_skyDetectorDialog;
	};

#endif
