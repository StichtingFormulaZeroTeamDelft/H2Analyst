#include "DataStore.h"


DataStore::DataStore() :
m_Datafiles() {
}

const std::vector<H2A::Datafile*>& DataStore::getDatafiles() {
	return m_Datafiles;
}

void DataStore::requestDatasetPopulation(const H2A::Dataset* dataset) {
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

/**
* Load the file with given filename into the dataStore.
* 
* @param filename Filename of file to load.
**/
H2A::Datafile* DataStore::loadFileFromName(const std::string& filename) {
	H2A::Datafile* df = new H2A::Datafile;

	// Todo: select parser based on filetype
	H2A::Parsers::IntCanLog(filename, df);
	this->createPopulator(df);

	return df;
}

/**
*
**/
void DataStore::createPopulator(H2A::Datafile* datafile) {
	// Start populating the data
	// Todo: check if population is necessary for loaded file
	// Todo: make auto-population an option that can be toggled
	QThread* pop_thread = new QThread();
	Populator* df_populator = new Populator(datafile);
	df_populator->moveToThread(pop_thread);

	// Connect signals and slots
	connect(pop_thread, SIGNAL(started()), df_populator, SLOT(run()));
	connect(df_populator, SIGNAL(finished()), pop_thread, SLOT(quit()));
	connect(df_populator, SIGNAL(finished()), df_populator, SLOT(deleteLater()));
	connect(pop_thread, SIGNAL(finished()), pop_thread, SLOT(deleteLater()));
	connect(df_populator, &Populator::datasetPopulated, [=](const H2A::Dataset* dataset) {emit datasetChanged(dataset); });

	datafile->populationThread = pop_thread;
	//pop_thread->start();
}

/**
* Load the given list files into the datastore.
* 
* @param files Files to load.
**/
void DataStore::loadFiles(const QStringList &files) {
	if (files.size() == 0) return;

	std::vector<H2A::Datafile*> datafiles;

	// If more than 1 datafile is in the list, ask to align and/or merge time vectors
	bool alignTime = false;
	bool mergeData = false;
	if (files.size() > 1 || m_Datafiles.size() > 0) {
		alignTime = H2A::Dialog::question("Align time vector?");
		if (alignTime) mergeData = H2A::Dialog::question("Merge datasets?");
	}

	// Load files
	for (const auto& file : files)
		datafiles.push_back(this->loadFileFromName(file.toStdString()));

	// If requested, align time and merge
	if (mergeData) {
		this->alignTimeVectors(datafiles);
		m_Datafiles.push_back(this->mergeData(datafiles));
	}
	else {
		m_Datafiles.insert(m_Datafiles.end(), datafiles.begin(), datafiles.end());
		if (alignTime) {
			this->alignTimeVectors(m_Datafiles);
		}
	}

	emit fileLoaded();

	// Start population of all datafiles that are not being populated yet
	for (const auto& datafile : m_Datafiles) {
		if (!datafile->populationStarted) datafile->populationThread->start();
	}

}

/**
* Adds a padding to the time vectors to make the given datafiles have the same start time.
**/
void DataStore::alignTimeVectors(std::vector<H2A::Datafile*> datafiles) {
	if (datafiles.size() == 0) return;

	// Find datafile that start at the earliest timestamp
	H2A::Datafile* first = datafiles.front();
	for (const auto& datafile : datafiles) {
		if (datafile->startTime < first->startTime)
			first = datafile;
	}

	// Apply offset to all datafiles to align time vectors
	for (const auto& datafile : datafiles) {
		if (datafile == first) continue;
		
		boost::posix_time::time_duration diff = datafile->startTime - first->startTime;
		long ms = diff.total_milliseconds();
		datafile->timeOffset = static_cast<double>(std::floor(ms / 1000) + (static_cast<double>(ms % 1000)) / 1000.0);
		//datafile->startTime = first->startTime;
	}
}

H2A::Datafile* DataStore::mergeData(std::vector<H2A::Datafile*> datafiles)
{
	if (datafiles.size() == 1) return datafiles.front();

	// Sort datafiles based on startTime
	std::sort(datafiles.begin(), datafiles.end(), [](const H2A::Datafile* lhs, const H2A::Datafile* rhs) {
		return lhs->startTime < rhs->startTime;
	});

	// Create new datafile
	H2A::Datafile* df = new H2A::Datafile;
	df->name = "Merged";
	df->startTime = datafiles.front()->startTime;
	df->endTime = datafiles.back()->endTime;

	// Store datasets of first datafile in new datafile.
	df->datasets = datafiles.front()->datasets;
	// Remove all datasets that are not in all other datafiles.
	for (const auto& datafile : datafiles) {
		df->datasets.erase(std::remove_if(df->datasets.begin(), df->datasets.end(),
			[datafile](H2A::Dataset* ds) {
				return std::find_if(datafile->datasets.begin(), datafile->datasets.end(), [ds](H2A::Dataset* ds2) {
					return ds->uid == ds2->uid;
					}) == datafile->datasets.end();
			}), df->datasets.end());
	}

	// Set datafile of datasets to the new merged datafile
	for (const auto& dataset : df->datasets) dataset->datafile = df;

	// Create message ID, time and message objects by concatenating the ones in the merging datafiles
	arma::Row<uint16_t> messageIDs;
	arma::Row<double> messageTimes;
	arma::Mat<uint8_t> messages;
	for (const auto& datafile : datafiles) {
		messageIDs = arma::join_horiz(messageIDs, *(datafile->message_ids));
		messages = arma::join_horiz(messages, *(datafile->messages));
		//messageTimes = arma::join_horiz(messageTimes, *(datafile->message_time));
		arma::Row<double> offsetMessageTimes = *(datafile->message_time);
		offsetMessageTimes += (arma::Row<double>(offsetMessageTimes.n_cols, arma::fill::ones) * datafile->timeOffset);
		messageTimes = arma::join_horiz(messageTimes, offsetMessageTimes);
	}
	df->message_ids = new arma::Row<uint16_t>(messageIDs);
	df->message_time = new arma::Row<double>(messageTimes);
	df->messages = new arma::Mat<uint8_t>(messages);
	
	// Create new populator for this datafile
	this->createPopulator(df);

	return df;
}
