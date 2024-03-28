#include "Emcies.h"

/**
* Function that converts the numerical severity value to a string with the readable severity.
**/
std::string H2A::Emcy::getSeverityStr(H2A::Emcy::Severity severity) {

	std::string s;
	switch (severity)
	{
	case H2A::Emcy::None:
		s = "None";
		break;
	case H2A::Emcy::Event:
		s = "Event";
		break;
	case H2A::Emcy::Anomaly:
		s = "Anomaly";
		break;
	case H2A::Emcy::Critical:
		s = "Critical";
		break;
	case H2A::Emcy::Panic:
		s = "Panic";
		break;
	case H2A::Emcy::Notice:
		s = "Notice";
		break;
	case H2A::Emcy::Inhibiting:
		s = "Inhibiting";
		break;
	case H2A::Emcy::Unknown:
		s = "Unknown";
		break;
	default:
		break;
	}
	return s;
}

std::string H2A::Emcy::getLevelStr(H2A::Emcy::Level level) {
	std::string s;
	switch (level)
	{
	case H2A::Emcy::NONE:
		s = "None";
		break;
	case H2A::Emcy::SYSTEM:
		s = "System";
		break;
	case H2A::Emcy::LV:
		s = "Low Voltage";
		break;
	case H2A::Emcy::HV:
		s = "High Voltage";
		break;
	case H2A::Emcy::TR_GENERAL:
		s = "Traction General";
		break;
	case H2A::Emcy::TR_FRONT:
		s = "Traction Front";
		break;
	case H2A::Emcy::TR_REAR:
		s = "Traction Rear";
		break;
	case H2A::Emcy::FC_GENERAL:
		s = "Fuel Cell General";
		break;
	case H2A::Emcy::FC_LEFT:
		s = "Fuel Cell Left";
		break;
	case H2A::Emcy::FC_RIGHT:
		s = "Fuel Cell Right";
		break;
	case H2A::Emcy::HIGH_SIDE:
		s = "High side";
		break;
	case H2A::Emcy::LOW_SIDE:
		s = "Low side";
		break;
	}
	return s;
}

H2A::Emcy::Level H2A::Emcy::getLevel(std::string str) {
	H2A::Emcy::Level level;
	if (str == "SYSTEM_LEVEL_NONE") {
		level = H2A::Emcy::NONE;
	}
	else if ((str == "EMCY_LEVEL_SYSTEM") || (str == "SYSTEM_LEVEL_SYSTEM")) {
		level = H2A::Emcy::SYSTEM;
	}
	else if ((str == "EMCY_LEVEL_LV") || (str == "SYSTEM_LEVEL_LV")) {
		level = H2A::Emcy::LV;
	}
	else if ((str == "EMCY_LEVEL_HV") || (str == "SYSTEM_LEVEL_HV")) {
		level = H2A::Emcy::HV;
	}
	else if ((str == "EMCY_LEVEL_TRACTION") || (str == "SYSTEM_LEVEL_TR_GENERAL")) {
		level = H2A::Emcy::TR_GENERAL;
	}
	else if (str == "SYSTEM_LEVEL_TR_FRONT") {
		level = H2A::Emcy::TR_FRONT;
	}
	else if (str == "SYSTEM_LEVEL_TR_REAR") {
		level = H2A::Emcy::TR_REAR;
	}
	else if ((str == "EMCY_LEVEL_FC") || (str == "SYSTEM_LEVEL_FC_GENERAL")) {
		level = H2A::Emcy::FC_GENERAL;
	}
	else if (str == "SYSTEM_LEVEL_FC_LEFT") {
		level = H2A::Emcy::FC_LEFT;
	}
	else if (str == "SYSTEM_LEVEL_FC_RIGHT") {
		level = H2A::Emcy::FC_RIGHT;
	}
	else if (str == "EMCY_LEVEL_HIGH_SIDE") {
		level = H2A::Emcy::HIGH_SIDE;
	}
	else if (str == "EMCY_LEVEL_LOW_SIDE") {
		level = H2A::Emcy::LOW_SIDE;
	}
	else {
		level = H2A::Emcy::NONE;
		std::cout << "No valid system level found" << std::endl;
	}
	return level;
}

void H2A::Emcy::readPayload(Emcy& emcy, const uint64_t& payload) {
	emcy.code = payload & 0xFFFF;
	emcy.severity = static_cast<H2A::Emcy::Severity>(static_cast<uint8_t>((payload >> 24) & 0xFF));
}

void H2A::Emcy::readEmcyCodesFromSettings(std::map<uint16_t, H2A::Emcy::Properties>& map, H2A::Car car) {
	map.clear();

	QString fileName;

	switch (car) {
	case H2A::Car::Forze8:
		fileName = F8_EMCY_FILE;
		break;
	case H2A::Car::Forze9:
		fileName = F9_EMCY_FILE;
		break;
	}

	std::cout << "Loading EMCY definitions from " << fileName.toStdString() << std::endl;

	QFile file(fileName);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream in(&file);
		std::string line;
		while (!in.atEnd()) {
			line = in.readLine().toStdString();

			if (line.size() < 5) continue;

			// Split line based on delimiter ';'
			std::vector<std::string> split_line;
			boost::split(split_line, line, boost::is_any_of(";"));

			H2A::Emcy::Properties properties;
			uint16_t code;

			// Emcy code
			std::stringstream ss;
			ss << std::hex << StrOps::trim_copy(split_line[0]);
			ss >> code;

			// Emcy level
			properties.level = getLevel(StrOps::trim_copy(split_line[1]));

			// Text
			properties.text = StrOps::trim_copy(split_line[2]);

			// Hide flag (if this is set, the EMCY will not be shown in the EMCY list)
			properties.hide = StrOps::trim_copy(split_line[3]) == "hide";

			//std::cout << std::hex << code << std::dec << "; " << properties.text << "; " << properties.hide << std::endl;

			// Add code with description to map for later reference.
			// First check if this code does not already exist in map.
			if (map.find(code) != map.end()) {
				H2A::logWarning("Duplicate EMCY codes defined. Please check the EMCY codes definition.");
			}
			map[code] = properties;
		}
		file.close();
	}

}
