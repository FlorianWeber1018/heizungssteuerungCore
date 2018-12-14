#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include "module.h"
#include <boost/property_tree/ptree_fwd.hpp>
namespace Module {

struct ModuleDescription{
    std::string TYPE = "";
    std::string UserDescription = "";
};
class ModuleManager
{
public:
    pt::ptree getProperties();
    ModuleManager();
    void createModule(const ModuleDescription &newModule);                        //all (interface for rest)
    void createModule(const ModuleDescription &newModule, unsigned int newID);    //all (interface for rest)
    void createModule(const std::string &newModule);                        //all (interface for rest) ONLY TEMP
    void createModule(const std::string &newModule, unsigned int newID);    //all (interface for rest) ONLY TEMP
    void deleteModule(const unsigned int ID);                                   //all (interface for rest)
    Module* getModule(const unsigned int ID);                                   //all (interface for rest)
    const std::map<unsigned int, Module*>& getAllModules();                     //only getting from local storage (interface for rest) (local storage   is/have to be   consistant with server)

    const std::map<std::pair<unsigned int, std::string>,std::pair<unsigned int, std::string>>& getAllConnections();
    void makeConnection(
            unsigned int destModuleID, std::string destSlotName,
            unsigned int srcModuleID, std::string srcSignalName);
    void deleteConnection(unsigned int destModuleID, std::string destSlotName);
protected:
    std::map<unsigned int, Module*> m_modules;                  //local Storage
    std::map<std::pair<unsigned int, std::string>,std::pair<unsigned int, std::string>> m_connections;
    void makeConnection(Slot* _slot, Signal* _signal);  //make connection between signal and slot
    void makeConnection(
        Module* destModule, std::string destSlotName,               //make connection between Modules
        Module* srcModule, std::string srcSignalName);
    void makeConnectionToInput(
        Module* destModule, std::string destSlotName,
        Signal* inputSignal);
    void makeConnectionToOutput(
        Slot* outputSlot,
        Module* srcModule, std::string srcSignalName);
    void addConnection(
            unsigned int destModuleID, std::string destSlotName,        //add connection to local storage
            unsigned int srcModuleID, std::string srcSignalName);
    void addModule(const unsigned int ID, Module *newModule);   //local

    void createConnectionOrUpdateOnServer(
            unsigned int destModuleID, std::string destSlotName,
            unsigned int srcModuleID, std::string srcSignalName);
    unsigned int getBiggestIdFromTable (const std::string& TableName);
    void deleteConnectionsOfModuleOnServer(const unsigned int ID);
    void deleteParamsOfModuleOnServer(const unsigned int ID);
    void createModuleOnServer(const ModuleDescription& newModule, unsigned int newID);
    void deleteModuleOnServer(unsigned int ID);
    void getModulesFromServer(std::map<unsigned int, ModuleDescription> &outMap);
    void deleteConnectionOnServer(unsigned int destModuleID, std::string destSlotName);
    void getConnectionsFromServer(
            std::map<   std::pair<unsigned int, std::string>,
                        std::pair<unsigned int, std::string>>& outMap); //map pair(uint,str)->pair(uint,str)
    void breakConnection(Module* _module, std::string slotName);                                                                   //           dest            src
};
}//namespace

#endif // MODULEMANAGER_H
