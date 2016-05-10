#include "cron_boost.h"
#include "cron.h"

namespace Cron {
    class CronConst : public ISchedule {
        public:
            
            CronConst( const Duration & d ) : m_duration(d) {} 
            
            virtual TimePoint Next(const TimePoint & now) const ;

        private:
            const Duration m_duration;;
    };

    ISchedulePtr Every( const std::string & duration ) ;
}
