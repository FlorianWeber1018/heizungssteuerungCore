#ifndef clock__h
#define clock__h
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
namespace Clock{

enum trigger{A_trigger, B_trigger};

class Clock
{
public:
    Clock(std::chrono::milliseconds _T, std::function<void(void)> callback);
    void start();
    void stop();
protected:
    void threadloopA();
    void threadloopB();

    volatile trigger m_trigger = A_trigger;

    std::mutex m_CallbackMutex;
    std::chrono::milliseconds T;
    std::function<void(void)> m_callback;
    volatile bool m_run = false;
private:

};

}
#endif
