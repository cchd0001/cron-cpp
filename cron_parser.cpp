#include <map>
#include <string>
#include "cron_err.h"
#include "cron_parser.h"
#include "cron_spec.h"
#include "cron_const.h"
#include "StringFunc.h"

#include <vector>
#include <cstring>
namespace Cron {

	TimeItem bounds::GetField(const std::string & str) const {
		TimeItem ret = 0ULL;
		auto rules = SplitString(str, ',');
		for (const auto & rule : rules) {
			ret |= getRange(rule);
		}
		return ret;
	}

	TimeItem bounds::getRange(const std::string & str) const {
		int start = 0, end = 0, step = 0;
		auto rangeAndStep = SplitString(str, '/');
		auto lowAndHigh = SplitString(str, '-');

		TimeItem extra_star = 0ULL;
		if (lowAndHigh[0][0] == '*' || lowAndHigh[0][0] == '?') {
			start = min;
			end = max;
			extra_star = SpecSchedule::AnyMark;
		}
		else {
			start = parseIntOrName(lowAndHigh[0]);
			if (lowAndHigh.size() == 1) {
				end = start;
			}
			else if (lowAndHigh.size() == 2) {
				end = parseIntOrName(lowAndHigh[1]);
			}
			else {
				throw InvalidExpression;
			}
		}

		if (rangeAndStep.size() == 1) {
			step = 1;
		}
		else if (rangeAndStep.size() == 2) {
			step = mustParseInt(rangeAndStep[1]);
			if (lowAndHigh.size() == 1) {
				end = max;
			}
		}
		else {
			throw InvalidExpression;
		}
		if (start > end || start < min || end > max) {
			throw InvalidExpression;
		}
		return getBits(start, end, step) | extra_star;
	}

	int bounds::parseIntOrName(const std::string & str) const {
		auto rangeAndStep = SplitString(str, '/');
		auto lowAndHigh = SplitString(str, '-');
		std::string real_str = "";
		if (rangeAndStep.size() > 1 && lowAndHigh.size() > 1) {
			throw InvalidExpression;
		}
		else if (rangeAndStep.size() == 2) { // Come 1/2 
			real_str = rangeAndStep[0];
		}
		else if (lowAndHigh.size() == 2) { // Come 1-2
			real_str = lowAndHigh[0];
		}
		else {
			real_str = str;
		}
		if (!names.empty()) {
			if (names.find(ToLower(real_str)) != names.end()) {
				return names.at(ToLower(real_str));
			}
		}
		return mustParseInt(real_str);
	}

	int bounds::mustParseInt(const std::string & str) const {
		int ret = 0;
		try {
			ret = std::stoi(str);
		}
		catch (...) {
			throw InvalidExpression;
		}
		return ret;
	}

	TimeItem bounds::getBits(int start, int end, int step)const {
		TimeItem ret = 0ULL;
		for (auto i = start; i <= end; i += step) {
			ret |= 1ULL << i;
		}
		return ret;
	}

	const bounds seconds{
		0 , 59
	};

	const bounds minutes{
		0 , 59
	};

	const bounds hours{
		0 , 23
	};

	const bounds dom{
		1 , 31
	};

	const bounds months{
		1 , 12 , {
			{"jan", 1},
				{"feb", 2},
				{"mar", 3},
				{"apr", 4},
				{"may", 5},
				{"jun", 6},
				{"jul", 7},
				{"aug", 8},
				{"sep", 9},
				{"oct", 10},
				{"nov", 11},
				{"dec", 12},
		}
	};

	const bounds dow{
		0 , 6 , {
			{"sun", 0},
				{"mon", 1},
				{"tue", 2},
				{"wed", 3},
				{"thu", 4},
				{"fri", 5},
				{"sat", 6},
		}
	};

	ISchedulePtr SpecParser::parseDescriptor(const std::string & str) {
		auto ret = std::make_shared<SpecSchedule>();
		if (str == "@yearly" || str == "@annually") {
			ret->m_sec = 1ULL << seconds.min;
			ret->m_min = 1ULL << minutes.min;
			ret->m_hour = 1ULL << hours.min;
			ret->m_dom = 1ULL << dom.min;
			ret->m_mon = 1ULL << months.min;
			ret->m_dow = dow.All();
		}
		else if (str == "@monthly") {
			ret->m_sec = 1ULL << seconds.min;
			ret->m_min = 1ULL << minutes.min;
			ret->m_hour = 1ULL << hours.min;
			ret->m_dom = 1ULL << dom.min;
			ret->m_mon = months.All();
			ret->m_dow = dow.All();
		}
		else if (str == "@weekly") {
			ret->m_sec = 1ULL << seconds.min;
			ret->m_min = 1ULL << minutes.min;
			ret->m_hour = 1ULL << hours.min;
			ret->m_dom = dom.All();
			ret->m_mon = months.All();
			ret->m_dow = 1ULL << dow.min;
		}
		else if (str == "@daily" || str == "@midnight") {
			ret->m_sec = 1ULL << seconds.min;
			ret->m_min = 1ULL << minutes.min;
			ret->m_hour = 1ULL << hours.min;
			ret->m_dom = dom.All();
			ret->m_mon = months.All();
			ret->m_dow = dow.All();
		}
		else if (str == "@hourly") {
			ret->m_sec = 1ULL << seconds.min;
			ret->m_min = 1ULL << minutes.min;
			ret->m_hour = hours.All();
			ret->m_dom = dom.All();
			ret->m_mon = months.All();
			ret->m_dow = dow.All();
		}
		else if (strstr(str.c_str() , "@every ") == str.c_str() ) {
            auto fileds = Fields(str) ;
            if( fileds.size() >= 2 ){
			    return Every(fileds[1]);
            }
		} else {
            ret = nullptr;
        }
		return ret;
	}

	ISchedulePtr SpecParser::returnNullandThrow(const std::string & str) {
		auto err = InvalidExpression;
		err.Append(str);
		throw err;
		return ISchedulePtr(nullptr);
	}

	ISchedulePtr SpecParser::Parse(const std::string & str) {
		if (str.empty()) {
			return returnNullandThrow("empty string !");
		}

		if ('@' == str[0]) {
			return parseDescriptor(str);
		}
		//Split by whitespace . We require 5 or 6 fields.
		// (second) (minute) (hour) (day of month) (month) (day of week, optional)
		auto fields = Fields(str);
		if (fields.size() != 5 && fields.size() != 6) {
			return returnNullandThrow(str);
		}
		// If a sixth field is not provided (DayOfWeek), then it is equivalent to star.
		if (fields.size() == 5) {
			fields.push_back("*");
		}

		auto ret = std::make_shared<SpecSchedule>();
		try {
			ret->m_dow = dow.GetField(fields[5]);
			ret->m_mon = months.GetField(fields[4]);
			ret->m_dom = dom.GetField(fields[3]);
			ret->m_hour = hours.GetField(fields[2]);
			ret->m_min = minutes.GetField(fields[1]);
			ret->m_sec = seconds.GetField(fields[0]);
		}
		catch (CronException e) {
			return returnNullandThrow(str);
		}
		catch (...) {
			return returnNullandThrow(str + " --> Unkown exception");
		}
		return ret;
	}

}
