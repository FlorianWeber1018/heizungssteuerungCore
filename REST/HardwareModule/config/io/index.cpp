CROW_ROUTE(app, "/HardwareModule/config/io")
([]{
    pt::ptree tree;
    std::map<std::string, int> hardwareConfigMap;
    globalIoD.getConfigs(hardwareConfigMap, true, false);
    for(auto&& element : hardwareConfigMap){
        tree.put(element.first, element.second);
    }
    std::stringstream ss;
    pt::json_parser::write_json(ss, tree);
    return EchoJSON(ss.str());
});
CROW_ROUTE(app, "/HardwareModule/config/io/<int>")
([](int number){
    pt::ptree tree;
    std::map<std::string, int> hardwareConfigMap;
    globalIoD.getConfigs(hardwareConfigMap, true, false, number);
    for(auto&& element : hardwareConfigMap){
        tree.put(element.first, element.second);
    }
    std::stringstream ss;
    pt::json_parser::write_json(ss, tree);
    return EchoJSON(ss.str());
});
CROW_ROUTE(app, "/HardwareModule/config/io/<int>").methods("PUT"_method)
([](const crow::request& req, int number){
    std::string jsonStr = req.body;
    auto x = crow::json::load(jsonStr);
    if (!x){
        return crow::response(400);
    }
    unsigned int newConfig = static_cast<uint8_t>(x["config"].u());
    globalIoD.changeConfig('I', number, newConfig);
    return crow::response(200);
});
