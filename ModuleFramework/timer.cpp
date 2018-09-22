#include <chrono>
#include "timer.h"
Timer::Timer()
{
	AlarmTime = std::chrono::system_clock::now();
}
bool Timer::getAlarmState()
{
	return std::chrono::system_clock::now() > AlarmTime;
}
void Timer::reload(unsigned int seconds)
{
	AlarmTime = std::chrono::system_clock::now() + std::chrono::seconds(seconds);
}
unsigned int Timer::getSecondsToAlarm()
{
	return std::chrono::duration_cast<std::chrono::seconds>(AlarmTime - std::chrono::system_clock::now()).count();
}
