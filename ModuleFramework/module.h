#ifndef module__h
#define module__h


#define debugMode 0

#include <algorithm>
#include <climits>
#include <vector>
#include <string>
#include <map>
#include <set>

#include "timer.h"
#include "control.h"



namespace Module{
/*struct mySqlSignal{
  std::string DeviceID;
  std::string PortType;
  std::string Port;
  std::string Pin;
  bool operator==(const mySqlSignal& otherSig) const
  {
    return (
      this->DeviceID  ==  otherSig.DeviceID   &&
      this->PortType  ==  otherSig.PortType   &&
      this->Port      ==  otherSig.Port       &&
      this->Pin       ==  otherSig.Pin
    );
  }
  bool operator < ( const mySqlSignal& otherSig ) const
  {
    return(
      ( this->DeviceID <  otherSig.DeviceID )  ||
      ( this->DeviceID == otherSig.DeviceID && this->PortType <  otherSig.PortType ) ||
      ( this->DeviceID == otherSig.DeviceID && this->PortType == otherSig.PortType && this->Port <  otherSig.Port ) ||
      ( this->DeviceID == otherSig.DeviceID && this->PortType == otherSig.PortType && this->Port == otherSig.Port && this->Pin <  otherSig.Pin )
    );
  }
};*/
struct mySqlParam{
  unsigned int ID;
  std::string paramKey;
  bool operator == (const mySqlParam& otherParam) const
  {
    return(
      this->ID        ==    otherParam.ID   &&
      this->paramKey  ==    otherParam.paramKey
    );
  }
  bool operator < (const mySqlParam& otherParam) const
  {
    return(
      (this->ID <   otherParam.ID)    ||
      (this->ID ==  otherParam.ID   &&  this->paramKey  <  otherParam.paramKey)
    );
  }
};

struct Slot
{
  int* value;
  int min = INT_MIN;
  int max = INT_MAX;
  bool synced = false;
};

struct Signal
{
  int value;
  int min = INT_MIN;
  int max = INT_MAX;
  std::vector<Slot*> m_slots;
};
class Module
{
public:
  Signal* getSignal(std::string signalName);
  Slot* getSlot(std::string slotName);
  void addPostModule(Module* postModule);
  void trigger();
  void changeParam(const std::string& paramKey, int newParamValue);

  ~Module();
  Module();
  unsigned int ID = 0;
private:

protected:
  std::vector<Module*> m_postModules;
  std::map<std::string, Signal*> m_signals;
  std::map<std::string, Slot*> m_slots;
  std::map<std::string, int> m_params; //TODO
  Signal* createSignal(std::string signalName);
  Slot* createSlot(std::string slotName);
  void triggerNext();
  virtual void process();
  void emitSignal(std::string signalName, int value);
  int getSignalValue(std::string slotName);
  int getNextAVID(); //TODO
  void createParamOrUpdateOnServer(const std::string& paramKey, int newParamValue); //TODO
  int tryToGetParamFromServer(const std::string& paramKey); //TODO
};

class ClockDistributer{
  public:
    void trigger();
    void addDestination(Module* destModule);
    void rmDestination(Module* destModule);
  protected:
    std::set<Module*> m_destModules;
};

// ____Module which provides a constant Signal defined by Config________________
class Module_constant: public Module
{
public:
  Module_constant(unsigned int ID);
  ~Module_constant();

private:

protected:
  void process() override;
};
// _____________________________________________________________________________

// ____Module which prints an Value for debugging to console,____________________
//     identifier is given by config


class Module_debug : public Module
{
  public:
    Module_debug(unsigned int ID);
  private:

  protected:
    void process() override;
};


// _____________________________________________________________________________

//____Module which controls an 3-way-valve______________________________________
//    IN:  requiredTemperature , actualTemperatur
//    OUT: DutyCyclePWMinc, DutyCyclePWMdec,



class Module_3WayValve : public Module
{
public:
  Module_3WayValve(unsigned int ID);
protected:
  PID pid;
  ServoPWM pwm;
  void process() override;
private:
};
//______________________________________________________________________________
//____Module which provides an 2-Point Controller_______________________________
//    IN:  T1, T2
//    OUT: state (1/0)

class Module_2Point : public Module
{
public:
  Module_2Point(unsigned int ID);

protected:
  int outState = 0;
  void process() override;
private:
};
//______________________________________________________________________________
//____Module which inverts an Bool Signal_______________________________________
//    IN:  S (1/0)
//    OUT: S (0/1)
class Module_Inverter : public Module
{
public:
  Module_Inverter(unsigned int ID);
protected:
  void process() override;
private:
};
//______________________________________________________________________________
//____Module which implements an Median filter__________________________________
//    IN:  S (1/0)
//    OUT: S (0/1)
class Module_MedianFilter : public Module
{
public:
  Module_MedianFilter(unsigned int ID);
protected:
  std::vector<int> m_values;
  void process() override;
private:
};
//______________________________________________________________________________
//____Module which controlls an wood fired oven_________________________________
//    INPUT={T_boiler ,T_storage , startButton , stopButton}
//    OUTPUT= {loadPump , fan , count}

class Module_Woodstove : public Module
{
public:
  Module_Woodstove(unsigned int ID);
protected:
  int diffPumpState = 0;
  int minPumpState = 0;
  int pumpState = 0;
  Timer m_timer;
  void process() override;
private:
};
//______________________________________________________________________________
}
#endif
