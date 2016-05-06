#include "cron_err.h"

namespace Cron {
    const CronException InvalidTime { 
        1,
        std::string("Invalid Time -- ")
    };

    const CronException InvalidExpression { 
        2,
        std::string("Invalid Expression -- ")
    };
}
