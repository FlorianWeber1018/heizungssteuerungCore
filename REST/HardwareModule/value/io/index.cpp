CROW_ROUTE(app, "/HardwareModule/value/io")
([]{
    crow::json::wvalue outValue;
    std::map<std::string, int> hardwareIoMap;
    globalIoD.getAllSignals(hardwareIoMap, true, false);
    for(auto&& element : hardwareIoMap){
        outValue[element.first] = element.second;
    }
    return outValue;
});
CROW_ROUTE(app, "/HardwareModule/value/io/<int>")
([](int number){
    crow::json::wvalue outValue;
    std::map<std::string, int> hardwareIoMap;
    globalIoD.getAllSignals(hardwareIoMap, true, false, number);
    for(auto&& element : hardwareIoMap){
        outValue[element.first] = element.second;
    }
    return outValue;
});
