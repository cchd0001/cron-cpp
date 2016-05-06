#include "cron.h"
#include "cron_boost.h"
#include "cron_parser.h"
#include <algorithm>

namespace Cron {
    
    /******************************************************
     *  
     * Cron public interface
     *
     ******************************************************/

    Cron::Cron(IOService & s ,  CronLogger l) 
        : m_service(s) 
        , m_timer(s)
        , m_logger(l) 
        , m_running(false) 
        , m_index(1)
    {} 

    void Cron::Start() {
        std::lock_guard<std::mutex> l(m_mutex);
        m_running = true ;
        for( auto & i : m_lists ){
            i.Next(Now());
        }
        run_nolock() ;
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
				//run_empty();
				return;
			}
			sort_nolock();
			if (m_lists.begin()->next == INVALID_TIMEPOINT) { // DELETE INVALID ENTRY
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
	}

    void Cron::run_currjobs() {
		if (m_lists.empty()) {
			return;
		}
		auto now = Now();
		auto itr = m_lists.begin();
		do {
			if (itr->next != INVALID_TIMEPOINT && itr->next > now) {
				break;
			}
			runjob_nolock(itr->job , itr->id);
			itr->Next(now);
			itr++;
		} while (itr != m_lists.end());
    }

	void Cron::runjob_nolock(Job  j , int id) {
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
}
