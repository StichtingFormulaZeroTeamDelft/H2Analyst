#include "DataOperations.h"

/**
* Resample dataset to given frequency.
* 
* @param dataset Dataset to resample.
* @param freq Frequency to resample data to.
* @param timeResampled Vector to store resampled time.
* @param dataResampled Vector to store resampled data.
**/
void H2A::resample(const H2A::Dataset* dataset, uint16_t freq, std::vector<double>& timeResampled, std::vector<double>& dataResampled)
{
	double dt = 1.0 / freq;
	float duration = dataset->timeVec.back() - dataset->timeVec.front();
	size_t nSteps = std::floor(duration / dt);
	std::cout << nSteps << std::endl;

	// Create resampled time vector
	std::vector<double> time(nSteps, 0.0);
	for (size_t step = 0; step < nSteps; ++step)
		time[step] = step * dt;

	std::vector<double> data(nSteps, 0.0);
	size_t cursor = 1; // Start at 1 to prevent errors when setting data
	for (size_t step = 0; step < nSteps; ++ step)
	{
		// Set cursor to first index where time in original data is later than current resampled index
		while (dataset->timeVec[cursor] < time[step])
		{
			if (cursor < dataset->timeVec.size())
				++cursor;
			else
				break;
		}

		// Store previous datapoint in resampled data
		data[step] = dataset->dataVec[cursor - 1];
	}

	timeResampled = time;
	dataResampled = data;
}

/**
* Resample dataset to match given time vector.
*
* @param dataset Dataset to resample.
* @param time Time vector to resample data with (must contain ascending timestamps).
* @param dataResampled Vector to store resampled data.
**/
void H2A::resample(const H2A::Dataset* dataset, std::vector<double> time, std::vector<double>& dataResampled, bool trimTime)
{
	// Get start and end time of dataset to be resampled for checking
	auto minMaxTime = std::minmax_element(dataset->timeVec.begin(), dataset->timeVec.end());
	double tStart = *minMaxTime.first;
	double tEnd = *minMaxTime.second;

	dataResampled = std::vector<double>(time.size(), 0.0);

	// Check if endTime in resampled time vector is after start time of dataset. If not, there is not data to resample
	if (time.back() <= tStart) return;

	// Skip to first datapoint that is after first dataset datapoint (in time)
	size_t step = 0;
	while (time[step] <= tStart)
		++step;

	// Trim away unset datapoints
	if (trimTime) {
		dataResampled.erase(dataResampled.begin(), dataResampled.begin() + step);
		time.erase(time.begin(), time.begin() + step);
		step = 0;
	}

	// Actual resampling
	size_t cursor = 1; // Start at 1 instead of 0 to prevent errors when setting data using '[cursor - 1]'
	while (true)
	{
		if (step >= time.size()) break; // End of time vector reached
		if (time[step] >= tEnd) break; // End of dataset datapoints reached, rest of resampled data keeps their initial values

		// Set cursor to first index where time in original data is later than current resampled index
		while (dataset->timeVec[cursor] < time[step])
		{
			if (cursor < dataset->timeVec.size())
				++cursor;
			else
				break;
		}

		// Store previous datapoint in resampled data
		dataResampled[step] = dataset->dataVec[cursor - 1];
		++step;
	}
}

void H2A::resample(const std::vector<const H2A::Dataset*> datasets, std::vector<double>& timeResampled, std::vector<std::vector<double>>& dataResampled, uint16_t frequency)
{
	std::vector<double> data;
	uint16_t freq = frequency;

	// Check if dataset vector is OK for this function
	if (datasets.size() == 0) return;
	if (datasets.size() < 2)
	{
		if (freq == 0)
		{
			dataResampled.push_back(datasets.front()->dataVec);
			return;
		}
		else
		{
			H2A::resample(datasets.front(), freq, timeResampled, data);
			dataResampled.push_back(data);
		}
		return;
	}

	// If no frequency is passed, use the lowest frequency of the supplied datasets
	if (freq == 0)
	{
		freq = H2A::samplingFreq(datasets.front());
		for (const auto& dataset : datasets)
		{
			freq = std::min({ freq, H2A::samplingFreq(dataset) });
		}
	}

	// If frequency calculation failed somehow, catch if before trying to continue
	if (freq < 1)
	{
		std::cout << "Error: Invalid frequency calculated... aborting resample" << std::endl;
		return;
	}

	// First, the dataset overlap in time is calculated.
	// This overlap is used to generate the new timevector that will be used for resampling.
	auto minMaxTime = std::minmax_element(
		datasets.front()->timeVec.begin(),
		datasets.front()->timeVec.end());
	double tStart = *minMaxTime.first;
	double tEnd = *minMaxTime.second;
	for (size_t i = 1; i < datasets.size(); ++ i) // First dataset used to set initial values
	{
		minMaxTime = std::minmax_element(
			datasets[i]->timeVec.begin(),
			datasets[i]->timeVec.end());
		tStart = std::max({ tStart, *minMaxTime.first });
		tEnd = std::min({ tEnd, *minMaxTime.second });
	}

	// Create time vector
	size_t nSteps = std::floor((tEnd - tStart) * freq);
	timeResampled = std::vector<double>(nSteps);
	double dt = 1.0 / freq;
	for (size_t step = 0; step < nSteps; ++step)
		timeResampled[step] = tStart + dt * step;
	
	for (const auto& dataset : datasets)
	{
		H2A::resample(dataset, timeResampled, data); // time vector has already been set by preceding code
		dataResampled.push_back(data);
	}
	return;
}


/**
* Determine sampling frequency of dataset
* 
* @param dataset Dataset to find frequency of.
**/
uint16_t H2A::samplingFreq(const H2A::Dataset* dataset)
{
	// Assuming time vector might not be monotonically increasing
	auto minMaxTime = std::minmax_element(dataset->timeVec.begin(), dataset->timeVec.end());
	double duration = *minMaxTime.second - *minMaxTime.first;
	return round(dataset->timeVec.size() / duration);
}

