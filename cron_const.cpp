#include "cron_const.h"
#include "cron_boost.h"
namespace Cron {
    TimePoint CronConst::Next( const TimePoint & now ) const {
        return now + m_duration ;
    }

    ISchedulePtr Every( const std::string & duration ) {
        Duration d(0,0,0);
        std::stringstream ss(duration);
        ss >> d ;
        if ( d == Duration(0,0,0) ) {
            return nullptr;
        }
        return ISchedulePtr(new CronConst(d)) ;
    }
}
