#include "UnitTest++/UnitTestPP.h"
#define CRON_UNITTEST_ON 1
#include "cron_parser.h"
#include "cron_err.h"
#include "cron_boost.h"
#include <locale>
#include <iomanip>
#include <iostream>

SUITE(CronTest) {
    typedef Cron::SpecSchedule SS;
    TEST(TestActivation){
        struct {
            std::string time , spec ;
            bool expected ;
        } tests[] {
            // Every fifteen minutes.
            {"2012-01-09 15:00:00", "0 0/15 * * *", true},          //0
                {"2012-Jul-09 15:45:00", "0 0/15 * * *", true},     //1
                {"2012-Jul-09 15:40:00", "0 0/15 * * *", false},    //2    

                // Every fifteen minutes, starting at 5 minutes.
                {"2012-Jul-09 15:05:00", "0 5/15 * * *", true},     //3
                {"2012-Jul-09 15:20:00", "0 5/15 * * *", true},     //4
                {"2012-Jul-09 15:50:00", "0 5/15 * * *", true},     //5

                // Named months
                {"2012-Jul-15 15:00:00", "0 0/15 * * Jul", true},   //6
                {"2012-Jul-15 15:00:00", "0 0/15 * * Jun", false},  //7
                      
                // Everything set.
                {"2012-Jul-15 08:30:00", "0 30 08 ? Jul Sun", true},//8
                {"2012-Jul-15 08:30:00", "0 30 08 15 Jul ?", true}, //9
                {"2012-Jul-16 08:30:00", "0 30 08 ? Jul Sun", false},//10
                {"2012-Jul-16 08:30:00", "0 30 08 15 Jul ?", false},//11

                // Predefined schedules
                {"2012-Jul-09 15:00:00", "@hourly", true},//12
                {"2012-Jul-09 15:04:00", "@hourly", false},//13
                {"2012-Jul-09 15:00:00", "@daily", false},//14
                {"2012-Jul-09 00:00:00", "@daily", true},//15
                {"2012-Jul-09 00:00:00", "@weekly", false},//16
                {"2012-Jul-08 00:00:00", "@weekly", true},//17
                {"2012-Jul-08 01:00:00", "@weekly", false},//18
                {"2012-Jul-08 00:00:00", "@monthly", false},//19
                {"2012-Jul-01 00:00:00", "@monthly", true},//20

                // Test interaction of DOW and DOM.
		        // If both are specified, then only one needs to match.
                {"2012-Jul-15 00:00:00", "0 * * 1,15 * Sun", true}, //21
                {"2012-Jun-15 00:00:00", "0 * * 1,15 * Sun", true}, //22
                {"2012-Aug-01 00:00:00", "0 * * 1,15 * Sun", true}, //23

                // However, if one has a star, then both need to match.
                {"2012-Jul-15 00:00:00", "0 * * * * Mon", false},//24
                {"2012-Jul-15 00:00:00", "0 * * */10 * Sun", false},//25
                {"2012-Jul-09 00:00:00", "0 * * 1,15 * *", false},//26
                {"2012-Jul-15 00:00:00", "0 * * 1,15 * *", true},//27
                {"2012-Jul-15 00:00:00", "0 * * */2 * Sun", true},//28

        };
        
        Cron::SpecParser parser;
        for( size_t i = 0 ; i < sizeof(tests) / sizeof(tests[0]) ; i++ ){
            auto sched = parser.Parse(tests[i].spec);
            if(!sched){
                CHECK(0 && "got nullptr from Parse ");
                continue;
            }
            auto actual = sched->Next(Cron::getTime(tests[i].time)- CRON_SECONDS(1));
            auto expected = Cron::getTime(tests[i].time);
            //std::cout<<i<<std::endl;
            //std::cout<<boost::posix_time::to_simple_string(actual)<<std::endl;
            //std::cout<<boost::posix_time::to_simple_string(expected)<<std::endl;

            if((tests[i].expected && expected != actual) || (!tests[i].expected && expected == actual) ){
                CHECK(0 && "time check failed ");
            }   

        }
    }
    TEST(TestNext) {
        struct {
            std::string time, spec;
            std::string expected;
        }tests[]{
            // Simple cases
            {"2012-Jul-09 14:45:00", "0 0/15 * * *", "2012-Jul-09 15:00:00"},                           //0
                {"2012-Jul-09 14:59:00", "0 0/15 * * *", "2012-Jul-09 15:00:00"},
                {"2012-Jul-09 14:59:59", "0 0/15 * * *", "2012-Jul-09 15:00:00"},

                // Wrap around hours
                {"2012-Jul-09 15:45:00", "0 20-35/15 * * *", "2012-Jul-09 16:20"},                 //3

                // Wrap around days
                {"2012-Jul-09 23:46:00", "0 */15 * * *", "2012-Jul-10 00:00"},
                {"2012-Jul-09 23:45:00", "0 20-35/15 * * *", "2012-Jul-10 00:20"},
                {"2012-Jul-09 23:35:51", "15/35 20-35/15 * * *", "2012-Jul-10 00:20:15"},
                {"2012-Jul-09 23:35:51", "15/35 20-35/15 1/2 * *", "2012-Jul-10 01:20:15"},
                {"2012-Jul-09 23:35:51", "15/35 20-35/15 10-12 * *", "2012-Jul-10 10:20:15"},           //8

                {"2012-Jul-09 23:35:51", "15/35 20-35/15 1/2 */2 * *", "2012-Jul-11 01:20:15"},
                {"2012-Jul-09 23:35:51", "15/35 20-35/15 * 9-20 * *", "2012-Jul-10 00:20:15"},
                {"2012-Jul-09 23:35:51", "15/35 20-35/15 * 9-20 Jul *", "2012-Jul-10 00:20:15"},   //11
                    
                // Wrap around months
                {"2012-Jul-09 23:35:00", "0 0 0 9 Apr-Oct ?", "2012-Aug-09 00:00:00"},
                {"2012-Jul-09 23:35:00", "0 0 0 */5 Apr,Aug,Oct Mon", "2012-Aug-06 00:00:00"},
                {"2012-Jul-09 23:35:00", "0 0 0 */5 Oct Mon", "2012-Oct-01 00:00:00"},                  //14

                // Wrap around years
                {"2012-Jul-09 23:35:00", "0 0 0 * Feb Mon", "2013-Feb-04 00:00:00"},
                {"2012-Jul-09 23:35:00", "0 0 0 * Feb Mon/2", "2013-Feb-01 00:00:00"},                  //16

                // Wrap around minute, hour, day, month, and year
                {"2012-Dec-31 23:59:45", "0 * * * * *", "2013-Jan-01 00:00:00"},                        //17

                // Leap year
                {"2012-Jul-09 23:35:00", "0 0 0 29 Feb ?", "2016-Feb-29 00:00:00"},                     //18
        };
        Cron::SpecParser parser;
        for( size_t i = 0 ; i < sizeof(tests) / sizeof(tests[0]) ; i++ ){
            try {
            auto sched = parser.Parse(tests[i].spec);
            if( nullptr == sched ) {
                CHECK(0 && "Parse return nullptr");
                continue;
            }
            //std::cout<<i<<std::endl;
            auto actual = sched->Next(Cron::getTime( tests[i].time ));
            auto expected = Cron::getTime(tests[i].expected);
            CHECK_EQUAL(expected , actual );

            } catch ( Cron::CronException e ) {
                CHECK(0 && "Cron::CronException");

                std::cout<<e.What()<<std::endl;
            } catch ( ... ) {

                CHECK(0 && "unknow exception ");

            }
        }
    }

}
