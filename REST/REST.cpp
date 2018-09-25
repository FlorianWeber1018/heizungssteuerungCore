#include <string>
#include "REST.h"
#include "crow_all.h"
#include "../IoD/IoD.h"
#include "../ModuleFramework/modulemanager.h"
#include "../ModuleFramework/module.h"


extern IoD::IoD globalIoD;
extern Module::ModuleManager globalModuleManager;
namespace REST{
    void restMain()
    {
        crow::SimpleApp app;
        #include "HardwareModule/index.cpp" //include HardwaremoduleHandlers
        #include "ModuleManager/index.cpp"  //include ModulemanagerHandlers



        app.loglevel(crow::LogLevel::Warning);
        app.port(18080).multithreaded().run();
    }

    /*void convertToJson(crow::json::wvalue& outJson, const std::map<std::string&, int> inputMap)
    {
        for(auto&& element : inputMap){
            outJson[element.first] = element.second;
        }
    }*/
}


