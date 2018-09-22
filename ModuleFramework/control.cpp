#include "control.h"
#include "util.h"
#include "paramRouter.h"

extern ParamRouter* globalParams;

void config_PID::syncParam(unsigned int ID){
  mySqlParam tempParamKey; // create Key to Config Param
  tempParamKey.ID = ID;
  tempParamKey.paramKey = "kp";
  this->kp = static_cast<float>(globalParams->getParam(tempParamKey));
  tempParamKey.paramKey = "up_max";
  this->up_max = static_cast<float>(globalParams->getParam(tempParamKey));
  tempParamKey.paramKey = "up_min";
  this->up_min = static_cast<float>(globalParams->getParam(tempParamKey));
  tempParamKey.paramKey = "ki";
  this->ki = static_cast<float>(globalParams->getParam(tempParamKey));
  tempParamKey.paramKey = "ui_max";
  this->ui_max = static_cast<float>(globalParams->getParam(tempParamKey));
  tempParamKey.paramKey = "ui_min";
  this->ui_min = static_cast<float>(globalParams->getParam(tempParamKey));
  tempParamKey.paramKey = "kd";
  this->kd = static_cast<float>(globalParams->getParam(tempParamKey));
  tempParamKey.paramKey = "ud_max";
  this->ud_max = static_cast<float>(globalParams->getParam(tempParamKey));
  tempParamKey.paramKey = "ud_min";
  this->ud_min = static_cast<float>(globalParams->getParam(tempParamKey));
}
void config_ServoPWM::syncParam(unsigned int ID){
  mySqlParam tempParamKey; // create Key to Config Param
  tempParamKey.ID = ID;
  tempParamKey.paramKey = "input_max";
  this->input_max = globalParams->getParam(tempParamKey);
  tempParamKey.paramKey = "input_min";
  this->input_min = globalParams->getParam(tempParamKey);
  tempParamKey.paramKey = "incPWM_max";
  this->incPWM_max = static_cast<float>(globalParams->getParam(tempParamKey));
  tempParamKey.paramKey = "incPWM_min";
  this->incPWM_min = static_cast<float>(globalParams->getParam(tempParamKey));
  tempParamKey.paramKey = "decPWM_max";
  this->decPWM_max = static_cast<float>(globalParams->getParam(tempParamKey));
  tempParamKey.paramKey = "decPWM_min";
  this->decPWM_min = static_cast<float>(globalParams->getParam(tempParamKey));
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


  moveToBorders(up, config.up_min, config.up_max);
  moveToBorders(ui, config.ui_min, config.ui_max);
  moveToBorders(ud, config.ud_min, config.ud_max);

  float y = up + ui + ud;

  pre_e = e;

  return y;
}

void ServoPWM::getOutput(int& PWMinc, int& PWMdec, int input)
{
//  std::cout << "servo input = " << input << std::endl;
  moveToBorders(input, config.input_min, config.input_max);

  if(input > config.input_max){
    input = config.input_max;
  }else if(input < config.input_min){
    input = config.input_min;
  }

  float inc_m = config.incPWM_max / config.input_max;
  float dec_m = config.decPWM_max / config.input_min;

  float pwminc = inc_m * static_cast<float>(input);
  float pwmdec = dec_m * static_cast<float>(input);

  moveToBorders(
    pwminc, config.incPWM_min, config.incPWM_max, 0.0f, config.incPWM_max);
  moveToBorders(
    pwmdec, config.decPWM_min, config.decPWM_max, 0.0f, config.decPWM_max);

  PWMinc = static_cast<int>(pwminc);
  PWMdec = static_cast<int>(pwmdec);
  if(PWMinc > 0 && PWMdec > 0){
    PWMinc = 0;
    PWMdec = 0;
  }
}
