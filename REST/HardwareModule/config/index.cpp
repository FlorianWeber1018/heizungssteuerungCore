CROW_ROUTE(app, "/HardwareModule/config")
([]{
    pt::ptree tree;
    std::map<std::string, int> hardwareConfigMap;
    globalIoD.getConfigs(hardwareConfigMap, true, true);
    for(auto&& element : hardwareConfigMap){
        tree.put(element.first, element.second);
    }
    std::stringstream ss;
    pt::json_parser::write_json(ss, tree);
    return EchoJSONSuccessful(ss.str());
});
#include "io/index.cpp"
#include "adc/index.cpp"
