CROW_ROUTE(app, "/HardwareModule")
([]{
    pt::ptree tree;

    std::map<std::string, int> hardwareValueMap;
    globalIoD.getValues(hardwareValueMap, true, true);

    for(auto&& element : hardwareValueMap){
        std::string key = element.first;
        key.append("value");
        tree.put(key, element.second);
    }

    std::map<std::string, int> hardwareConfigMap;
    globalIoD.getConfigs(hardwareConfigMap, true, true);

    for(auto&& element : hardwareConfigMap){
        std::string key = element.first;
        key.append("config");
        tree.put(key, element.second);
    }
    std::stringstream ss;
    pt::json_parser::write_json(ss, tree);
    return EchoJSON(ss.str());
});
#include "value/index.cpp"
#include "config/index.cpp"
