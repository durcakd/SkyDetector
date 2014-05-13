#ifndef SKYDETECTORDIALOG_H
#define SKYDETECTORDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
//#include <QLineEdit>
//#include <QListWidget>
//#include <QProgressBar>
//#include <QCheckBox>
//#include <QGroupBox>
#include <QStatusBar>

#include "SkyDetectorThread.h"


class SkyDetectorDialog : public QDialog
	{
	Q_OBJECT
public:
	explicit SkyDetectorDialog(QWidget *parent = 0);

	//QListWidget *getListWidged() const	{ return listWidged; }
	//QPushButton *getStartB() const		{ return _startB; }
	//QPushButton *getOpenFileB() const	{ return _openFileB; }
	//const SkyDetectorThread *getThread() const		{ return &_thread; }
	//QProgressBar *getBarPB() const	{ return _barPB; }
	//QStatusBar	*getStatusBar() const	{ return _statusBar; }

signals:
	//void requestForReadFile(const QString &);
	//void sentStatusMsg( const QString &msg, int timeout = 0 ) const;

public slots:
	//void addStrToListWidged(const QString &str);
	//void threadDone(const QString msg);

private slots:
	bool open();
	void start();

	//void on_popSizeLE_textChanged(const QString &str);

private:
	SkyDetectorThread thread;
	//QListWidget *listWidged;
	//QStatusBar	*statusBar;


	QPushButton *_openFileB;
	QPushButton *_startB;



	//QGroupBox	*_parametersGroup;

	//QProgressBar *_barPB;

	//PARAMETERS parm;

	};

#endif
