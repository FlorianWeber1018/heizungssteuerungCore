#ifndef control__h
#define control__h

#include <climits>
#include "../util.h"


struct config_PID
{
  void syncParam(int kp, int up_max, int up_min, int ki, int ui_max, int ui_min, int kd, int ud_max, int ud_min);
  float kp        = 3000;
    float up_max  = INT_MAX;
    float up_min  = INT_MIN;
  float ki        = 0;
    float ui_max  = INT_MAX;
    float ui_min  = INT_MIN;
  float kd        = 0;
    float ud_max  = INT_MAX;
    float ud_min  = INT_MIN;
};

class PID
{
public:
  float getOutput(float input, float w);
  float getUp();
  float getUi();
  float getUd();
  config_PID config;
protected:
  float up=0;
  float ui=0;
  float ud=0;
  bool firstRun = true;

  float pre_e = 0;
private:
};

struct config_ServoPWM
{
  void syncParam(int input_max, int input_min, int incPWM_max, int incPWM_min, int decPWM_max, int decPWM_min);
  int input_max  = 30000;
  int input_min  = -30000;
  float incPWM_max = 150;
  float incPWM_min = 30;
  float decPWM_max = 150;
  float decPWM_min = 30;
};

class ServoPWM
{
public:
  void getOutput(int& PWMinc, int& PWMdec, int input);
  config_ServoPWM config;
protected:

private:

};

#endif
