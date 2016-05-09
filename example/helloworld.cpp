#include "cron.h"
#include <thread>
#include <iostream>
int main() {
    Cron::IOService ios;
    Cron::Cron cron(ios, [](Cron::CronLogLevel lev , const std::string & log) {
      std::cout<<"Log lev : "<<lev<<" -- log message : "<<log<<std::endl;
    });
    cron.AddJob("* * * * *" , []() {
      std::cout<<"Hello worLd"<<std::endl;
    });
    cron.Start();
    ios.run();
    return 0 ;
}
