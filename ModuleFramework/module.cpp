#include "module.h"
#include <iostream>
#include "../mysqlcon.h"
#include "timer.h"
#include <algorithm>
#include "../util.h"
extern Module::ClockDistributer globalClockDistributer;
extern mSQL::mysqlcon globalSQLCon;



namespace Module {
// ____Slot_____________________________________________________________________
void Slot::connectToSignal(Signal* _signal)
{

    this->m_signal = _signal;
    this->value = &(_signal->value);
    _signal->m_slots.push_back(this);
    this->synced = true;
}
void Slot::breakConnectionToSignal()
{
    if(m_signal != nullptr){
        this->value = nullptr;
        this->synced = false;
        util::eraseSingleElementInVector(m_signal->m_slots, this);
        m_signal = nullptr;
    }
}
bool Slot::connected()
{
   return m_signal == nullptr ? false : true;
}
// ____Signal___________________________________________________________________
void Signal::connectToSlot(Slot* _slot)
{
    _slot->m_signal = this;
    _slot->value = &(this->value);
    this->m_slots.push_back(_slot);
    _slot->synced = true;
}
void Signal::breakConnectionToSlot(Slot* _slot)
{
    _slot->breakConnectionToSignal();
}
void Signal::breakConnectionsToAllSlots()
{
    std::vector<Slot*> tempSlots(this->m_slots); //make copy
    for(auto&& element : tempSlots){
        element->breakConnectionToSignal();
    }
}
void Signal::emitSignal(int value)
{
    util::moveToBorders(value, this->min, this->max);
    this->value = value;
    for(auto&& element : m_slots){
        element->synced=true;
        if(element->m_module!=nullptr){
            element->m_module->trigger();
        }

    }
}
bool Signal::connected()
{
    return m_slots.size() > 0 ? true : false;
}
// ____Module___________________________________________________________________
void Module::emitSignal(std::string signalName, int value)
{
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
    signal->emitSignal( value );
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
    util::moveToBorders(preRet, slot->min, slot->max);
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
  for (auto&& _signal : m_signals) {
      for(auto&& _slot : _signal.second->m_slots){
          if(_slot->m_module != nullptr){
                _slot->m_module->trigger();
                if (debugMode)
                    std::cout << "1 Module triggered" << std::endl;
          }else{
                if (debugMode)
                    std::cout << "1 slot in IoD synced" << std::endl;
          }

      }
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
    newSlot->m_module = this;
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

bool Module::changeParam(const std::string& paramKey, int newParamValue)
{
    int* param;

    if(util::searchInMap(m_params, paramKey, param)){
        m_params[paramKey] = newParamValue;
        createParamOrUpdateOnServer(paramKey, newParamValue);
        return true;
    }else{
        return false;
    }
}


Module::~Module() {
    if (debugMode) {
        std::cout << "debug:~Module()" << std::endl;
    }
    for (auto&& keyValPair : m_signals) {
        keyValPair.second->breakConnectionsToAllSlots();
        delete keyValPair.second; //delete slot in map
    }
    for (auto &&keyValPair : m_slots) {
        keyValPair.second->breakConnectionToSignal();
        delete keyValPair.second;
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
  return &m_params[paramKey];
}
int Module::getParamValue(const std::string& paramKey){
    try{
        return m_params.at(paramKey);
    }
    catch (const std::exception &e) {
        if (debugMode) {
          std::cout << "exception was cought : " << e.what() << std::endl;
        }
    }
}
void Module::getParamFromServerIfExists(const std::string& paramKey, int &outParam)
{
    std::string query = "SELECT Param from ModuleConfig WHERE ModuleID = ";
    query.append(std::to_string(this->ID));
    query.append(" AND ParamKey = '");
    query.append(paramKey);
    query.append("' ;");

    MYSQL_RES *result = globalSQLCon.sendCommand(query);

    MYSQL_ROW row;
    if (result != nullptr) {
        if (row = mysql_fetch_row(result)) {
            std::string paramStr = row[0];
            outParam = std::stoi(paramStr);
        }
        mysql_free_result(result);
    }
}
void Module::createParamOrUpdateOnServer(const std::string& paramKey, const int &newParamValue)
{
    std::string query = "REPLACE INTO ModuleConfig (ModuleID, ParamKey, Param) VALUES (";
    query.append(std::to_string(this->ID));
    query.append(", '");
    query.append(paramKey);
    query.append("', ");
    query.append(std::to_string(newParamValue));
    query.append(" );");

    MYSQL_RES *result = globalSQLCon.sendCommand(query);

    MYSQL_ROW row;
    if (result != nullptr) {
        mysql_free_result(result);
    }
}
std::string Module::getModuleType()
{
    return ModuleType;
}
int Module::getParam(const std::string& paramKey)
{
    int* param = nullptr;
    util::searchInMap(m_params,paramKey,param);
    return param == nullptr ? 0 : *param;
}
const std::map<std::string, int>& Module::getAllParams()
{
    return m_params;
}
// ____ConnectionHelper_________________________________________________________
void ConnectionHelper::connect(Signal* _signal, Slot* _slot) const
{
    _slot->connectToSignal(_signal);
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
    this->ModuleType = "constant";
    globalClockDistributer.addDestination(this);

    createSignal("constSig");

    createParam("constSig", 0);
}
Module_constant::~Module_constant() {
    globalClockDistributer.rmDestination(this);
}

void Module_constant::process() {
  emitSignal("constSig", getParamValue("constSig"));
}
// ____Module_debug_____________________________________________________________

Module_debug::Module_debug(unsigned int ID) {
  this->ID = ID;
  this->ModuleType = "debug";

  createSlot("debugSlot");

  createParam("identifier", 0);
}
void Module_debug::process() {
  if(debugMode==4){
    std::cout << "Module_debug::process()" << std::endl;
  }
  std::cout << "Module_debug::" << getParamValue("identifier") << " = "
            << getSignalValue("debugSlot") << std::endl;
}
// ____Module_3WayValve_________________________________________________________
Module_3WayValve::Module_3WayValve(unsigned int ID) {
    this->ID = ID;
    this->ModuleType = "3WayValve";

    createSlot("requiredTemperature");
    createSlot("actualTemperature");
    createSlot("!EN");

    createSignal("DutyCyclePWMinc");
    createSignal("DutyCyclePWMdec");

    createParam("kp", 3000);
    createParam("up_max", INT_MAX);
    createParam("up_min", INT_MIN);
    createParam("ki", 0);
    createParam("ui_max", INT_MAX);
    createParam("ui_min", INT_MIN);
    createParam("kd", 0);
    createParam("ud_max", INT_MAX);
    createParam("ud_min", INT_MIN);

    createParam("input_max", 30000);
    createParam("input_min", -30000);
    createParam("incPWM_max", 150);
    createParam("incPWM_min", 15);
    createParam("decPWM_max", 150);
    createParam("decPWM_min", 15);
}
void Module_3WayValve::process() {
    if(debugMode==4){
        std::cout << "Module_3WayValve::process()" << std::endl;
    }
    int DC_inc, DC_dec;
    if (getSignalValue("!EN")) {
        DC_dec = getParamValue("decPWM_max");
        DC_inc = 0;
    } else {
        pid.config.syncParam(
                    getParamValue("kp"),
                    getParamValue("up_max"),
                    getParamValue("up_min"),
                    getParamValue("ki"),
                    getParamValue("ui_max"),
                    getParamValue("ui_min"),
                    getParamValue("kd"),
                    getParamValue("ud_max"),
                    getParamValue("ud_min")
                    );
        pwm.config.syncParam(
                    getParamValue("input_max"),
                    getParamValue("input_min"),
                    getParamValue("incPWM_max"),
                    getParamValue("incPWM_min"),
                    getParamValue("decPWM_max"),
                    getParamValue("decPWM_min")
                    );
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
    this->ModuleType = "2Point";
    createSlot("T1");
    createSlot("T2");

    createSignal("outState");

    createParam("dT_on", 8);
    createParam("dT_off", 2);
}
void Module_2Point::process() {
  if(debugMode==4){
    std::cout << "Module_2Point::process()" << std::endl;
  }

  int dT_on = getParamValue("dT_on");
  int dT_off = getParamValue("dT_off");

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
  this->ModuleType = "inverter";
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
  this->ModuleType = "medianFilter";
  createSlot("S");

  createSignal("S");

  createParam("size", 3);
}
void Module_MedianFilter::process() {
  if(debugMode==4){
    std::cout << "Module_MedianFilter::process()" << std::endl;
  }

  int NewSize = getParamValue("size");
  int NewValue = getSignalValue("S");

  m_values.push_back(NewValue);

  while(m_values.size() > NewSize){   //remove elements till the buffer hats the correct size from the config
    m_values.erase(m_values.begin());
  }
  int value = util::findMedian(m_values);

  emitSignal("S", value);
}
// ____MODULE_Woodstove_________________________________________________________
Module_Woodstove::Module_Woodstove(unsigned int ID) {
    this->ID = ID;
    this->ModuleType = "woodstove";
    createSlot("T_boiler");
    createSlot("T_storage");
    createSlot("startButton");
    createSlot("stopButton");

    createSignal("loadPump");
    createSignal("fan");
    createSignal("cnt");

    createParam("dT_on", 8);
    createParam("dT_off", 4);
    createParam("T_boilerMin", 55);
    createParam("T_boilerMinHyst", 4);
    createParam("initCnt", 3600);
}
void Module_Woodstove::process() {
  if(debugMode==4){
    std::cout << "Module_Woodstove::process()" << std::endl;
  }


//________2point loadPump___

  int dT_on = getParamValue("dT_on");
  int dT_off = getParamValue("dT_off");
  int T_boilerMin = getParamValue("T_boilerMin");
  int T_boilerMinHyst = getParamValue("T_boilerMinHyst");
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
  int initCnt = getSignalValue("initCnt");

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
    emitSignal("cnt", 0);
  }
}
// _____________________________________________________________________________

}//namespace
