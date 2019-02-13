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

    setTrigger(A_trigger);
}
void Clock::stop(){
    setTrigger(noTrigger);
}
trigger Clock::getTrigger()
{
    m_triggerMutex.lock();
    std::lock_guard<std::mutex> lg(m_triggerMutex, std::adopt_lock);
    return m_trigger;
}
void Clock::setTrigger(const trigger& newTrigger)
{
    m_triggerMutex.lock();
    std::lock_guard<std::mutex> lg(m_triggerMutex, std::adopt_lock);
    m_trigger = newTrigger;
}
void Clock::setTriggerConditionary(const trigger& newTrigger, const trigger& waitTrigger)
{
    while(getTrigger() == waitTrigger) {
        std::this_thread::sleep_for(std::chrono::microseconds(200));
    }
    setTrigger(newTrigger);
}
void Clock::threadloopA()
{
    while(1){
        while(getTrigger() != A_trigger){      //wait for trigger from B
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        std::this_thread::sleep_for(T);      //wait cycle Time

        m_CallbackMutex.lock();
        std::lock_guard<std::mutex> lg(m_CallbackMutex, std::adopt_lock);

        setTriggerConditionary(B_trigger, noTrigger);    //trigger B

        m_callback();
    }
}
void Clock::threadloopB()

{
    while(1){
        while(getTrigger() != B_trigger){      //wait for trigger from A
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        std::this_thread::sleep_for(T);      //wait cycle Time

        m_CallbackMutex.lock();
        std::lock_guard<std::mutex> lg(m_CallbackMutex, std::adopt_lock);

        setTriggerConditionary(A_trigger, noTrigger);      //trigger A

        m_callback();
    }
}
}
