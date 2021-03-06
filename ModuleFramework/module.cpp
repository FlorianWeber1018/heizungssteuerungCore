#include "module.h"
#include <iostream>
#include "../mysqlcon.h"
#include "timer.h"
#include <algorithm>
#include "../util.h"
#include <boost/property_tree/ptree.hpp>
#include <chrono>
#include <ctime>
extern Module::ClockDistributer globalClockDistributer;
extern mSQL::mysqlcon globalSQLCon;



namespace Module {
// ____Slot_____________________________________________________________________

pt::ptree Slot::getProperties()
{
    pt::ptree tree;
    tree.put("TYPE", "Slot");
    tree.put("moduleID", getParentModuleID());
    tree.put("keyName", name);
    int value = 0;
    if(this->value != nullptr){
        value = *(this->value);
    }
    tree.put("value", value);
    pt::ptree connectedSignal;
    if(m_signal != nullptr){
        connectedSignal.put("TYPE", "Signal");
        connectedSignal.put("moduleID", m_signal->getParentModuleID());
        connectedSignal.put("keyName", m_signal->name);
    }


    tree.put_child("connection", connectedSignal);
    return tree;
}
unsigned int Slot::getParentModuleID()
{
    return m_parentModule == nullptr ? 0 : m_parentModule->ID;
}
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

pt::ptree Signal::getProperties()
{
    pt::ptree tree;
    tree.put("TYPE", "Signal");
    tree.put("moduleID", getParentModuleID());
    tree.put("keyName", name);
    pt::ptree connectedSlots;
    for(auto&& element : m_slots){
        pt::ptree connectedSlot;
        connectedSlot.put("TYPE", "Slot");
        connectedSlot.put("moduleID", element->getParentModuleID());
        connectedSlot.put("keyName", element->name);

        std::string nodeKeyStr = std::to_string(element->getParentModuleID());
        nodeKeyStr += "__";
        nodeKeyStr += element->name;

        connectedSlots.put_child(nodeKeyStr, connectedSlot);
    }
    tree.put("value", value);
    tree.put_child("connections", connectedSlots);
    return tree;
}
unsigned int Signal::getParentModuleID()
{
    return m_parentModule == nullptr ? 0 : m_parentModule->ID;
}
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
        if(element->m_parentModule!=nullptr){
            element->m_parentModule->trigger();
        }

    }
}
bool Signal::connected()
{
    return m_slots.size() > 0 ? true : false;
}
// ____Module___________________________________________________________________
pt::ptree Module::getProperties()
{
    pt::ptree tree;
    tree.put("TYPE", "Module");
    tree.put("ID", this->ID);
    tree.put("moduleType", ModuleType);

    pt::ptree parameterTree;
    pt::ptree signalTree;
    pt::ptree slotTree;


    for(auto&& parameter : m_params)
    {
        std::string keyToParameterKey = parameter.first;
        keyToParameterKey += ".key";
        std::string keyToParameterValue = parameter.first;
        keyToParameterValue += ".value";
        parameterTree.put(keyToParameterKey, parameter.first);
        parameterTree.put(keyToParameterValue, parameter.second);
    }
    for(auto&& element : m_signals)
    {
        pt::ptree _signalTree = element.second->getProperties();
        signalTree.put_child(element.first, _signalTree);
    }
    for(auto&& element : m_slots)
    {
        pt::ptree _slotTree = element.second->getProperties();
        slotTree.put_child(element.first, _slotTree);
    }
    tree.put_child("parameters", parameterTree);
    tree.put_child("signals", signalTree);
    tree.put_child("slots", slotTree);

    return tree;
}
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
          if(_slot->m_parentModule != nullptr){
                _slot->m_parentModule->trigger();
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
    newSignal->m_parentModule = this;
    newSignal->name = signalName;
    m_signals[signalName] = newSignal;
    return newSignal;
  } else {
    return m_signals.at(signalName);
  }
}

