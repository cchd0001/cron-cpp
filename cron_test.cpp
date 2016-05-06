#include "cron.h"
#include "UnitTest++/UnitTestPP.h"
#include <iostream>
#include <thread>
SUITE(CronTest) {
    
    struct CronTestMgr {
        Cron::IOService ios;
        Cron::Timer     timer;
        boost::asio::io_service::work work;
        CronTestMgr() : timer(ios) , work(ios) {} 
    };

    // Test Start and Stop for zero tasks .
    TEST_FIXTURE(CronTestMgr, EmptyStartStop) {
        Cron::Cron cron(ios 
          , []( Cron::CronLogLevel lev ,  const std::string & string){
                std::cout<<" LEV0 :"<<lev<<" LOG : "<<string<<std::endl;        
                });

        std::thread t([this](){
                ios.run();
                });
        cron.Start() ;
        cron.Stop() ;
        ios.stop();
        t.join();
    }

    // Test Add a job before start and run it.
    TEST_FIXTURE(CronTestMgr, AddJobBeforeStartAndRun) {
        Cron::Cron cron(ios 
          , []( Cron::CronLogLevel lev ,  const std::string & string){
                std::cout<<" LEV1 :"<<lev<<" LOG : "<<string<<std::endl;        
                });
        
        int i = 0 ;
        cron.AddJob( "* * * * * ?" , [&i]() {
                i++ ;
                });
        std::thread t([this](){
                ios.run();
                });
        cron.Start() ;
        timer.expires_from_now(CRON_SECONDS(2));
        timer.wait() ;/*[this , &cron](const boost::system::error_code & e){*/
        cron.Stop() ;
        ios.stop();
        CHECK( 1 == i || i == 2) ; // timer may expired before second execute.
        t.join();
    }

    // Test Add a job after start and run it.
    TEST_FIXTURE(CronTestMgr, AddJobAfterStartAndRun) {
        Cron::Cron cron(ios 
          , []( Cron::CronLogLevel lev ,  const std::string & string){
                std::cout<<" LEV2 :"<<lev<<" LOG : "<<string<<std::endl;        
                });
        
        int i = 0 ;
        cron.AddJob( "* * * * * ?" , [&i]() {
                i++ ;
                });
        std::thread t([this](){
                ios.run();
                });
        cron.Start() ;
        timer.expires_from_now(CRON_SECONDS(2));
        cron.AddJob( "* * * * * ?" , [&i]() {
                i++ ;
                });
        timer.wait() ;/*[this , &cron](const boost::system::error_code & e){*/
        cron.Stop() ;
        ios.stop();
        CHECK( 2 == i || 4 == i ) ;
        t.join();
    }
    // Test remove a job after running .
    TEST_FIXTURE(CronTestMgr, RemoveJobAfterRunning) {
        Cron::Cron cron(ios 
          , []( Cron::CronLogLevel lev ,  const std::string & string){
                std::cout<<" LEV3 :"<<lev<<" LOG : "<<string<<std::endl;        
                });
        
        int i = 0 ;
        int task = cron.AddJob( "*/2 * * * * ?" , [&i]() {
                i++ ;
                });
        std::thread t([this](){
                ios.run();
                });
        cron.Start() ;
        timer.expires_from_now(CRON_SECONDS(4));
        timer.wait() ;
        cron.RemoveJob(task);
        timer.expires_from_now(CRON_SECONDS(2));
        timer.wait() ;
        cron.Stop() ;
        ios.stop();
        CHECK_EQUAL( 2, i ) ;
        t.join();
    }
    // Test job throw exception after running .
    TEST_FIXTURE(CronTestMgr, RunThrowJob) {
        Cron::Cron cron(ios 
          , []( Cron::CronLogLevel lev ,  const std::string & string){
                std::cout<<" LEV4 :"<<lev<<" LOG : "<<string<<std::endl;        
                });
        
        int i = 0 ;
        cron.AddJob( "* * * * * ?" , [&i]() {
                throw 1;
                });
        std::thread t([this](){
                ios.run();
                });
        cron.Start() ;
        timer.expires_from_now(CRON_SECONDS(2));
        timer.wait() ;
        cron.Stop() ;
        ios.stop();
        t.join();
    }
}
