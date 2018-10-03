CROW_ROUTE(app, "/HardwareModule/value")
([]{
    pt::ptree tree;
    std::map<std::string, int> hardwareIoMap;
    globalIoD.getValues(hardwareIoMap, true, true);
    for(auto&& element : hardwareIoMap){
        tree.put(element.first, element.second);
    }
    std::stringstream ss;
    pt::json_parser::write_json(ss, tree);
    return EchoJSON(ss.str());
});
#include "io/index.cpp"
#include "adc/index.cpp"
