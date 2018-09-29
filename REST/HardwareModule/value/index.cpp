CROW_ROUTE(app, "/HardwareModule/value")
([]{
    crow::json::wvalue outValue;
    std::map<std::string, int> hardwareIoMap;
    globalIoD.getValues(hardwareIoMap, true, true);
    for(auto&& element : hardwareIoMap){
        outValue[element.first] = element.second;
    }
    return outValue;
});
#include "io/index.cpp"
#include "adc/index.cpp"
