#include "REST.h"
#include "crow_all.h"
#include "../IoD/IoD.h"
#include <string>

extern IoD::IoD globalIoD;

namespace REST{
    void restMain()
    {
        crow::SimpleApp app;
        #include "HardwareModule/index.cpp" //include HardwaremoduleHandlers

        //app.loglevel(crow::LogLevel::Warning);
        app.port(18080).multithreaded().run();
    }

    /*void convertToJson(crow::json::wvalue& outJson, const std::map<std::string&, int> inputMap)
    {
        for(auto&& element : inputMap){
            outJson[element.first] = element.second;
        }
    }*/
}


