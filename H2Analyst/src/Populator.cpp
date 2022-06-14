#include "Populator.h"


Populator::Populator(H2A::Datafile* datafile) : QObject()
{
	m_datafile = datafile;
}

void Populator::run()
{
	// Create threadpool used to populate datasets, always keeping at least 2 threads free for the other tasks on the system
	std::cout << "Starting population of " << m_datafile->name << std::endl;
	this->startNextDataset();
}


void Populator::startNextDataset()
{

	H2A::Dataset* ds = nullptr;
	QThread* thread = nullptr;

	// Check if there are datasets in prio list of the datafile
	m_datafile->mutex.lock();
	if (m_datafile->populationPrioList.size() > 0)
	{
		ds = m_datafile->populationPrioList.back();
		m_datafile->populationPrioList.pop_back();
	}
	// Nothing in prio list, so select the first unpopulated dataset
	else
	{
		for (auto const& dataset : m_datafile->datasets)
		{
			if (!dataset->populated && !dataset->populating)
			{
				ds = dataset;
				break;
			}
		}
	}
	m_datafile->mutex.unlock();

	// If the ds pointer is not a nullptr, a dataset was found to be populated
	if (ds)
	{
		ds->mutex.lock();
		ds->populating = true;
		ds->mutex.unlock();

		thread = new QThread();
		PopulatorWorker* worker = new PopulatorWorker(ds);
		worker->moveToThread(thread);

		connect(thread, &QThread::started, worker, &PopulatorWorker::run);
		// Start population of next dataset, emit signal to notify others and quit the thread when finished
		connect(worker, &PopulatorWorker::finished, this, &Populator::startNextDataset);
		connect(worker, &PopulatorWorker::finished, [=](const H2A::Dataset* dataset) { emit datasetPopulated(dataset); });
		connect(thread, &QThread::finished, thread, &QThread::quit);
		// Clean up when finished
		connect(worker, &PopulatorWorker::finished, worker, &PopulatorWorker::deleteLater);
		connect(thread, &QThread::finished, thread, &QThread::deleteLater);
		
		thread->start();

	}
	// No dataset found, so all datasets are populated and datafile population is done
	else
	{
		// If there is still a thread alive, wait for it to finish
		if (thread)
			thread->wait();

		std::cout << "Finished population of " << m_datafile->name << std::endl;
		emit finished();
		return;
	}
}



PopulatorWorker::PopulatorWorker(H2A::Dataset* dataset) : QObject()
{
	m_ds = dataset;
}


void PopulatorWorker::run()
{
	// Find indices of messages that match the ID of this dataset
	arma::uvec mess_cols = arma::find(*(m_ds->datafile->message_ids) == m_ds->id);
	arma::uvec mess_rows(m_ds->length);
	for (arma::uword i = 0; i < mess_rows.size(); ++i) mess_rows(i) = m_ds->byteOffset + i;

	// Extract submatrix from message matrix with found indices
	arma::Mat<uint8_t> messages = m_ds->datafile->messages->submat(mess_rows, mess_cols);

	// Lock dataset to prepare for data insertion (thread-protection)
	m_ds->mutex.lock();

	// Time vector
	m_ds->timeVec = arma::conv_to<std::vector<double>>::from(m_ds->datafile->message_time->cols(mess_cols));

	// Data vector
	m_ds->dataVec = std::vector<double>(messages.n_cols);
	uint64_t temp_val;
	for (size_t col = 0; col < messages.n_cols; ++ col)
	{
		// Combine bytes of message into single temporary variable using bitwise operations
		temp_val = 0;
		for (size_t row = 0; row < messages.n_rows; ++row)
		{
			temp_val |= (static_cast<uint64_t>(messages(row, col)) << (8 * row));
		}

		// Convert temporary variable into right type using reinterpret_casting
		switch (m_ds->datatype)
		{
		case 0:
		{
			uint8_t* conv_result = reinterpret_cast<uint8_t*>(&temp_val);
			m_ds->dataVec[col] = *conv_result;
			break;
		}
		case 1:
		{
			int8_t* conv_result = reinterpret_cast<int8_t*>(&temp_val);
			m_ds->dataVec[col] = *conv_result;
			break;
		}
		case 2:
		{
			uint16_t* conv_result = reinterpret_cast<uint16_t*>(&temp_val);
			m_ds->dataVec[col] = *conv_result;
			break;
		}
		case 3:
		{
			int16_t* conv_result = reinterpret_cast<int16_t*>(&temp_val);
			m_ds->dataVec[col] = *conv_result;
			break;
		}
		case 4:
		{
			uint32_t* conv_result = reinterpret_cast<uint32_t*>(&temp_val);
			m_ds->dataVec[col] = *conv_result;
			break;
		}
		case 5:
		{
			int32_t* conv_result = reinterpret_cast<int32_t*>(&temp_val);
			m_ds->dataVec[col] = *conv_result;
			break;
		}
		case 6:
		{
			uint64_t* conv_result = reinterpret_cast<uint64_t*>(&temp_val);
			m_ds->dataVec[col] = *conv_result;
			break;
		}
		case 7:
		{
			int64_t* conv_result = reinterpret_cast<int64_t*>(&temp_val);
			m_ds->dataVec[col] = *conv_result;
			break;
		}
		case 8:
		{
			float* conv_result = reinterpret_cast<float*>(&temp_val);
			m_ds->dataVec[col] = *conv_result;
			break;
		}
		case 9:
		{
			double* conv_result = reinterpret_cast<double*>(&temp_val);
			m_ds->dataVec[col] = *conv_result;
			break;
		}
		}

		// Apply scaling and offset to data
		m_ds->dataVec[col] *= m_ds->scale;
		m_ds->dataVec[col] += m_ds->offset;
	}

	m_ds->populated = true;
	m_ds->mutex.unlock();
	
	//std::cout << "Finished population of " << m_ds->name << std::endl;
	
	emit finished(m_ds);
}

