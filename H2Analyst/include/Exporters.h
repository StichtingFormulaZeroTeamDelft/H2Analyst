#pragma once

#include "DataStructures.h"
#include "DataOperations.h"

#include <iostream>
#include <string>
#include <fstream>

namespace H2A
{
	namespace Export
	{

		void CSV(const std::vector<const H2A::Dataset*> datasets, const std::string filename, const uint16_t resamplingFreq = 10);

	}
}

