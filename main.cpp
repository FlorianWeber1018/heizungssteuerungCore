
#include "IoD/IoD.h"
#include "IoD/COMprotocol.h"
#include <iostream>
#include "REST/REST.h"
#include "ModuleFramework/module.h"
#include "ModuleFramework/modulemanager.h"
#include "mysqlcon.h"

mSQL::mysqlcon globalSQLCon("localhost",3306,"IoD","637013","heating");
IoD::IoD globalIoD(true, 2000, "/dev/ttyACM0",57600);

Module::ClockDistributer globalClock;
Module::ModuleManager globalModuleManager;



int main(int argc, char* argv[]){
    std::thread t(REST::restMain);//start REST ASYNC
    t.detach();
    std::string input;
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
