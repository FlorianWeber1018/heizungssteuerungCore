#include <boost/property_tree/ptree.hpp>
#include "modulemanager.h"
#include "module.h"
#include "../mysqlcon.h"
#include "../util.h"
#include "../IoD/IoD.h"

extern mSQL::mysqlcon globalSQLCon;
extern IoD::IoD globalIoD;

namespace Module {

pt::ptree ModuleManager::getProperties()
{
    pt::ptree tree;
    tree.put("TYPE", "ModuleManager");

    pt::ptree modulesTree;

    pt::ptree IoDTree = globalIoD.getProperties();
    tree.put_child("IoD", IoDTree);

    for(auto&& element : m_modules)
    {
        pt::ptree _moduleTree = element.second->getProperties();
        //_moduleTree.put("ID", element.first);
        modulesTree.put_child(std::to_string(element.first), _moduleTree);
    }
    tree.put_child("modules", modulesTree);

    return tree;
}
ModuleManager::ModuleManager()
{
    std::map<unsigned int, ModuleDescription> moduleMap;
    getModulesFromServer(moduleMap);
    for(auto&& element : moduleMap){
        createModule(element.second, element.first);
    }
    getConnectionsFromServer(m_connections);
    for(auto&& element : m_connections){
        makeConnection(element.first.first, element.first.second, element.second.first, element.second.second);
    }

}
void ModuleManager::addModule(const unsigned int ID, Module* newModule)
{
    m_modules[ID] = newModule;
}
void ModuleManager::addConnection(
        unsigned int destModuleID, std::string destSlotName,
        unsigned int srcModuleID, std::string srcSignalName)
{
    m_connections[std::make_pair(destModuleID, destSlotName)] = std::make_pair(srcModuleID, srcSignalName);
}
void ModuleManager::deleteModule(const unsigned int ID)
{
    Module** tempModule = nullptr;
    bool moduleExist = util::searchInMap(m_modules, ID, tempModule);
    if(moduleExist){
        if(*tempModule != nullptr){
            delete *tempModule;
            deleteParamsOfModuleOnServer(ID);
            deleteConnectionsOfModuleOnServer(ID);
            deleteModuleOnServer(ID);
            m_modules.erase(ID);
        }
    }
}
Module* ModuleManager::getModule(const unsigned int ID){
    Module** result = nullptr;
    if(util::searchInMap(m_modules, ID, result)){
        return *result;
    }else{
        return nullptr;
    }

}
const std::map<unsigned int, Module*>& ModuleManager::getAllModules()
{
    return m_modules;
}

void ModuleManager::createModuleOnServer(const ModuleDescription& newModule, unsigned int newID)
{
    std::string query = "INSERT INTO ModuleList (ID, TYPE) VALUES (";
    query.append(std::to_string(newID));
    query.append(", '");
    query.append(newModule.TYPE);
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
void ModuleManager::createModule(const std::string &newModule)
{
    ModuleDescription t;
    t.TYPE = newModule;
    createModule(t);
}
void ModuleManager::createModule(const std::string &newModule, unsigned int newID)
{
    ModuleDescription t;
    t.TYPE = newModule;
    createModule(t,newID);
}
void ModuleManager::createModule(const ModuleDescription& newModule)
{
    unsigned int newID = getBiggestIdFromTable("ModuleList");
    newID++;
    createModule(newModule, newID);
}
void ModuleManager::createModule(const ModuleDescription& newModule, unsigned int newID)
{
    Module** tempModuleNotUsed = nullptr;
    if( ! util::searchInMap(m_modules, newID, tempModuleNotUsed)){
        createModuleOnServer(newModule, newID);
        if(newModule.TYPE == "constant"){
            addModule(newID, new Module_constant(newID));
        }else if(newModule.TYPE == "debug"){
            addModule(newID, new Module_debug(newID));
        }else if(newModule.TYPE == "2Point"){
            addModule(newID, new Module_2Point(newID));
        }else if(newModule.TYPE == "3WayValve"){
            addModule(newID, new Module_3WayValve(newID));
        }else if(newModule.TYPE == "inverter"){
            addModule(newID, new Module_Inverter(newID));
        }else if(newModule.TYPE == "medianFilter"){
            addModule(newID, new Module_MedianFilter(newID));
        }else if(newModule.TYPE == "woodstove"){
            addModule(newID, new Module_Woodstove(newID));
        }else if(newModule.TYPE == "button"){
            addModule(newID, new Module_Button(newID));
        }else if(newModule.TYPE == "AND"){
            addModule(newID, new Module_AND(newID));
        }else if(newModule.TYPE == "OR"){
            addModule(newID, new Module_OR(newID));
        }else if(newModule.TYPE == "MUX"){
            addModule(newID, new Module_MUX(newID));
        }else if(newModule.TYPE == "datalogger"){
            addModule(newID, new Module_datalogger(newID));
        }else if(newModule.TYPE == "transformation"){
            addModule(newID, new Module_transformation(newID));
        }else if(newModule.TYPE == "clockTimer"){
            addModule(newID, new Module_clockTimer(newID));
        }
    }
}

void ModuleManager::getModulesFromServer(std::map<unsigned int, ModuleDescription >& outMap)
{
    std::string query = "SELECT ID, Type, UserDescription from ModuleList;";

    MYSQL_RES *result = globalSQLCon.sendCommand(query);

    MYSQL_ROW row;
    if (result != nullptr) {
        while (row = mysql_fetch_row(result)) {
            outMap[static_cast<unsigned int>(std::stol(row[0]))].TYPE = row[1];
            outMap[static_cast<unsigned int>(std::stol(row[0]))].UserDescription = row[2];
        }
        mysql_free_result(result);
    }
}
unsigned int ModuleManager::getBiggestIdFromTable (const std::string& TableName)
{
    std::string query = "SELECT MAX(ID) FROM ";
    query += TableName;
    query += ";";

    unsigned int resultID = 0;

    MYSQL_RES *result = globalSQLCon.sendCommand(query);

    MYSQL_ROW row;
    if (result != nullptr) {
        if (row = mysql_fetch_row(result)) {
            resultID = std::stol(row[0]);
        }
        mysql_free_result(result);
    }
    return resultID;
}
void ModuleManager::deleteConnectionsOfModuleOnServer(const unsigned int ID)
{
    std::string query = "DELETE FROM ConnectionList WHERE destID = ";
    query += std::to_string(ID);
    query += " OR srcID = ";
    query += std::to_string(ID);
    query += ";";

    MYSQL_RES *result = globalSQLCon.sendCommand(query);

    if (result != nullptr) {
        mysql_free_result(result);
    }
}
void ModuleManager::deleteParamsOfModuleOnServer(const unsigned int ID)
{
    std::string query = "DELETE FROM ModuleConfig WHERE ModuleID = ";
    query.append(std::to_string(ID));
    query.append(";");

    MYSQL_RES *result = globalSQLCon.sendCommand(query);

    if (result != nullptr) {
        mysql_free_result(result);
    }
}
void ModuleManager::makeConnection(Slot* _slot, Signal* _signal)
{
    if(_slot != nullptr && _signal != nullptr){
        _slot->connectToSignal(_signal);
    }
}
void ModuleManager::makeConnection(Module* destModule, std::string destSlotName,
                                   Module* srcModule, std::string srcSignalName)
{

    makeConnection(destModule->getSlot(destSlotName), srcModule->getSignal(srcSignalName));
}
void ModuleManager::makeConnectionToInput(Module* destModule, std::string destSlotName, Signal* inputSignal)
{
    makeConnection(destModule->getSlot(destSlotName),inputSignal);
    globalIoD.syncInUse();
}
void ModuleManager::makeConnectionToOutput(Slot* outputSlot, Module* srcModule, std::string srcSignalName)
{
    makeConnection(outputSlot, srcModule->getSignal(srcSignalName));
    globalIoD.syncInUse();
}
void ModuleManager::makeConnection(
        unsigned int destModuleID, std::string destSlotName,
        unsigned int srcModuleID, std::string srcSignalName)
{
    Module** destModule;
    Module** srcModule;
    bool destModuleFound = util::searchInMap(m_modules, destModuleID, destModule);
    bool srcModuleFound = util::searchInMap(m_modules, srcModuleID, srcModule);

    if(destModuleID == 0){
        if(srcModuleID == 0){
            //connection: IoD(input)->IoD(output)
            //USELESS not yet supported
        }else{
            //connection: Module->IoD(output)
            if(srcModuleFound){
                if(destSlotName.length() > 1 && destSlotName.at(0) == 'I' ){
                    makeConnectionToOutput(globalIoD.getSlot(std::stoi(destSlotName.substr(1))),*srcModule, srcSignalName);
                    addConnection(destModuleID, destSlotName, srcModuleID, srcSignalName);
                    createConnectionOrUpdateOnServer(destModuleID, destSlotName, srcModuleID, srcSignalName);
                }
            }
        }
    }else{
        if(srcModuleID == 0){
            //connection: IoD(input)->Module
            if(destModuleFound){
                if(srcSignalName.length() > 1 && ( srcSignalName.at(0) == 'A' || srcSignalName.at(0) == 'I' ) ){
                    makeConnectionToInput(*destModule, destSlotName, globalIoD.getSignal(srcSignalName.at(0), std::stoi(srcSignalName.substr(1))));
                    addConnection(destModuleID, destSlotName, srcModuleID, srcSignalName);
                    createConnectionOrUpdateOnServer(destModuleID, destSlotName, srcModuleID, srcSignalName);
                }

            }

        }else{
            //connection: Module->Module
            if(destModuleFound && srcModuleFound){
                makeConnection(*destModule, destSlotName, *srcModule, srcSignalName);
                addConnection(destModuleID, destSlotName, srcModuleID, srcSignalName);
                createConnectionOrUpdateOnServer(destModuleID, destSlotName, srcModuleID, srcSignalName);
            }
        }
    }




}

void ModuleManager::createConnectionOrUpdateOnServer(
        unsigned int destModuleID, std::string destSlotName,
        unsigned int srcModuleID, std::string srcSignalName)
{

    std::string query = "REPLACE INTO ConnectionList (destID, destSlotName, srcID, srcSignalName) VALUES (";
    query.append(std::to_string(destModuleID));
    query.append(", '");
    query.append(destSlotName);
    query.append("', ");
    query.append(std::to_string(srcModuleID));
    query.append(", '");
    query.append(srcSignalName);
    query.append("' );");

    MYSQL_RES *result = globalSQLCon.sendCommand(query);

    MYSQL_ROW row;
    if (result != nullptr) {
        mysql_free_result(result);
    }
}
void ModuleManager::getConnectionsFromServer(std::map<std::pair<unsigned int, std::string>, std::pair<unsigned int, std::string> > &outMap)
{
    std::string query = "SELECT destID, destSlotName, srcID, srcSignalName FROM ConnectionList;";

    MYSQL_RES *result = globalSQLCon.sendCommand(query);

    MYSQL_ROW row;
    if (result != nullptr) {
        while (row = mysql_fetch_row(result)) {
            outMap[std::make_pair(static_cast<unsigned int>(std::stol(row[0])),row[1])]
                 = std::make_pair(static_cast<unsigned int>(std::stol(row[2])),row[3]);
        }
        mysql_free_result(result);
    }
}
void ModuleManager::deleteConnectionOnServer(unsigned int destModuleID, std::string destSlotName)
{
    std::string query = "DELETE FROM ConnectionList WHERE destID = ";
    query.append(std::to_string(destModuleID));
    query.append(" AND destSlotName = '");
    query.append(destSlotName);
    query.append("';");

    MYSQL_RES *result = globalSQLCon.sendCommand(query);

    if (result != nullptr) {
        mysql_free_result(result);
    }
}
void ModuleManager::deleteConnection(unsigned int destModuleID, std::string destSlotName) //for rest (public)
{
    if(destModuleID == 0){
        if(destSlotName.size()>1 && destSlotName.at(0)== 'I');
        Slot* _slot = globalIoD.getSlot(std::stoi(destSlotName.substr(1)));
        if(_slot != nullptr){
            _slot->breakConnectionToSignal();
            m_connections.erase(std::make_pair(destModuleID, destSlotName));
            deleteConnectionOnServer(destModuleID, destSlotName);
            globalIoD.syncInUse();
        }
    }else{
        Module** destModule;
        bool destModuleFound = util::searchInMap(m_modules, destModuleID, destModule);
        if(destModuleFound){
            breakConnection(*destModule, destSlotName);
            m_connections.erase(std::make_pair(destModuleID, destSlotName));
            deleteConnectionOnServer(destModuleID, destSlotName);
        }
    }

}
void ModuleManager::breakConnection(Module* _module, std::string slotName)
{
    Slot* _slot = _module->getSlot(slotName);
    if(_slot != nullptr){
        _slot->breakConnectionToSignal();
    }
}
const std::map<std::pair<unsigned int, std::string>,std::pair<unsigned int, std::string>>& ModuleManager::getAllConnections()
{
    return m_connections;
}

}

