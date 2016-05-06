#ifndef __CRON_CRON_ERR_H__
#define __CRON_CRON_ERR_H__

#include <string>

namespace Cron {
    class CronException {
        public:
            std::string What() const {return what;}

            bool operator ==(const CronException & another) {
                return id == another.id ;
            }

            void Append(const std::string & str) { what  += str ; } 
            
            const int id;
            std::string what;
    };

    // Pre-define exception

    const extern CronException InvalidTime;

    const extern CronException InvalidExpression;


}

#endif //__CRON_CRON_ERR_H__
