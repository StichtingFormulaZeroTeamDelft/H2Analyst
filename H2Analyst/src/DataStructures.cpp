#include "DataStructures.h"


const std::vector<double> H2A::Dataset::timeVec() const
{
	std::vector<double> time = timeVector;
	std::for_each(time.begin(), time.end(), [this](double& t) {t += datafile->timeOffset; });
	return time;
}
