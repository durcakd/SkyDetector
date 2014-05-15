#ifndef SKYDETECTORDIALOG_H
#define SKYDETECTORDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
//#include <QListWidget>



#include "SkyDetectorThread.h"
#include "SpinBox.h"
#include "LineEdit.h"
#include "constants.h"

class SkyDetectorDialog : public QDialog
	{
	Q_OBJECT
public:
	explicit SkyDetectorDialog(QWidget *parent = 0);

	const SkyDetectorThread *getThread() const		{ return &_thread; }


public slots:
	void threadDone();

private slots:
	bool open();
	void start();

private:
	void prepareParam();


	SkyDetectorThread _thread;

	QPushButton *_openFileB;
	QPushButton *_startB;
	PARAMETERS	_parm;


	QString		_fileName;
	QLineEdit	*_spcountLE;
	QLineEdit	*_compactnessLE;
	SpinBox	*_sky1SB;
	SpinBox	*_sky2SB;
	SpinBox	*_sky3SB;
	SpinBox	*_sky4SB;
	SpinBox	*_sky5SB;
	SpinBox	*_sky6SB;
	SpinBox	*_maybe1SB;
	SpinBox	*_maybe2SB;
	SpinBox	*_maybe3SB;
	SpinBox	*_maybe4SB;
	SpinBox	*_maybe5SB;
	SpinBox	*_maybe6SB;
	SpinBox	*_white1SB;
	SpinBox	*_white2SB;
	SpinBox	*_white3SB;
	SpinBox	*_white4SB;
	SpinBox	*_white5SB;
	SpinBox	*_white6SB;

	LineEdit	*_sim1aLE;
	LineEdit	*_sim1bLE;
	LineEdit	*_sim1cLE;
	LineEdit	*_sim2aLE;
	LineEdit	*_sim2bLE;
	LineEdit	*_sim2cLE;
	LineEdit	*_sim3aLE;
	LineEdit	*_sim3bLE;
	LineEdit	*_sim3cLE;

	};

#endif
