#include "module.h"
#include <iostream>
//#include "paramRouter.h"
#include "timer.h"
extern Module::ClockDistributer globalClock;
extern mSQL::mysqlcon globalSQLCon;


    // ____Module___________________________________________________________________
namespace Module {


void Module::emitSignal(std::string signalName, int value) {
  Signal *signal = nullptr;
  try {
    signal = m_signals.at(signalName);
  } catch (const std::exception &e) {
    if (debugMode) {
      std::cout << "exception was cought : " << e.what() << std::endl;
    }
    return;
  }
  if (signal != nullptr) {
    if (value > signal->max) {
      signal->value = signal->max;
    } else {
      if (value < signal->min) {
        signal->value = signal->min;
      } else {
        signal->value = value;
      }
    }
    for (auto&& slot : signal->m_slots) {
      slot->synced = true;
    }
  }
}

int Module::getSignalValue(std::string slotName) {
  Slot *slot = nullptr;
  int preRet = 0;
  try {
    slot = m_slots.at(slotName);
  } catch (const std::exception &e) {
    std::cout << "exception was cought : " << e.what() << std::endl;
    return preRet;
  }
  if (slot->value == nullptr) {
    return preRet;
  }

  if (slot != nullptr) {
    preRet = *(slot->value);
    if (preRet > slot->max) {
      preRet = slot->max;
    } else {
      if (preRet < slot->min) {
        preRet = slot->min;
      }
    }
  }
  return preRet;
}

void Module::trigger() {
  bool allInputsSynced = true;
  for (auto slotName_slot : m_slots) {
    if ((!slotName_slot.second->synced) &&
        slotName_slot.second->value != nullptr) {
      allInputsSynced = false;
    }
  }
  if (allInputsSynced) {
    for (auto slotName_slot : m_slots) {
      slotName_slot.second->synced = false;
    }
    process();
    triggerNext();
  }
}

void Module::triggerNext() {
  if (debugMode)
    std::cout << "Module::triggerNext" << std::endl;
  for (auto postModule : m_postModules) {
    if (debugMode)
      std::cout << "1 Module triggered" << std::endl;
    postModule->trigger();
  }
}

void Module::process() {
  if (debugMode) {
    std::cout << "virtual Module::process():" << std::endl;
    for (auto &&keyValPair : m_signals) {
      std::cout << "    " << keyValPair.first << keyValPair.second->value
                << std::endl;
    }
  }
}

Signal *Module::createSignal(std::string signalName) {
  if (m_signals.count(signalName) == 0) {
    Signal *newSignal = new Signal();
    m_signals[signalName] = newSignal;
    return newSignal;
  } else {
    return m_signals.at(signalName);
  }
}

Slot *Module::createSlot(std::string slotName) {
  if (m_slots.count(slotName) == 0) {
    Slot *newSlot = new Slot();
    m_slots[slotName] = newSlot;
    newSlot->value = nullptr;
    return newSlot;
  } else {
    return m_slots.at(slotName);
  }
}

Signal *Module::getSignal(std::string signalName) {
  try {
    return m_signals.at(signalName);
  } catch (const std::exception &e) {
    if (debugMode) {
      std::cout << "exception was cought : " << e.what() << std::endl;
    }
    return nullptr;
  }
}
void Module::addPostModule(Module *postModule) {
  m_postModules.push_back(postModule);
}

Slot *Module::getSlot(std::string slotName) {
  try {
    return m_slots.at(slotName);
  } catch (const std::exception &e) {
    if (debugMode) {
      std::cout << "exception was cought : " << e.what() << std::endl;
    }
    return nullptr;
  }
}

void Module::changeParam(const std::string& paramKey, int newParamValue)
{
    m_params[paramKey] = newParamValue;
    //PERSIST TO SERVER!!!!
}

Module::~Module() {
  if (debugMode) {
    std::cout << "debug:~Module()" << std::endl;
  }
  for (auto &&keyValPair : m_signals) {
    delete keyValPair.second;
    if (debugMode) {
      std::cout << "deleted Signal: " << keyValPair.first << std::endl;
    }
  }
  for (auto &&keyValPair : m_slots) {
    delete keyValPair.second;
    if (debugMode) {
      std::cout << "deleted Slot: " << keyValPair.first << std::endl;
    }
  }
}

Module::Module() {
  std::cout << "NEW Module" << std::endl;
}
int* Module::createParam(const std::string& paramKey, int defaultValue)
{
  getParamFromServerIfExists(paramKey, defaultValue);
  createParamOrUpdateOnServer(paramKey, defaultValue);
  m_params[paramKey] = defaultValue;
  return m_params[paramKey]&;
}
// ____ClockDistributer_________________________________________________________
void ClockDistributer::trigger() {
  for (auto &&destModule : m_destModules) {
    destModule->trigger();
  }
}
void ClockDistributer::addDestination(Module *destModule) {
  m_destModules.insert(destModule);
}
void ClockDistributer::rmDestination(Module *destModule) {
  if (m_destModules.erase(destModule) < 1) {
    if (debugMode) {
      std::cout << "ClockDistributer::rmDestination : Element to remove"
                << "is not in m_destModules" << std::endl;
    }
  }
}

// ____Module_constant__________________________________________________________

Module_constant::Module_constant(unsigned int ID){
    this->ID = ID;
    globalClock.addDestination(this);
    createSignal("constSig");

    if (this->ID == 0) {
        this->ID = getNextAVID();
    }
    createParam()
    m_params["constSig"] = 0;   //default value
    tryToGetParamFromServer("constSig"); //try to update local value with persistent Value from server(will only be done if the param exists there)
    createParamOrUpdateOnServer("constSig", m_params["constSig"]);  //MABY THE BLOCK ABOVE IN AN SEPERATE FUNCTION IN MODULE Class ?????? .. for bigger Modules....
}
Module_constant::~Module_constant() { globalClock.rmDestination(this); }

void Module_constant::process() {
  if(debugMode==4){
    std::cout << "Module_constant::process()" << std::endl;
  }
  mySqlParam tempParamKey; // create Key to Config Param
  tempParamKey.ID = this->ID;
  tempParamKey.paramKey = "constSig";
  emitSignal("constSig", globalParams->getParam(tempParamKey));
}
// ____Module_debug_____________________________________________________________

Module_debug::Module_debug(unsigned int ID) {
  this->ID = ID;
  createSlot("debugSlot");
  mySqlParam tempParamKey; // create Key to Config Param
  if (this->ID == 0) {
    this->ID = globalParams->getNextAvID();
  }
  tempParamKey.ID = this->ID;
  tempParamKey.paramKey = "identifier";
  globalParams->createParamIfNotExist(tempParamKey, 0); // Create Line on Server
}
void Module_debug::process() {
  if(debugMode==4){
    std::cout << "Module_debug::process()" << std::endl;
  }
  mySqlParam tempParamKey; // create Key to Config Param
  tempParamKey.ID = this->ID;
  tempParamKey.paramKey = "identifier";
  std::cout << "Module_debug::" << globalParams->getParam(tempParamKey) << " = "
            << getSignalValue("debugSlot") << std::endl;
}
// ____Module_3WayValve_________________________________________________________
Module_3WayValve::Module_3WayValve(unsigned int ID) {
  this->ID = ID;
  createSlot("requiredTemperature");
  createSlot("actualTemperature");
  createSlot("!EN");
  createSignal("DutyCyclePWMinc");
  createSignal("DutyCyclePWMdec");
  mySqlParam tempParamKey; // create Key to Config Param
  if (this->ID == 0) {
    this->ID = globalParams->getNextAvID();
  }
  tempParamKey.ID = this->ID;

  tempParamKey.paramKey = "kp";
  globalParams->createParamIfNotExist(tempParamKey, 3000);
  tempParamKey.paramKey = "up_max";
  globalParams->createParamIfNotExist(tempParamKey, INT_MAX);
  tempParamKey.paramKey = "up_min";
  globalParams->createParamIfNotExist(tempParamKey, INT_MIN);

  tempParamKey.paramKey = "ki";
  globalParams->createParamIfNotExist(tempParamKey, 0);
  tempParamKey.paramKey = "ui_max";
  globalParams->createParamIfNotExist(tempParamKey, INT_MAX);
  tempParamKey.paramKey = "ui_min";
  globalParams->createParamIfNotExist(tempParamKey, INT_MIN);

  tempParamKey.paramKey = "kd";
  globalParams->createParamIfNotExist(tempParamKey, 0);
  tempParamKey.paramKey = "ud_max";
  globalParams->createParamIfNotExist(tempParamKey, INT_MAX);
  tempParamKey.paramKey = "ud_min";
  globalParams->createParamIfNotExist(tempParamKey, INT_MIN);

  tempParamKey.paramKey = "input_max";
  globalParams->createParamIfNotExist(tempParamKey, 30000);
  tempParamKey.paramKey = "input_min";
  globalParams->createParamIfNotExist(tempParamKey, -30000);

  tempParamKey.paramKey = "incPWM_max";
  globalParams->createParamIfNotExist(tempParamKey, 150);
  tempParamKey.paramKey = "incPWM_min";
  globalParams->createParamIfNotExist(tempParamKey, 30);

  tempParamKey.paramKey = "decPWM_max";
  globalParams->createParamIfNotExist(tempParamKey, 150);
  tempParamKey.paramKey = "decPWM_min";
  globalParams->createParamIfNotExist(tempParamKey, 30);
}
void Module_3WayValve::process() {
  if(debugMode==4){
    std::cout << "Module_3WayValve::process()" << std::endl;
  }
  int DC_inc, DC_dec;
  if (getSignalValue("!EN")) {
    mySqlParam tempParamKey; // create Key to Config Param
    tempParamKey.ID = this->ID;
    tempParamKey.paramKey = "decPWM_max";
    DC_dec = globalParams->getParam(tempParamKey);

    DC_inc = 0;
  } else {
    pid.config.syncParam(this->ID);
    pwm.config.syncParam(this->ID);
    int y = static_cast<int>(pid.getOutput(
        static_cast<float>(getSignalValue("actualTemperature")),
        static_cast<float>(getSignalValue("requiredTemperature"))));
    pwm.getOutput(DC_inc, DC_dec, y);
  }

  emitSignal("DutyCyclePWMinc", DC_inc);
  emitSignal("DutyCyclePWMdec", DC_dec);
}
// ____Module_2Point____________________________________________________________
Module_2Point::Module_2Point(unsigned int ID) {
  this->ID = ID;
  createSlot("T1");
  createSlot("T2");
  createSignal("outState");
  mySqlParam tempParamKey; // create Key to Config Param
  if (this->ID == 0) {
    this->ID = globalParams->getNextAvID();
  }
  tempParamKey.ID = this->ID;

  tempParamKey.paramKey = "dT_on";
  globalParams->createParamIfNotExist(tempParamKey, 8);
  tempParamKey.paramKey = "dT_off";
  globalParams->createParamIfNotExist(tempParamKey, 2);
}
void Module_2Point::process() {
  if(debugMode==4){
    std::cout << "Module_2Point::process()" << std::endl;
  }
  mySqlParam tempParamKey; // create Key to Config Param
  tempParamKey.ID = this->ID;
  tempParamKey.paramKey = "dT_on";
  int dT_on = globalParams->getParam(tempParamKey);
  tempParamKey.paramKey = "dT_off";
  int dT_off = globalParams->getParam(tempParamKey);

  int diff = getSignalValue("T1") - getSignalValue("T2");
  if (debugMode)
    std::cout << "diff: " << diff << std::endl;
  if (diff >= dT_on) {
    outState = 1;
  } else if (diff <= dT_off) {
    outState = 0;
  }
  emitSignal("outState", outState);
}
// ____MODULE_Inverter__________________________________________________________
Module_Inverter::Module_Inverter(unsigned int ID) {
  this->ID = ID;
  createSlot("S");
  createSignal("S");
}
void Module_Inverter::process() {
  if(debugMode==4){
    std::cout << "Module_Inverter::process()" << std::endl;
  }
  int value = getSignalValue("S");
  if (value <= 1 && value >= 0) {
    if (value == 1) {
      value = 0;
    } else {
      value = 1;
    }
  }
  emitSignal("S", value);
}
// ____MODULE_Median____________________________________________________________
Module_MedianFilter::Module_MedianFilter(unsigned int ID) {
  this->ID = ID;
  createSlot("S");
  createSignal("S");

  mySqlParam tempParamKey; // create Key to Config Param
  if (this->ID == 0) {
    this->ID = globalParams->getNextAvID();
  }
  tempParamKey.ID = this->ID;

  tempParamKey.paramKey = "size";
  globalParams->createParamIfNotExist(tempParamKey, 3);
}
void Module_MedianFilter::process() {
  if(debugMode==4){
    std::cout << "Module_MedianFilter::process()" << std::endl;
  }
  mySqlParam tempParamKey; // create Key to Config Param
  tempParamKey.ID = this->ID;
  tempParamKey.paramKey = "size";
  int NewSize = globalParams->getParam(tempParamKey);
  int NewValue = getSignalValue("S");

  m_values.push_back(NewValue);

  while(m_values.size() > NewSize){   //remove elements till the buffer hats the correct size from the config
    m_values.erase(m_values.begin());
  }
  int value = findMedian(m_values);

  emitSignal("S", value);
}
// ____MODULE_Woodstove_________________________________________________________
Module_Woodstove::Module_Woodstove(unsigned int ID) {
  this->ID = ID;
  createSlot("T_boiler");
  createSlot("T_storage");
  createSlot("startButton");
  createSlot("stopButton");

  createSignal("loadPump");
  createSignal("fan");
  createSignal("cnt");

  mySqlParam tempParamKey; // create Key to Config Param
  if (this->ID == 0) {
    this->ID = globalParams->getNextAvID();
  }
  tempParamKey.ID = this->ID;

  tempParamKey.paramKey = "dT_on";
  globalParams->createParamIfNotExist(tempParamKey, 8);
  tempParamKey.paramKey = "dT_off";
  globalParams->createParamIfNotExist(tempParamKey, 4);
  tempParamKey.paramKey = "T_boilerMin";
  globalParams->createParamIfNotExist(tempParamKey, 55);
  tempParamKey.paramKey = "T_boilerMinHyst";
  globalParams->createParamIfNotExist(tempParamKey, 4);
  //tempParamKey.paramKey = "initCnt";
  //globalParams->createParamIfNotExist(tempParamKey, 3600);

}
void Module_Woodstove::process() {
  if(debugMode==4){
    std::cout << "Module_Woodstove::process()" << std::endl;
  }
  mySqlParam tempParamKey; // create Key to Config Param
  tempParamKey.ID = this->ID;

//________2point loadPump___
  tempParamKey.paramKey = "dT_on";
  int dT_on = globalParams->getParam(tempParamKey);
  tempParamKey.paramKey = "dT_off";
  int dT_off = globalParams->getParam(tempParamKey);
  tempParamKey.paramKey = "T_boilerMin";
  int T_boilerMin = globalParams->getParam(tempParamKey);
  tempParamKey.paramKey = "T_boilerMinHyst";
  int T_boilerMinHyst = globalParams->getParam(tempParamKey);

  int T_boiler = getSignalValue("T_boiler");

  int diff = T_boiler - getSignalValue("T_storage");

  if (diff >= dT_on) {
    diffPumpState = 1;
  } else if (diff <= dT_off) {
    diffPumpState = 0;
  }
  if ( T_boiler >= ( T_boilerMin + T_boilerMinHyst ) ) {
    minPumpState = 1;
  } else if (diff <= T_boilerMin) {
    minPumpState = 0;
  }
  if( ( diffPumpState & minPumpState ) == 1 ){
    pumpState = 1;
  }else{
    pumpState = 0;
  }
  emitSignal("loadPump", pumpState);
//___________________________
//________counter fan
  tempParamKey.paramKey = "initCnt";
  int initCnt = globalParams->getParam(tempParamKey);

  int startButton = getSignalValue("startButton");
  int stopButton = getSignalValue("stopButton");

  if(startButton == 1){
    m_timer.reload(initCnt);
  }
  if(stopButton == 1){
    m_timer.reload(0);
  }
  if(!m_timer.getAlarmState()){//counter is running
    emitSignal("fan", 1);
    if(pumpState == 1){
      m_timer.reload(initCnt);
    }
    emitSignal("cnt", m_timer.getSecondsToAlarm());
  }else{
    emitSignal("fan", 0);
    emitSignal("cnt", 0);
  }
}
// _____________________________________________________________________________

}//namespace
