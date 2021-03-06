#include "IoD/IoD.h"
#include "IoD/COMprotocol.h"
#include <iostream>
#include "REST/REST.h"
#include "ModuleFramework/module.h"
#include "ModuleFramework/modulemanager.h"
#include "mysqlcon.h"
#include "main.h"
#include <functional>
#include <thread>
#include "serveradress.h"
mSQL::mysqlcon globalSQLCon(serveradress,3306,"IoD","637013","heating");
IoD::IoD globalIoD(false, 500, "/dev/ttyACM1",57600);

Module::ClockDistributer globalClockDistributer;
Module::ModuleManager globalModuleManager;
Clock::Clock globalClock(std::chrono::milliseconds(500), std::bind(&mainloop));


int main(){
    std::thread t(REST::restMain);//start REST ASYNC
    t.detach();
    globalClock.start();
    while(1){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  unsigned long fc, sc, rc, cc;
  fc = globalIoD.getFlushCount();
  sc = globalIoD.getSendCount();
  rc = globalIoD.getRecCount();
  cc = globalIoD.getClockCount();
  std::cout << std::dec;
  std::cout << "flushed:    " << fc << std::endl
            << "sended:     " << sc << std::endl
            << "received:   " << rc << std::endl
            << "ClockCount: " << cc << std::endl;


  return 0;
}

void mainloop(){
    static volatile bool firstRun=true;
    if(globalIoD.getBufOutCnt() <=200){
        if(firstRun){
            firstRun=false; //inputs not yet available
        }else{
            globalClockDistributer.trigger();
            globalIoD.writeOutputs(false);
        }
        globalIoD.readInputs(false);
    }else{
        globalClock.stop();
        std::cout << "buff isn t sended completely." <<
                     " Cycle was skiped! Time: " <<
                     globalSQLCon.getTimeString() <<
                     std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        globalIoD.reconnect();
        globalClock.start();
    }
}

