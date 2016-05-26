#ifndef __CRON_CRON_H__
#define __CRON_CRON_H__

#include <functional>
#include <list>
#include <mutex>
#include <memory>
#include "cron_boost.h"

namespace Cron{

    class ISchedule  {
        public:

            virtual ~ISchedule() {}

            virtual TimePoint Next(const TimePoint & now) const = 0 ; 
    };

    typedef std::shared_ptr<ISchedule>  ISchedulePtr;

    class IParser {
        public:
            virtual ISchedulePtr Parse(const std::string & str) = 0 ;
    };

    typedef std::function<void()> Job;
    
    
    // A Schedule Job Entry 
    struct Entry {

        int id;

        ISchedulePtr schedule;

        TimePoint next;

        TimePoint prev;

        Job job;

		bool Valid() const { return nullptr != schedule ; }

		void Next(const TimePoint now) {
			if (Valid()) {
				next = schedule->Next(now);
			}
		}
    };

    enum CronLogLevel {
        Unknow = 0 ,
        Info = 1,
        Debug = 2 ,
        Warning = 3 ,
        Error = 4 ,
        Fatel = 5
    };

    // Where Cron print it's logs
    typedef std::function<void( CronLogLevel lev ,  const std::string & string)> CronLogger;

    class Cron {
        public:

			static const int InvalidID = -1; 

            Cron(IOService & s ,  CronLogger l) ;

            Cron(IOService & s ,  CronLogger l , int  seconds) ;
			// Start all tasks .
            void Start();
			// Stop all tasks . Already started jobs will not effected .
            void Stop();

            int AddJob(const std::string & pattern , Job j);

            void RemoveJob(int id);

        private:

            void run_nolock() ;
            
            void start_nolock() ;

            void stop_nolock() ;
            // Sort task list
            void sort_nolock();
			// Run on time jobs .
            void run_currjobs();
			// Post a job . Will catch all exception and report it. 
            void runjob_nolock(Job j , int id);

            void start_clock_check_nolock() ;

            void stop_clock_check_nolock();

            std::mutex m_mutex;

            std::vector<Entry> m_lists;

            IOService & m_service;

            Timer m_timer;

            CpuTimer m_clock_checker;

            CronLogger m_logger;

            TimePoint m_clock_time;

            bool m_running;

            int m_index ;

            int m_check_seconds;
    };

} //namespace Cron 


#endif //__CRON_CRON_H__
