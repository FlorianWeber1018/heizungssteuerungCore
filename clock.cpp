#include "clock.h"
#include <functional>
#include <iostream>
#include <chrono>
#include <thread>
namespace Clock{
Clock::Clock(std::chrono::milliseconds T, std::function<void(void)> callback)
{
    this->T = T;
    this->m_callback = callback;
    std::thread threadA(&Clock::threadloopA, this);
    std::thread threadB(&Clock::threadloopB, this);
    threadB.detach();
    threadA.detach();

}

void Clock::start(){
    this->m_run = true;
}
void Clock::stop(){
    this->m_run = false;
}

void Clock::threadloopA()
{
    while(1){
        while(m_trigger != A_trigger && m_run){      //wait for trigger from B
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        std::this_thread::sleep_for(T);      //wait cycle Time

        m_CallbackMutex.lock();
        std::lock_guard<std::mutex> lg(m_CallbackMutex, std::adopt_lock);

        m_trigger = B_trigger;      //trigger B

        m_callback();
    }
}
void Clock::threadloopB()
{
    while(1){
        while(m_trigger != B_trigger && m_run){      //wait for trigger from A
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        std::this_thread::sleep_for(T);      //wait cycle Time

        m_CallbackMutex.lock();
        std::lock_guard<std::mutex> lg(m_CallbackMutex, std::adopt_lock);

        m_trigger = A_trigger;      //trigger A

        m_callback();
    }
}
}
