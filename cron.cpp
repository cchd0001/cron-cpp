#include "cron.h"
#include "cron_boost.h"
#include "cron_parser.h"
#include <algorithm>
#include <sstream>

namespace Cron {

    /******************************************************
     *  
     * Cron public interface
     *
     ******************************************************/

    Cron::Cron(IOService & s ,  CronLogger l) : Cron(s,l,3) {}

    Cron::Cron(IOService & s ,  CronLogger l , int  seconds) 
        : m_service(s) 
          , m_timer(s)
          , m_clock_checker(s)
          , m_logger(l) 
          , m_running(false) 
          , m_index(1)
          , m_check_seconds(seconds)  
    {}

    void Cron::Start() {
        std::lock_guard<std::mutex> l(m_mutex);
        m_running = true ;
        start_nolock();
    }
    void Cron::Stop() {
        std::lock_guard<std::mutex> l(m_mutex);
        m_running = false ;
        stop_nolock() ;
    }
    void Cron::RemoveJob(int id) {
        std::lock_guard<std::mutex> l(m_mutex);
        m_lists.erase(std::remove_if(m_lists.begin(), m_lists.end(), [id](const Entry & e) {
                    return e.id == id;
                    }));
    }

    int Cron::AddJob(const std::string & pattern, Job j) {
        try {
            auto sched_ptr = SpecParser().Parse(pattern);
            if (nullptr == sched_ptr) {
                return InvalidID;
            }
            {
                std::lock_guard<std::mutex> l(m_mutex);
                Entry e{
                    m_index ,
                            sched_ptr,
                            INVALID_TIMEPOINT,
                            INVALID_TIMEPOINT,
                            j
                };
                m_index ++ ;
                if (!m_running) {
                    // Cron is not running yet . 
                    // Just insert to the end of list .
                    m_lists.push_back(e);
                    return e.id;
                }
                // Cron is running 
                e.Next(Now());
                if (e.next == INVALID_TIMEPOINT) {
                    return InvalidID;
                }
                m_lists.push_back(e);
                if (m_lists.size() == 1 || e.next < m_lists.begin()->next) {
                    // Need reset timer ;
                    sort_nolock();
                    m_logger(CronLogLevel::Info ,pattern);
                    run_nolock();
                }
                return e.id;
            }
        }
        catch (...) {
            return InvalidID;
        }
    }
    /******************************************************
     *  
     * Cron private interface
     *
     ******************************************************/
    void Cron::start_nolock() {
        for( auto & i : m_lists ){
            i.Next(Now());
        }
        run_nolock() ;
        start_clock_check_nolock();
    }
    // Sort m_lists by next timepoint .
    void Cron::sort_nolock() {
        std::sort(m_lists.begin(), m_lists.end(), [](const Entry & e1 , const Entry & e2){
                return e1.next < e2.next;
                });
    }
    // Cron main control flow .
    void Cron::run_nolock() {
        do {
            if (m_lists.empty()) {  // No task in list .
                m_logger(CronLogLevel::Info, "task  list empty");
                return;
            }
            sort_nolock();
            if (m_lists.begin()->next == INVALID_TIMEPOINT) { // DELETE INVALID ENTRY
                m_logger(CronLogLevel::Warning, "task invalid !");
                m_lists.erase(m_lists.begin());
                continue; // Redo run_nolock
            }
            m_timer.expires_from_now(m_lists.begin()->next - Now()); // Wait for nearest task .
            m_logger(CronLogLevel::Info , std::string("next activity time  : ") + TIMEPOINT_TO_STRING(m_lists.begin()->next));
            m_timer.async_wait([this](const boost::system::error_code& error) {
                    if (error) {
                    //TODO : log this err 
                    return;
                    }
                    std::lock_guard<std::mutex> l(m_mutex);
                    if (m_running) {
                    run_currjobs();
                    run_nolock();
                    }
                    }
                    );
            break; 
        } while (1);
    }
    void Cron::stop_nolock() {
        m_timer.cancel();
        stop_clock_check_nolock();
    }

    void Cron::run_currjobs() {
        if (m_lists.empty()) {
            return;
        }
        auto now = Now();
        auto itr = m_lists.begin();
        do {
            if (itr->next != INVALID_TIMEPOINT && itr->next > now) {
                m_logger(CronLogLevel::Info,TIMEPOINT_TO_STRING(itr->next));
                m_logger(CronLogLevel::Info,TIMEPOINT_TO_STRING(now));
                break;
            }
            runjob_nolock(itr->job , itr->id);
            itr->prev = now ;
            itr->Next(now);
            itr++;
        } while (itr != m_lists.end());
    }

    void Cron::runjob_nolock(Job  j , int id) {
        m_logger(CronLogLevel::Info, std::to_string(id));
        m_service.post([this,j , id]() {
                try {
                j();
                }
                catch (...) {
                std::ostringstream ost;
                ost << id << " task thrown exception !!!";
                m_logger(CronLogLevel::Error, ost.str());
                }
                });
    }

    void Cron::start_clock_check_nolock() {
        m_clock_time = Now() + CRON_SECONDS(m_check_seconds) ;
        m_clock_checker.expires_from_now(CRON_CHRONO_SECONDS(m_check_seconds));
        m_clock_checker.async_wait([this](const boost::system::error_code& error){
                if (error) {
                m_logger(CronLogLevel::Warning , " m_clock_checker timer error");
                //TODO : log this err 
                return;
                }
                std::lock_guard<std::mutex> l(m_mutex);
                if( Now() != m_clock_time ) {
                std::ostringstream ost;
                ost<<"Clock time unmatch! Should be "<<TIMEPOINT_TO_STRING(m_clock_time) << " but was  "<< TIMEPOINT_TO_STRING(Now());
                m_logger(CronLogLevel::Warning, ost.str());
                { // Reset everything 
                stop_nolock();
                start_nolock();
                }
                return ;
                } else {
                m_logger(CronLogLevel::Info, "Clock time correct" );
                start_clock_check_nolock();
                }
                });
    }
    void Cron::stop_clock_check_nolock() {
        m_clock_checker.cancel();
    }
}
