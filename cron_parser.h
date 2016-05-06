#ifndef __CRON_CRON_PARSER_H__
#define __CRON_CRON_PARSER_H__

#include "cron.h"
#include "cron_spec.h"

namespace Cron {
    
    class SpecParser : public IParser {
        public:
            ISchedulePtr Parse(const std::string & str) override;
            
        private:
            ISchedulePtr parseDescriptor(const std::string & str);

            ISchedulePtr returnNullandThrow(const std::string & str);
    };
    

    struct bounds {
        public:
            
            int min ;

            int max ;
            
            std::map<std::string,int> names ;

            TimeItem GetField(const std::string & str) const ; 

            TimeItem All() const  {
                return getBits(min,max,1) | SpecSchedule::AnyMark ;
            }

        protected:

            TimeItem getRange(const std::string & str) const ;

            int parseIntOrName(const std::string & str) const ;

            int mustParseInt(const std::string & str) const ;

            TimeItem getBits( int start , int end , int step )const ;
    };

    extern const bounds seconds ;
     
    extern const bounds minutes;

    extern const bounds hours;

    extern const bounds months;

    extern const bounds dom;

    extern const bounds dow;
}

#endif //__CRON_CRON_PARSER_H__
