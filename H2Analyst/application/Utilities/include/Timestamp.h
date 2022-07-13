#pragma once

#include <iostream>
#include <vector>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

/**
* Timestamp to make operators with date/time easier.
**/
struct Timestamp
{

	boost::posix_time::ptime timePoint;
	void set(std::vector<uint16_t> vec);

	bool operator< (const Timestamp& ts) const {
		return timePoint < ts.timePoint;
	}

	bool operator> (const Timestamp& ts) const {
		return timePoint > ts.timePoint;
	}

	boost::posix_time::time_duration operator- (const Timestamp& ts) const {
		return timePoint - ts.timePoint;
	}

	Timestamp operator+ (const boost::posix_time::time_duration& dur) const {
		Timestamp ts((*this));
		ts.timePoint += dur;
		return ts;
	}

	friend std::ostream& operator<<(std::ostream& os, const Timestamp& dt);
};
