#pragma once

#include <QObject>
#include <QThreadPool>
#include <QFileDialog>

#include <vector>
#include <string>
#include <iostream>

#include "Parsers.h"
#include "DataStructures.h"
#include "Populator.h"


class DataStore : public QObject
{

	Q_OBJECT

private:
	std::vector<H2A::Datafile*> m_Datafiles;

	void loadFileFromName(const std::string& filename);

public:

	DataStore();

	const std::vector<H2A::Datafile*>& getDatafiles();
	void requestDatasetPopulation(const H2A::Dataset* dataset);
	void loadFiles(QStringList &files);

signals:
	void fileLoaded();
	void datasetChanged(const H2A::Dataset* dataset);

};


