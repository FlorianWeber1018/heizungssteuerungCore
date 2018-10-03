#include <string>
#include "REST.h"
#include "crow_all.h"
#include "../IoD/IoD.h"
#include "../ModuleFramework/modulemanager.h"
#include "../ModuleFramework/module.h"
#include <vector>
#include <map>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

extern IoD::IoD globalIoD;
extern Module::ModuleManager globalModuleManager;

namespace REST{
    crow::response EchoJSON(const std::string& specialInformation)
    {
        // echo response
        crow::response resp;
        resp.code = 200;
        resp.add_header("Access-Control-Allow-Origin", "*");
        resp.add_header("Content-Type", "application/json");
        resp.write(specialInformation);
        return resp;
    }
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


