#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include "module.h"

namespace Module {

class ModuleManager
{
public:
    ModuleManager();
    void createModule(const std::string& newModuleType, unsigned int newID);    //all (interface for rest)
    void deleteModule(const unsigned int ID);                                   //all (interface for rest)
    const std::map<unsigned int, Module*>& getAllModules();                     //only getting from local storage (interface for rest) (local storage   is/have to be   consistant with server)
protected:
    std::map<unsigned int, Module*> m_modules;                  //local Storage
    void addModule(const unsigned int ID, Module *newModule);   //local
private:
    void createModuleOnServer(const std::string& newModuleType, unsigned int newID);
    void deleteModuleOnServer(unsigned int ID);
    void getModulesFromServer(std::map<unsigned int, std::string>& outMap);
};

}


#endif // MODULEMANAGER_H
