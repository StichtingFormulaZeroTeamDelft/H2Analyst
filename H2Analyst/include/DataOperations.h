#pragma once

#include "DataStructures.h"


namespace H2A
{

	void resample(const H2A::Dataset* dataset, uint16_t freq, std::vector<double>& timeResult, std::vector<double>& dataResult);
	void resample(const H2A::Dataset* dataset, std::vector<double>& time, std::vector<double>& dataResampled);
	void resample(const std::vector<const H2A::Dataset*> datasets, std::vector<double>& timeResampled, std::vector<std::vector<double>>& dataResampled, uint16_t frequency = 0);

	uint16_t samplingFreq(const H2A::Dataset* dataset);

}