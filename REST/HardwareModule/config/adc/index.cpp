CROW_ROUTE(app, "/HardwareModule/config/adc")
([]{
    crow::json::wvalue outValue;
    std::map<std::string, int> hardwareConfigMap;
    globalIoD.getAllConfigs(hardwareConfigMap, false, true);
    for(auto&& element : hardwareConfigMap){
        outValue[element.first] = element.second;
    }
    return outValue;
});
CROW_ROUTE(app, "/HardwareModule/config/adc/<int>")
([](int number){
    crow::json::wvalue outValue;
    std::map<std::string, int> hardwareConfigMap;
    globalIoD.getAllConfigs(hardwareConfigMap, false, true, number);
    for(auto&& element : hardwareConfigMap){
        outValue[element.first] = element.second;
    }
    return outValue;
});
CROW_ROUTE(app, "/HardwareModule/config/adc/<int>").methods("PUT"_method)
([](const crow::request& req, int number){
    std::string jsonStr = req.body;
    auto x = crow::json::load(jsonStr);
    if (!x){
        return crow::response(400);
    }
    unsigned int newConfig = static_cast<uint8_t>(x["config"].u());
    globalIoD.changeConfig('A', number, newConfig);
    return crow::response(200);
});
