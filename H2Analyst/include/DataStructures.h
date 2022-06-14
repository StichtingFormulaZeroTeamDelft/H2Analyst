#pragma once

#include <QVariant>
#include <QMutex>

#include <string>
#include <vector>

#include <armadillo>

namespace H2A
{

	struct Datafile;

	struct Dataset // A Dataset contains one timeseries of a single signal.
	{
		QMutex mutex = QMutex(QMutex::NonRecursive); // Mutex for multi-thread protection

		Datafile* datafile = nullptr;

		std::string name = "Not set";
		std::string quantity = "Not set";
		std::string unit = "Not set";
		uint8_t length = 0;
		uint8_t byteOffset = 0;
		uint16_t id = 0;
		uint32_t uid = 0;
		uint32_t datatype = 0;
		float offset = 0.0;
		float scale = 0.0;

		std::vector<double> timeVec = std::vector<double>();
		std::vector<double> dataVec = std::vector<double>();

		bool volatile populating = false;
		bool volatile populated = false;
	};

	struct Datafile
		// A Datafile is a bundle of Datasets that belong to each other.
	{
		QMutex mutex = QMutex(QMutex::NonRecursive); // Mutex for multi-thread protection

		std::string name = "Not set";
		std::vector<uint16_t> startTime = std::vector<uint16_t>();
		std::vector<Dataset*> datasets = std::vector<Dataset*>();

		arma::Row<uint16_t>* message_ids = nullptr;
		arma::Row<float> *message_time = nullptr;
		arma::Mat<uint8_t> *messages = nullptr;

		std::vector<Dataset*> populationPrioList = std::vector<Dataset*>();
	};

}