Slot *Module::createSlot(std::string slotName) {
  if (m_slots.count(slotName) == 0) {
    Slot *newSlot = new Slot();
    newSlot->m_parentModule = this;
    newSlot->name = slotName;
    m_slots[slotName] = newSlot;  
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
        return createParamOrUpdateOnServer(paramKey, newParamValue);
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
        return 0;
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
bool Module::createParamOrUpdateOnServer(const std::string& paramKey, const int &newParamValue)
{
    std::string query = "REPLACE INTO ModuleConfig (ModuleID, ParamKey, Param) VALUES (";
    query.append(std::to_string(this->ID));
    query.append(", '");
    query.append(paramKey);
    query.append("', ");
    query.append(std::to_string(newParamValue));
    query.append(" );");

    return globalSQLCon.sendCUD(query);
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
const std::map<std::string, Signal*>& Module::getAllSignals()
{
    return m_signals;
}
const std::map<std::string, Slot*>& Module::getAllSlots()
{
    return m_slots;
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

    createSignal("up");
    createSignal("ui");
    createSignal("ud");

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
    emitSignal("up",static_cast<int>(pid.getUp()));
    emitSignal("ui",static_cast<int>(pid.getUi()));
    emitSignal("ud",static_cast<int>(pid.getUd()));
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

  size_t NewSize = static_cast<uint>(getParamValue("size"));
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
    createParam("reloadCnt", 1200);
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
  } else if (T_boiler <= T_boilerMin) {
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
  int initCnt = getParamValue("initCnt");
  int reloadCnt = getParamValue("reloadCnt");

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
    if(pumpState == 1 && m_timer.getSecondsToAlarm() < reloadCnt){
      m_timer.reload(reloadCnt);
    }
    emitSignal("cnt", m_timer.getSecondsToAlarm());
  }else{
    emitSignal("fan", 0);
    emitSignal("cnt", 0);
  }
}

// ____MODULE_Button____________________________________________________________
Module_Button::Module_Button(unsigned int ID)
{
    this->ID = ID;
    this->ModuleType = "Button";
    createSignal("S");
    createParam("Mode", btn_trigger);
    globalClockDistributer.addDestination(this);
}
void Module_Button::ClickEvent()
{
    switch (getParamValue("Mode"))
    {
        case btn_toggle:{
            if(value == 1){
                value = 0;
            }else if(value == 0){
                value = 1;
            }
        }break;
        case btn_trigger:{
            value = 1;
        }break;
    }
}
void Module_Button::process()
{
    emitSignal("S", value);
    if(getParam("Mode") == btn_trigger){
        value = 0;
    }
}
// _____________________________________________________________________________
Module_AND::Module_AND(unsigned int ID)
{
    this->ID = ID;
    this->ModuleType = "AND";
    createSignal("S");
    createSlot("S0");
    createSlot("S1");
    createSlot("S2");
    createSlot("S3");
    createSlot("S4");
    createSlot("S5");
    createSlot("S6");
    createSlot("S7");
}
void Module_AND::process()
{
    bool result = true;
    for(auto&& element : m_slots)
    {
        if(element.second->connected()){
            if( ! (getSignalValue(element.first) > 0) ){
                result &= false;
            }
        }
    }
    if(result){
        emitSignal("S", 1);
    }else{
        emitSignal("S", 0);
    }
}
// _____________________________________________________________________________
Module_OR::Module_OR(unsigned int ID)
{
    this->ID = ID;
    this->ModuleType = "OR";
    createSignal("S");
    createSlot("S0");
    createSlot("S1");
    createSlot("S2");
    createSlot("S3");
    createSlot("S4");
    createSlot("S5");
    createSlot("S6");
    createSlot("S7");
}
void Module_OR::process()
{
    int result = 0;
    for(auto&& element : m_slots)
    {
        if(element.second->connected()){
            result += getSignalValue(element.first);
        }
    }
    util::moveToBorders(result,0, 1);
    emitSignal("S", result);
}
// _____________________________________________________________________________
Module_MUX::Module_MUX(unsigned int ID)
{
    this->ID = ID;
    this->ModuleType = "MUX";
    createSignal("S");
    createSlot("Select");
    createSlot("S0");
    createSlot("S1");
    createSlot("S2");
    createSlot("S3");
    createSlot("S4");
    createSlot("S5");
    createSlot("S6");
    createSlot("S7");
}
void Module_MUX::process()
{
    int select = getSignalValue("Select");
    util::moveToBorders(select, 0, 7);
    std::string SlotName = "S";
    SlotName += std::to_string(select);
    emitSignal("S", getSignalValue(SlotName));
}
// _____________________________________________________________________________
Module_datalogger::Module_datalogger(unsigned int ID)
{
    this->ID = ID;
    this->ModuleType = "datalogger";
    createParam("label", 0);
    createParam("clkDiv", 1);
    createSlot("S");
    createSlot("EN");
}
void Module_datalogger::insertIntoLoggingTableOnServer(const int& label, const int& value)
{
     std::string query = "INSERT INTO DataLog (label, millisec, value) VALUES(";
     query.append(std::to_string(label));
     query.append(", UNIX_TIMESTAMP(CURTIME(3)) * 1000, ");
     query.append(std::to_string(value));
     query.append(" );");
     MYSQL_RES *result = globalSQLCon.sendCommand(query);

     if (result != nullptr) {
         mysql_free_result(result);
     }
}
void Module_datalogger::process()
{
    cnt++;
    if(getSignalValue("EN") > 0){
        if(cnt >= getParam("clkDiv")){
            insertIntoLoggingTableOnServer(getParam("label"), getSignalValue("S"));
            cnt=0;
        }
    }

}
// _____________________________________________________________________________

Module_transformation::Module_transformation(unsigned int ID)
{
    this->ID = ID;
    this->ModuleType = "transformation";
    createSignal("S");
    createSlot("S");
    createParam("m", 1);
    createParam("b", 0);
}
void Module_transformation::process()
{
    float x = static_cast<float>(getSignalValue("S"));
    float m = static_cast<float>(getParam("m"));
    float b = static_cast<float>(getParam("b"));
    float y = (m * x) + b;
    emitSignal("S", static_cast<int>(y));
}
// _____________________________________________________________________________

Module_clockTimer::Module_clockTimer(unsigned int ID)
{
    this->ID = ID;
    this->ModuleType = "clockTimer";
    createSignal("S");
    createParam("Te", 0);
    createParam("Ta", 0);
    globalClockDistributer.addDestination(this);
}
void Module_clockTimer::process()
{
    unsigned int Te = getParam("Te");
    unsigned int Ta = getParam("Ta");
    auto utcTime = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
    std::tm now = *std::localtime( &utcTime );

    unsigned int h = now.tm_hour;
    unsigned int m = now.tm_min;
    unsigned int time = (h * 100) + m;
    int outsignal = 0 ;
    if(Te > Ta){
        if(time > Ta && time < Te){
            outsignal = 0;
        }else{
            outsignal = 1;
        }
    }else if(Te < Ta){
        if(time > Te && time < Ta){
            outsignal = 1;
        }else{
            outsignal = 0;
        }
    }else if(Te == Ta){
        outsignal = 0;
    }
    emitSignal("S", outsignal);
    //std::cout << "state:" << outsignal << std::endl;
}
// _____________________________________________________________________________
}//namespace
