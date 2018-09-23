
#include "IoD/IoD.h"
#include "IoD/COMprotocol.h"
#include <iostream>
#include "REST/REST.h"
#include "ModuleFramework/module.h"
#include "mysqlcon.h"

IoD::IoD globalIoD(true,"/dev/ttyACM0",57600,"localhost",3306,"IoD","637013","heating");
mSQL::mysqlcon globalSQLCon;

int mainModule(int argc, char* argv[]){
    std::thread t(REST::restMain);//start REST ASYNC
    t.detach();
  std::this_thread::sleep_for(std::chrono::seconds(100));
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
