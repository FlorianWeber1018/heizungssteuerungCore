#ifndef clock__h
#define clock__h
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
namespace Clock{

enum trigger{noTrigger, A_trigger, B_trigger};

class Clock
{
public:
    Clock(std::chrono::milliseconds _T, std::function<void(void)> callback);
    void start();
    void stop();
protected:
    void threadloopA();
    void threadloopB();

    std::chrono::milliseconds T;

    std::mutex m_triggerMutex;
    volatile trigger m_trigger = noTrigger;
    trigger getTrigger();
    void setTrigger(const trigger& newTrigger);
    void setTriggerConditionary(const trigger& newTrigger, const trigger& waitTrigger);

    std::mutex m_CallbackMutex;
    std::function<void(void)> m_callback;
private:

};

}
#endif
