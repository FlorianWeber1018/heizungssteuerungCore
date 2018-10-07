#include "eventhandler.h"
#include <boost/property_tree/ptree.hpp>
#include "../ModuleFramework/module.h"
#include "../ModuleFramework/modulemanager.h"
#include "../IoD/IoD.h"

namespace pt = boost::property_tree;

extern IoD::IoD globalIoD;
extern Module::ModuleManager globalModuleManager;

namespace EventHandler{

bool POST_ButtonClick(const pt::ptree& passedParams)
{
    if(passedParams.count("buttonID") > 0){
        unsigned int buttonID = passedParams.get("buttonID", 0);
        Module::Module* module = globalModuleManager.getModule(buttonID);
        if(module != nullptr){
            if(module->getModuleType() == "Button"){
                Module::Module_Button* moduleButton = static_cast<Module::Module_Button*>(module);
                moduleButton->ClickEvent();
                return true;
            }else{
                return false; // Module is not a Module_Button
            }
        }else{
            return false;//module does not exist
        }
        return true;
    }else{
        return false;
    }
}
bool POST_UpdateModuleParam(const pt::ptree &passedParams)
{
    if(passedParams.count("moduleID") > 0 && passedParams.count("paramKey") > 0 && passedParams.count("newParam") > 0){
        unsigned int moduleID = passedParams.get("moduleID", 0);
        Module::Module* module = globalModuleManager.getModule(moduleID);
        if(module != nullptr){
            std::string paramKey = passedParams.get("paramKey", "");
            int newParam = passedParams.get("newParam", 0);
            return module->changeParam(paramKey, newParam);
        }else{
            return false;//module does not exist
        }
        return true;
    }else{
        return false; //no moduleID passed
    }
}

}//namespace EventHandler
