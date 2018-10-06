CROW_ROUTE(app, "/HardwareModule/value/io")
([]{
    pt::ptree tree;
    std::map<std::string, int> hardwareIoMap;
    globalIoD.getValues(hardwareIoMap, true, false);
    for(auto&& element : hardwareIoMap){
        tree.put(element.first, element.second);
    }
    std::stringstream ss;
    pt::json_parser::write_json(ss, tree);
    return EchoJSONSuccessful(ss.str());
});
CROW_ROUTE(app, "/HardwareModule/value/io/<int>")
([](int number){
    pt::ptree tree;
    std::map<std::string, int> hardwareIoMap;
    globalIoD.getValues(hardwareIoMap, true, false, number);
    for(auto&& element : hardwareIoMap){
        tree.put(element.first, element.second);
    }
    std::stringstream ss;
    pt::json_parser::write_json(ss, tree);
    return EchoJSONSuccessful(ss.str());
});
