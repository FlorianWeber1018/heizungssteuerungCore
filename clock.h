#ifndef clock__h
#define clock__h
#include <chrono>
#include <mutex>
#include <thread>
#include <functional>
namespace Clock{
class Clock
{
public:
  Clock(std::chrono::milliseconds T, std::function<void(void)> callback);
  void run();
  void stop();
  void setTimebase(std::chrono::milliseconds T);
protected:
  void spawnNewThread();
  std::mutex m_mutex;
  bool m_run;
  std::chrono::milliseconds T;
  std::function<void(void)> m_callback;
private:

};

}
#endif
