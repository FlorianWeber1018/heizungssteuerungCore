CROW_ROUTE(app, "/HardwareModule")
([]{
    crow::json::wvalue outValue;

    std::map<std::string, int> hardwareValueMap;
    globalIoD.getAllSignals(hardwareValueMap, true, true);

    for(auto&& element : hardwareValueMap){
        std::string key = element.first;
        key.append("value");
        outValue[key] = element.second;
    }

    std::map<std::string, int> hardwareConfigMap;
    globalIoD.getAllConfigs(hardwareConfigMap, true, true);

    for(auto&& element : hardwareConfigMap){
        std::string key = element.first;
        key.append("config");
        outValue[key] = element.second;
    }

    return outValue;
});
#include "value/index.cpp"
#include "config/index.cpp"
