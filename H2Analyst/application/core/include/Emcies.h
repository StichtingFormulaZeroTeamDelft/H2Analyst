#pragma once

#include "Namespace.h"
#include "StringOperations.h"

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <boost/algorithm/string.hpp>

namespace H2A
{
	namespace Emcy
	{
		enum Severity : uint8_t { None = 0, Anomaly = 1, Critical = 2, Panic = 3, Notice = 4, Inhibiting = 5, Unknown = 6 };
		std::string getSeverityStr(H2A::Emcy::Severity severity);

		enum Level { System, FC, HighSide, Traction, LowSide, LV };

		struct Properties {
			std::string text;
			bool hide;
		};

		struct Emcy {
			double time;
			std::string source;
			uint16_t code;
			H2A::Emcy::Severity severity;
			uint8_t details[4];
			std::string description;

			bool operator < (const Emcy& rhs) const {
				return time < rhs.time;
			}
		};
		void readPayload(Emcy& emcy, const uint64_t& payload);

		void readEmcyCodesFromSettings(std::map<uint16_t, H2A::Emcy::Properties>& emcyMap);
	}
}

