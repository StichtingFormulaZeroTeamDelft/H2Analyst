#pragma once

#include <QVariant>
#include <QMutex>

#include <string>
#include <vector>

#include <armadillo>

#include "Timestamp.h"

namespace H2A
{
	
	struct Datafile;

	/**
	* A Dataset contains one timeseries of a single signal.
	**/
	struct Dataset
	{
	private:
		std::vector<double> timeVector = std::vector<double>();

	public:
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

		void setTimeVec(std::vector<double> time) { timeVector = time; };
		const std::vector<double> timeVec() const;

		std::vector<double> dataVec = std::vector<double>();
		std::vector<uint64_t> byteVec = std::vector<uint64_t>();

		bool volatile populating = false;
		bool volatile populated = false;
	};

	/**
	* A Datafile is a bundle of Datasets that belong to each other.
	**/
	struct Datafile
	{
		QMutex mutex = QMutex(QMutex::NonRecursive); // Mutex for multi-thread protection

		std::string name = "Not set";
		Timestamp startTime;
		Timestamp endTime;
		double timeOffset = 0.0;
		std::vector<Dataset*> datasets = std::vector<Dataset*>();

		arma::Row<uint16_t>* message_ids = nullptr;
		arma::Row<double>* message_time = nullptr;
		arma::Mat<uint8_t>* messages = nullptr;

		bool volatile populationStarted = false;
		QThread* populationThread;
		std::vector<Dataset*> populationPrioList = std::vector<Dataset*>();
	};
}

