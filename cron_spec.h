#ifndef __CRON_CRON_SPEC_H__
#define __CRON_CRON_SPEC_H__

#include "cron.h"
#include "cron_boost.h"

namespace Cron {
    class SpecParser ;

    typedef unsigned long long TimeItem;
    
    struct SpecSchedule : public ISchedule {
        public:

            static const TimeItem AnyMark = (1ULL<<63) ;

            TimePoint Next(const TimePoint & now) const override ;    

            friend class SpecParser;

            bool operator == (const SpecSchedule & another ) const  ;
#if CRON_UNITTEST_ON 
        public:
#else
        protected:
#endif

            TimeItem m_dow;  //Day of week

            TimeItem m_mon;  //Month

            TimeItem m_dom;  //Day of month

            TimeItem m_hour; //Hour

            TimeItem m_min;  //Minuter

            TimeItem m_sec;  //Second

            bool inline Match(int a , TimeItem b)const { return bool( (1ull <<a) & b); }
            
            bool DayMatch(bool dom , bool dow) const ;

    };

} //namespace Cron

#endif //__CRON_CRON_SPEC_H__
