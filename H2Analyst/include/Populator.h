#pragma once

#include <iostream>

#include <QObject>
#include <QRunnable>
#include <QMutex>
#include <QThreadPool>

#include <armadillo>

#include "DataStructures.h"


class Populator :
	public QObject
{

	Q_OBJECT

	H2A::Datafile* m_datafile = nullptr;

public:

	Populator(H2A::Datafile* datafile = nullptr);
	
public slots:
	void run();
	void startNextDataset();

signals:
	void finished();
	void datasetPopulated(const H2A::Dataset* dataset);
};



class PopulatorWorker
	: public QObject
{

	Q_OBJECT

	H2A::Dataset* m_ds;

public:
	PopulatorWorker(H2A::Dataset* dataset);
	
public slots:
	void run();

signals:
	void finished(const H2A::Dataset* dataset);


};