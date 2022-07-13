#include "Timestamp.h"

void Timestamp::set(std::vector<uint16_t> vec) {
	if (vec.size() != 7) return;
	timePoint = boost::posix_time::ptime(boost::gregorian::date(vec[0], vec[1], vec[2]),
		boost::posix_time::hours(vec[3]) +
		boost::posix_time::minutes(vec[4]) +
		boost::posix_time::seconds(vec[5]) +
		boost::posix_time::milliseconds(vec[6]));
}

std::ostream& operator<<(std::ostream& os, const Timestamp& dt) {
	os << boost::posix_time::to_simple_string(dt.timePoint);
	return os;
}
