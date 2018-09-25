#include "control.h"
#include "../util.h"


void config_PID::syncParam(int kp, int up_max, int up_min, int ki, int ui_max, int ui_min, int kd, int ud_max, int ud_min)
{
  this->kp = static_cast<float>(kp);
  this->up_max = static_cast<float>(up_max);
  this->up_min = static_cast<float>(up_min);
  this->ki = static_cast<float>(ki);
  this->ui_max = static_cast<float>(ui_max);
  this->ui_min = static_cast<float>(ui_min);
  this->kd = static_cast<float>(kd);
  this->ud_max = static_cast<float>(ud_max);
  this->ud_min = static_cast<float>(ud_min);
}
void config_ServoPWM::syncParam(int input_max, int input_min, int incPWM_max, int incPWM_min, int decPWM_max, int decPWM_min)
{
  this->input_max = input_max;
  this->input_min = input_min;
  this->incPWM_max = static_cast<float>(incPWM_max);
  this->incPWM_min = static_cast<float>(incPWM_min);
  this->decPWM_max = static_cast<float>(decPWM_max);
  this->decPWM_min = static_cast<float>(decPWM_min);
}
float PID::getOutput(float x, float w)
{
  float e = w - x;

  if(firstRun){
    firstRun=false;
  }else{

    up = e * config.kp;

    ui += e * config.ki;

    ud = (e - pre_e) * config.kd;
  }


  util::moveToBorders(up, config.up_min, config.up_max);
  util::moveToBorders(ui, config.ui_min, config.ui_max);
  util::moveToBorders(ud, config.ud_min, config.ud_max);

  float y = up + ui + ud;

  pre_e = e;

  return y;
}

void ServoPWM::getOutput(int& PWMinc, int& PWMdec, int input)
{
//  std::cout << "servo input = " << input << std::endl;
  util::moveToBorders(input, config.input_min, config.input_max);

  if(input > config.input_max){
    input = config.input_max;
  }else if(input < config.input_min){
    input = config.input_min;
  }

  float inc_m = config.incPWM_max / config.input_max;
  float dec_m = config.decPWM_max / config.input_min;

  float pwminc = inc_m * static_cast<float>(input);
  float pwmdec = dec_m * static_cast<float>(input);

  util::moveToBorders(
    pwminc, config.incPWM_min, config.incPWM_max, 0.0f, config.incPWM_max);
  util::moveToBorders(
    pwmdec, config.decPWM_min, config.decPWM_max, 0.0f, config.decPWM_max);

  PWMinc = static_cast<int>(pwminc);
  PWMdec = static_cast<int>(pwmdec);
  if(PWMinc > 0 && PWMdec > 0){
    PWMinc = 0;
    PWMdec = 0;
  }
}
