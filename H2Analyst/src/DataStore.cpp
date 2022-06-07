#include "DataStore.h"


DataStore::DataStore()
{
}


const std::vector<H2A::Datafile*>& DataStore::getDatafiles()
{
	return m_Datafiles;
}


void DataStore::requestDatasetPopulation(const H2A::Dataset* dataset)
{
	// This function adds the given dataset to the population priority list of its datafile

	H2A::Datafile* datafile = dataset->datafile;
	for (const auto& ds : datafile->datasets)
	{
		if (ds == dataset)
		{
			datafile->mutex.lock();
			datafile->populationPrioList.push_back(ds);
			datafile->mutex.unlock();
		}
	}
}


void DataStore::loadFileFromName(const std::string& filename)
{
	H2A::Datafile* df = new H2A::Datafile;

	// Todo: select parser based on filetype
	H2A::Parsers::IntCanLog(filename, df);

	m_Datafiles.push_back(df);
	emit fileLoaded();

	// Start populating the data
	// Todo: check if population is necessary for loaded file
	// Todo: make auto-population an option that can be toggled
	QThread *pop_thread = new QThread();
	Populator* df_populator = new Populator(df);
	df_populator->moveToThread(pop_thread);

	// Connect signals and slots
	connect(pop_thread, SIGNAL(started()), df_populator, SLOT(run()));
	connect(df_populator, SIGNAL(finished()), pop_thread, SLOT(quit()));
	connect(df_populator, SIGNAL(finished()), df_populator, SLOT(deleteLater()));
	connect(pop_thread, SIGNAL(finished()), pop_thread, SLOT(deleteLater()));
	connect(df_populator, &Populator::datasetPopulated, [=](const H2A::Dataset* dataset) {emit datasetChanged(dataset); });

	pop_thread->start();
}


void DataStore::loadFiles(QStringList &files)
{

	//std::string filename = "C:/Users/dptre/Dropbox/Forze/Data/INTCANLOG2-20170922T185408-IDLE-HV-FC.mat";
	//std::string filename = "C:/Users/dptre/Dropbox/Forze/Data/INTCANLOG2-20190714T171600-IDLE-HV-FC.mat";
	std::string filename = "C:/Users/Daan/Dropbox/Forze/Data/INTCANLOG2-20190818T164831-IDLE-HV-FC.mat";

	for (const auto& file : files)
	{
		this->loadFileFromName(file.toStdString());
	}
}
