#include "../cron_const.h"
#include <string>
#include <UnitTest++/UnitTestPP.h>

SUITE(CronTest) {

    TEST(CronConstNext){
        struct {
            std::string time;
            std::string delay;
            std::string expected;
        } tests[] = {
            {"2012-Jul-09 14:45:00", "00:15:00", "2012-Jul-09 15:00:00"},
            {"2012-Jul-09 14:59:00", "00:15:00", "2012-Jul-09 15:14:00"},
            {"2012-Jul-09 14:59:59", "00:15:00", "2012-Jul-09 15:14:59"},

            // Wrap around hours
            {"2012-Jul-09 15:45:00", "00:35:00", "2012-Jul-09 16:20:00"},

            // Wrap around days
            {"2012-Jul-09 23:46:00", "00:14:00", "2012-Jul-10 00:00:00"},
            {"2012-Jul-09 23:45:00", "00:35:00", "2012-Jul-10 00:20:00"},
            {"2012-Jul-09 23:35:51", "00:44:24", "2012-Jul-10 00:20:15"},
            {"2012-Jul-09 23:35:51", "25:44:24", "2012-Jul-11 01:20:15"},

            // Wrap around months
            {"2012-Jul-09 23:35:00", "2184:25:00", "2012-Oct-09 00:00:00"},

            // Wrap around minute, hour, day, month, and year
            {"2012-Dec-31 23:59:45","00:00:15", "2013-Jan-01 00:00:00"},

            // Round to nearest second on the delay
            {"2012-Jul-09 14:45:00", "00:15:00" ,"2012-Jul-09 15:00"},

            // Round up to 1 second if the duration is less.
            {"2012-Jul-09 14:45:00", "00:00:01", "2012-Jul-09 14:45:01"},

        };
        for( size_t i = 0 ; i < sizeof(tests) / sizeof(tests[0]) ; i ++ ) {
            auto scedule = Cron::Every(tests[i].delay) ;
            if( nullptr == scedule ) {
                CHECK( 0 && "got nullptr as scedule");
                continue;
            }
            CHECK_EQUAL( Cron::getTime(tests[i].expected) , scedule->Next(Cron::getTime(tests[i].time)) ) ;
        }
    }
    
}
