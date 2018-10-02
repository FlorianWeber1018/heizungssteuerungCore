#ifndef clock__h
#define clock__h
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
namespace Clock{
class Clock
{
public:
  Clock(std::chrono::milliseconds T, std::function<void(void)> callback);

  void runAsync();
  void stop();
  void setTimebase(std::chrono::milliseconds T);
protected:
  void run();

  unsigned int callStackCnt = 0;
  unsigned int callStackCntMax = 3;

  void incCallStackCnt();
  void decCallStackCnt();
  void spawnNewThread();
  std::mutex m_mutex;
  bool m_run;
  std::chrono::milliseconds T;
  std::function<void(void)> m_callback;

private:

};

}
#endif
