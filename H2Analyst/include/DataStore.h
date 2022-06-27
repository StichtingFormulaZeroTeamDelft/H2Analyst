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
#include "Dialogs.h"


class DataStore : public QObject
{

	Q_OBJECT


	std::vector<H2A::Datafile*> m_Datafiles;

	H2A::Datafile* loadFileFromName(const std::string& filename);
	void createPopulator(H2A::Datafile* datafile);
	void alignTimeVectors(std::vector<H2A::Datafile*> datafiles);
	H2A::Datafile* mergeData(std::vector<H2A::Datafile*> datafiles);

public:

	DataStore();

	const std::vector<H2A::Datafile*>& getDatafiles();
	void requestDatasetPopulation(const H2A::Dataset* dataset);
	void loadFiles(const QStringList &files);

signals:
	void fileLoaded();
	void datasetChanged(const H2A::Dataset* dataset);

};


