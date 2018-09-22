#ifndef timer__h
#define timer__h
#include <chrono>
class Timer {
public:
Timer();
bool getAlarmState();
void reload(unsigned int seconds);
unsigned int getSecondsToAlarm();
protected:
std::chrono::system_clock::time_point AlarmTime;
};
#endif
