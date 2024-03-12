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

void H2A::Emcy::readPayload(Emcy& emcy, const uint64_t& payload) {
	emcy.code = payload & 0xFFFF;
	emcy.severity = static_cast<H2A::Emcy::Severity>(static_cast<uint8_t>((payload >> 24) & 0xFF));
}

void H2A::Emcy::readEmcyCodesFromSettings(std::map<uint16_t, H2A::Emcy::Properties>& map) {
	map.clear();

	std::cout << std::filesystem::current_path() << std::endl;

	std::ifstream file("data\\default_settings\\emcy_codes.txt");
	std::string line;
	if (file.is_open()) {
		while (std::getline(file, line)) {
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

			// Text
			properties.text = StrOps::trim_copy(split_line[1]);

			// Hide flag (if this is set, the EMCY will not be shown in the EMCY list)
			properties.hide = StrOps::trim_copy(split_line[2]) == "hide";

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
