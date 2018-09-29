CROW_ROUTE(app, "/HardwareModule/value/adc")
([]{
    crow::json::wvalue outValue;
    std::map<std::string, int> hardwareIoMap;
    globalIoD.getValues(hardwareIoMap, false, true);
    for(auto&& element : hardwareIoMap){
        outValue[element.first] = element.second;
    }
    return outValue;
});
CROW_ROUTE(app, "/HardwareModule/value/adc/<int>")
([](int number){
    crow::json::wvalue outValue;
    std::map<std::string, int> hardwareIoMap;
    globalIoD.getValues(hardwareIoMap, false, true, number);
    for(auto&& element : hardwareIoMap){
        outValue[element.first] = element.second;
    }
    return outValue;
});
