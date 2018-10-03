CROW_ROUTE(app, "/HardwareModule/value/adc")
([]{
    pt::ptree tree;
    std::map<std::string, int> hardwareIoMap;
    globalIoD.getValues(hardwareIoMap, false, true);
    for(auto&& element : hardwareIoMap){
        tree.put(element.first, element.second);
    }
    std::stringstream ss;
    pt::json_parser::write_json(ss, tree);
    return EchoJSON(ss.str());
});
CROW_ROUTE(app, "/HardwareModule/value/adc/<int>")
([](int number){
    pt::ptree tree;
    std::map<std::string, int> hardwareIoMap;
    globalIoD.getValues(hardwareIoMap, false, true, number);
    for(auto&& element : hardwareIoMap){
        tree.put(element.first, element.second);
    }
    std::stringstream ss;
    pt::json_parser::write_json(ss, tree);
    return EchoJSON(ss.str());
});
