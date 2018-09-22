#include "REST.h"
#include "crow_all.h"
#include "../IoD/IoD.h"

extern IoD::IoD globalIoD;
namespace REST{
    void restMain()
    {
        crow::SimpleApp app;
        CROW_ROUTE(app, "/HardwareModule")
        ([]{
            crow::json::wvalue outValue;
            std::map<std::string, int> hardwareIoMap;
            globalIoD.getAllSignals(hardwareIoMap);
            for(auto&& element : hardwareIoMap){
                outValue[element.first] = element.second;
            }

            return outValue;
        });

        app.port(18080)
                .multithreaded()
                .run();
    }

    /*void convertToJson(crow::json::wvalue& outJson, const std::map<std::string&, int> inputMap)
    {
        for(auto&& element : inputMap){
            outJson[element.first] = element.second;
        }
    }*/
}


