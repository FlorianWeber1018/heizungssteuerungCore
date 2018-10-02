#include "clock.h"
#include <functional>
#include <iostream>
namespace Clock{
Clock::Clock(std::chrono::milliseconds T, std::function<void(void)> callback)
{
  this->T = T;
  this->m_callback=callback;
  m_run = true;
}

void Clock::run()
{
  std::this_thread::sleep_for(T);
  if(m_run){
    spawnNewThread();
  }
  m_mutex.lock();
  std::lock_guard<std::mutex> lg(m_mutex, std::adopt_lock);
  m_callback();
  decCallStackCnt();
}

void Clock::runAsync()
{
    this->spawnNewThread();
}

void Clock::stop(){
  bool run=false;
}

void Clock::setTimebase(std::chrono::milliseconds T)
{
  this->T = T;
}

void Clock::spawnNewThread(){

    incCallStackCnt();
    std::thread t(&Clock::run, this);
    t.detach();
}

void Clock::incCallStackCnt()
{
    while(callStackCnt >= callStackCntMax){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    callStackCnt++;
}
void Clock::decCallStackCnt()
{
    if(callStackCnt > 0){
        callStackCnt--;
    }else{
        std::cout << "ERROR Clock::decCallStackCnt() ... callstack ! > 0" << std::endl;
    }
}
}
