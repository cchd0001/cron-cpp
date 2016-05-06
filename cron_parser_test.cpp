#include "UnitTest++/UnitTestPP.h"
#define CRON_UNITTEST_ON 1
#include "cron_parser.h" 

SUITE(CronTest) {
    
    typedef Cron::bounds Bounds;  

    TEST_FIXTURE(Bounds , Test_getRange) {
        struct {
            std::string expr;
            int min;
            int max;
            Cron::TimeItem expected;
        }tests[] = {
            {"5", 0, 7, 1 << 5},
            {"0", 0, 7, 1 << 0},
            {"7", 0, 7, 1 << 7},

            {"5-5", 0, 7, 1 << 5},
            {"5-6", 0, 7, 1<<5 | 1<<6},
            {"5-7", 0, 7, 1<<5 | 1<<6 | 1<<7},

            {"5-6/2", 0, 7, 1 << 5},
            {"5-7/2", 0, 7, 1<<5 | 1<<7},
            {"5-7/1", 0, 7, 1<<5 | 1<<6 | 1<<7},

            {"*", 1, 3, 1<<1 | 1<<2 | 1<<3 | Cron::SpecSchedule::AnyMark},
            {"*/2", 1, 3, 1<<1 | 1<<3 | Cron::SpecSchedule::AnyMark},
        };
        names.clear();
        for( size_t i = 0 ; i < sizeof(tests) / sizeof(tests[0]) ; i++ ){
            min = tests[i].min;
            max = tests[i].max;
            auto actual = getRange(tests[i].expr);
            CHECK_EQUAL(tests[i].expected , actual );
        }
    }
    
    TEST_FIXTURE(Bounds , GetField){
        struct {
            std::string expr;
            int min;
            int max;
            Cron::TimeItem expected;
        }tests[] = {
            {"5", 1, 7, 1 << 5},
            {"5,6", 1, 7, 1<<5 | 1<<6},
            {"5,6,7", 1, 7, 1<<5 | 1<<6 | 1<<7},
            {"1,5-7/2,3", 1, 7, 1<<1 | 1<<5 | 1<<7 | 1<<3},
        };
        names.clear();
        for( size_t i = 0 ; i < sizeof(tests) / sizeof(tests[0]) ; i++ ){
            min = tests[i].min;
            max = tests[i].max;
            auto actual = GetField(tests[i].expr);
            CHECK_EQUAL(tests[i].expected , actual );
        }
    }
    TEST_FIXTURE(Bounds , getBits) {
        struct {
            Bounds r;
            Cron::TimeItem expected;
        }tests[]{
            {Cron::minutes,   0x8fffffffffffffff},            // top1 +  0-59: 60 ones
                {Cron::hours, 0x8000000000ffffff},            // top1 +  0-23: 24 ones
                {Cron::dom,   0x80000000fffffffe},            // top1 +  1-31: 31 ones, 1 zero
                {Cron::months,0x8000000000001ffe},            // top1 +  1-12: 12 ones, 1 zero
                {Cron::dow,   0x800000000000007f},            // top1 +  0-6: 7 ones
        };
        for( size_t i = 0 ; i < sizeof(tests) / sizeof(tests[0]) ; i++ ){
            min = tests[i].r.min;
            max = tests[i].r.max;
            names = tests[i].r.names;
            CHECK_EQUAL( tests[i].expected ,  All());
        }

        struct {
            int min, max, step ;
            Cron::TimeItem expected ;
        }tests1[]{
            {0, 0, 1, 0x1},
                {1, 1, 1, 0x2},
                {1, 5, 2, 0x2a}, // 101010
                {1, 4, 2, 0xa},  // 1010
        };

        min = 0 ;
        max = 63;
        for( size_t i = 0 ; i < sizeof(tests1) / sizeof(tests1[0]) ; i++ ){
            auto actual = getBits(tests1[i].min , tests1[i].max , tests1[i].step ) ;
            CHECK_EQUAL(tests1[i].expected, actual);
        }
    }
    typedef Cron::SpecParser SP;
    TEST_FIXTURE(SP,TestParse){

            std::string str("* 5 * * * *");
            Cron::SpecSchedule expected ;
             
            expected.m_dow = Cron::dow.All();
            expected.m_mon = Cron::months.All();
            expected.m_dom = Cron::dom.All();
            expected.m_hour = Cron::hours.All();
            expected.m_min = 1ull << 5 ;
            expected.m_sec = Cron::seconds.All() ;
            auto real = std::dynamic_pointer_cast<Cron::SpecSchedule>(Parse(str));   
            CHECK( *real == expected) ;
    }
    
}
