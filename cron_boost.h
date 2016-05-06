#ifndef __CRON_CRON_BOOST_H__
#define __CRON_CRON_BOOST_H__

#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/asio.hpp>

namespace Cron {

    typedef boost::posix_time::ptime TimePoint;

    typedef boost::asio::io_service IOService;

    typedef boost::asio::deadline_timer Timer;
    
    inline TimePoint getTime(const std::string & value){
        return TimePoint(boost::posix_time::time_from_string(value));
    }

    inline TimePoint Now() {
        return boost::posix_time::second_clock::local_time();
    }
}

#define CRON_HOURS(x)        ( boost::posix_time::hours((x)) )

#define CRON_DAY(x)         CRON_HOURS( 24 * (x) )

#define CRON_MINUTES(x)     ( boost::posix_time::minutes((x)) )

#define CRON_SECONDS(x)     ( boost::posix_time::seconds((x)) ) 

// TimePoint to std::tm
#define CRON_TO_TM(x)       ( boost::posix_time::to_tm(x) )
// timestamp to TimePoint
#define CRON_FROM_TIME_T(x) ( boost::posix_time::from_time_t(x) )
// std::tm to TimePoint
#define CRON_FROM_TM(x)     ( boost::posix_time::ptime_from_tm(x) ) 

#define INVALID_TIMEPOINT   CRON_FROM_TIME_T(std::time_t(0))

#define TIMEPOINT_TO_STRING(x) (boost::posix_time::to_iso_string(x))
#endif // __CRON_CRON_BOOST_H__
