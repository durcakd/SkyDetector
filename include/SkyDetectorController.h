#ifndef SKYDETECTORCONTROLLER_H
#define SKYDETECTORCONTROLLER_H

#include <QApplication>

#include "SkyDetectorDialog.h"
#include "SkyDetect.h"


class SkyDetectorController
	{
public:
	SkyDetectorController(QApplication *app = NULL);
	~SkyDetectorController();

private:
	void createConnections();

	QApplication *_app;
	SkyDetectorDialog *_skyDetectorDialog;
	};

#endif
