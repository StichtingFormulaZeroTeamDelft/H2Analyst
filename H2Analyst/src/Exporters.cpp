#include "Exporters.h"

/**
* CSV export function. Creates a file with given filename and exports the given datasets to it. Optionally, the data is resampled to a given frequency before exporting.
*
* @param datasets Datasets to export.
* @param filename Filename of the created file.
* @param resamplingFreq Frequency to resample all data to before exporting.
**/
void H2A::Export::CSV(const std::vector<const H2A::Dataset*> datasets, const std::string filename, const uint16_t resamplingFreq) {

	std::cout << "Exporting " << datasets.size() << " datasets to CSV format... ";

	std::ofstream file(filename);

	// Write header to file
	file << "time,";
	for (const auto& dataset : datasets) {
		file << dataset->name;
		if (dataset != datasets.back()) file << ",";
	}
	file << "\n";

	// Determine time span of datasets and create resampled time vector
	double tStart = datasets.front()->timeVec.front();
	double tEnd = datasets.front()->timeVec.back();
	for (const auto& dataset : datasets) {
		auto minMaxTime = std::minmax_element(dataset->timeVec.begin(), dataset->timeVec.end());
		tStart = std::min({ tStart, *minMaxTime.first });
		tEnd = std::max({ tEnd, *minMaxTime.second });
	}

	// Create time vector
	double duration = tEnd - tStart;
	std::vector<double> time(duration * resamplingFreq);
	for (size_t i = 0; i < time.size(); ++i)
		time[i] = tStart + i * (1.0 / resamplingFreq);

	// Resample data
	std::vector<std::vector<double>> resampledData;
	for (size_t i = 0; i < datasets.size(); ++i)
		resampledData.push_back(std::vector<double>(time.size()));
	for (size_t i = 0; i < datasets.size(); ++i)
		H2A::resample(datasets[i], time, resampledData[i], false);

	// Write data to file
	for (size_t time_i = 0; time_i < time.size(); ++ time_i) {
		file << time[time_i] << ",";
		for (size_t dataset_i = 0; dataset_i < datasets.size(); ++ dataset_i) {
			file << resampledData[dataset_i][time_i];
			if (dataset_i != datasets.size() - 1) file << ",";
		}
		file << "\n";
	}
	file.close();
	
	std::cout << "done" << std::endl;
}