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
#include <QFile>
#include <QTextStream>

namespace H2A
{
	namespace Emcy
	{

		static QString F8_EMCY_FILE = ":/data/emcy-codes-f8";
		static QString F9_EMCY_FILE = ":/data/emcy-codes-f9";

		enum Severity : uint8_t { None = 0, Event = 1, Anomaly = 2, Notice = 3, Inhibiting = 4, Critical = 5, Panic = 6, Unknown = 7 };
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

		void readEmcyCodesFromSettings(std::map<uint16_t, H2A::Emcy::Properties>& emcyMap, H2A::Car car);
	}
}

