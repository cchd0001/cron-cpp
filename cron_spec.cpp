#include "cron_spec.h"
#include "cron_err.h"
namespace Cron{

    bool SpecSchedule::DayMatch(bool dom , bool dow)const {
        if ( (m_dom & AnyMark) || ( m_dow & AnyMark ) ) {
            return dom && dow ;
        } else {
            return dom || dow ;
        }
    }
    
    bool SpecSchedule::operator == (const SpecSchedule & another )const {
        if( this == &another ) {
            return true;
        } else {
            return m_dow == another.m_dow   && m_dom == another.m_dom && m_mon == another.m_mon 
                && m_hour == another.m_hour && m_min == another.m_min && m_sec == another.m_sec ;
        }
    }
    TimePoint SpecSchedule::Next(const TimePoint &now) const{

        TimePoint next = now + CRON_SECONDS(1);

        int year_limit = CRON_TO_TM(next).tm_year + 5 ;
        bool added = false ;
WRAP:

        if (CRON_TO_TM(next).tm_year > year_limit ) {
			return INVALID_TIMEPOINT;
        }

        // Month
        for( ; ; ){
            auto ctime = CRON_TO_TM(next);
            // month start from 0 
            if( Match(ctime.tm_mon +1  , m_mon) ) {
                break ;
            }
            if(!added) {
                ctime.tm_mday = 1;
                ctime.tm_hour = 0 ;
                ctime.tm_min = 0;
                ctime.tm_sec = 0 ;
                next = CRON_FROM_TM(ctime);
                added = true ;
            }
            int prev_month = ctime.tm_mon;
            next += CRON_DAY( 28 ) ;
            for( ; ; ) {
                if( CRON_TO_TM(next).tm_mon  != prev_month ) {
                    break;
                }
                next += CRON_DAY(1) ;
            }
            ctime = CRON_TO_TM(next);
            if( ctime.tm_mon == 0 ) {
                goto WRAP;
            }
        }
        //Day
        for( ; ; ){
            auto ctime = CRON_TO_TM(next);
            if( DayMatch(Match(ctime.tm_mday , m_dom) , Match(ctime.tm_wday , m_dow)) ) {
                break ;
            }
            if(!added) {
                ctime.tm_hour = 0 ;
                ctime.tm_min = 0;
                ctime.tm_sec = 0 ;
                next = CRON_FROM_TM(ctime);
                added = true ;
            }
            next += CRON_DAY(1);

            ctime = CRON_TO_TM(next);
            if( ctime.tm_mday == 1 ) {
                goto WRAP;
            }
        }
        // Hours
        for( ; ; ){
            auto ctime = CRON_TO_TM(next);
            if( Match(ctime.tm_hour , m_hour) ) {
                break ;
            }
            if(!added) {
                ctime.tm_min = 0;
                ctime.tm_sec = 0 ;
                next = CRON_FROM_TM(ctime);
                added = true ;
            }
            next += CRON_HOURS(1) ;

            ctime = CRON_TO_TM(next);
            if( ctime.tm_hour == 0 ) {
                goto WRAP;
            }
        }
        // Minutes
        for( ; ; ){
            auto ctime = CRON_TO_TM(next);
            if( Match(ctime.tm_min , m_min) ) {
                break ;
            }
            if(!added) {
                ctime.tm_sec = 0 ;
                next = CRON_FROM_TM(ctime);
                added = true ;
            }
            next += CRON_MINUTES(1) ;

            ctime = CRON_TO_TM(next);
            if( ctime.tm_min == 0 ) {
                goto WRAP;
            }
        }

        // Seconds
        for( ; ; ){
            auto ctime = CRON_TO_TM(next);
            if( Match(ctime.tm_sec , m_sec) ) {
                break ;
            }
            if(!added) {
                added = true ;
            }
            next += CRON_SECONDS(1) ;

            ctime = CRON_TO_TM(next);
            if( ctime.tm_sec == 0 ) {
                goto WRAP;
            }
        }
        return next;
    }
}
