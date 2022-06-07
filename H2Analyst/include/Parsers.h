#pragma once


#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <armadillo>

#include "DataStructures.h"


namespace H2A
{
	const float INTCANLOG_SAMPLING_TIME = 1.e-4;

	namespace Parsers
	{
		void IntCanLog(const std::string& filename, H2A::Datafile *datafile);
	}
}


