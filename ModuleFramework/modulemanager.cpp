#include "modulemanager.h"
#include "module.h"
#include "../mysqlcon.h"

extern mSQL::mysqlcon globalSQLCon;

namespace Module {
ModuleManager::ModuleManager()
{
    std::map<unsigned int, std::string> moduleMap;
    getModulesFromServer(moduleMap);
    for(auto&& element : moduleMap){
        createModule(element.second, element.first);
    }

}
void ModuleManager::addModule(const unsigned int ID, Module* newModule)
{
    m_modules[ID] = newModule;
}
void ModuleManager::deleteModule(const unsigned int ID)
{
    deleteModuleOnServer(ID);
    m_modules.erase(ID);
}
const std::map<unsigned int, Module*>& ModuleManager::getAllModules()
{
    return m_modules;
}

void ModuleManager::createModuleOnServer(const std::string& newModuleType, unsigned int newID)
{
    std::string query = "INSERT INTO ModuleList (ID, TYPE) VALUES (";
    query.append(std::to_string(newID));
    query.append(", '");
    query.append(newModuleType);
    query.append("') ;");

    MYSQL_RES *result = globalSQLCon.sendCommand(query);

    if (result != nullptr) {
        mysql_free_result(result);
    }
}
void ModuleManager::deleteModuleOnServer(unsigned int ID)
{
    std::string query = "DELETE FROM ModuleList WHERE ID = ";
    query.append(std::to_string(ID));
    query.append(";");

    MYSQL_RES *result = globalSQLCon.sendCommand(query);

    if (result != nullptr) {
        mysql_free_result(result);
    }
}
void ModuleManager::createModule(const std::string& newModuleType, unsigned int newID)
{
    if(newModuleType == "constant"){
        addModule(newID, new Module_constant(newID));
    }else if(newModuleType == "debug"){
        addModule(newID, new Module_debug(newID));
    }else if(newModuleType == "2Point"){
        addModule(newID, new Module_2Point(newID));
    }else if(newModuleType == "3WayValve"){
        addModule(newID, new Module_3WayValve(newID));
    }else if(newModuleType == "inverter"){
        addModule(newID, new Module_Inverter(newID));
    }else if(newModuleType == "medianFilter"){
        addModule(newID, new Module_MedianFilter(newID));
    }else if(newModuleType == "woodstove"){
        addModule(newID, new Module_Woodstove(newID));
    }
}

void ModuleManager::getModulesFromServer(std::map<unsigned int, std::string>& outMap)
{
    std::string query = "SELECT ID, Type from ModuleList;";

    MYSQL_RES *result = globalSQLCon.sendCommand(query);

    MYSQL_ROW row;
    if (result != nullptr) {
        while (row = mysql_fetch_row(result)) {
            outMap[static_cast<unsigned int>(std::stol(row[0]))] = row[1];
        }
        mysql_free_result(result);
    }
}







}

