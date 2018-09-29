CROW_ROUTE(app, "/HardwareModule/config")
([]{
    crow::json::wvalue outValue;
    std::map<std::string, int> hardwareConfigMap;
    globalIoD.getConfigs(hardwareConfigMap, true, true);
    for(auto&& element : hardwareConfigMap){
        outValue[element.first] = element.second;
    }
    return outValue;
});
#include "io/index.cpp"
#include "adc/index.cpp"
