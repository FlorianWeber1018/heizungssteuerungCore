CROW_ROUTE(app, "/ModuleManager/module")
([]{
    pt::ptree tree;

    std::map<unsigned int, Module::Module*> moduleMap;
    moduleMap = globalModuleManager.getAllModules();
    for(auto&& element : moduleMap){
        unsigned int moduleID = element.first;
        std::string moduleType = element.second->getModuleType();
        std::string keyToModule = std::to_string(moduleID);

        std::string keyToModuleID = keyToModule;
        keyToModuleID += ".ID";
        tree.put(keyToModuleID, moduleID);

        std::string keyToModuleType = keyToModule;
        keyToModuleType += ".Type";
        tree.put(keyToModuleType, moduleType);

        std::string keyToParams = keyToModule;
        keyToParams += ".Parameters";

        std::string keyToSignals = keyToModule;
        keyToSignals += ".Signals";

        std::string keyToSlots = keyToModule;
        keyToSlots += ".Slot";

        const std::map<std::string, int>& paramMap = element.second->getAllParams();
        const std::map<std::string, Module::Signal*>& signalMap = element.second->getAllSignals();
        const std::map<std::string, Module::Slot*>& slotMap = element.second->getAllSlots();
        for(auto&& element : paramMap){
            std::string keyToParam = keyToParams;
            keyToParam += ".";
            keyToParam += element.first;
            tree.put(keyToParam, element.second);
        }
        for(auto&& element : signalMap){
            std::string keyToSignal = keyToSignals;
            keyToSignal += ".";
            keyToSignal += element.first;
            std::string keyToSignalValue = keyToSignal;
            keyToSignalValue += ".value";
            tree.put(keyToSignalValue, element.second->value);
            std::string keyToSignalSlots = keyToSignal;
            keyToSignalSlots += ".connectedSlots";
            for(auto&& slotElement : element.second->m_slots){
              unsigned int ModuleID = slotElement->m_parentModule == nullptr ? 0 : slotElement->m_parentModule->ID;
              tree.add(keyToSignalSlots, ModuleID);
            }

        }
        /*for(auto&& element : slotMap){
            std::string keyToSlot = keyToSlots;
            keyToSlot += ".";
            keyToSlot += element.first;
            tree.put(keyToSlot, element.second);
        }*/
    }
    std::stringstream ss;
    pt::json_parser::write_json(ss, tree);
    return ss.str();
});
CROW_ROUTE(app, "/ModuleManager/module").methods("POST"_method)
([](const crow::request& req){
    std::string jsonStr = req.body;
    auto x = crow::json::load(jsonStr);
    if (!x){
        return crow::response(400);
    }
    if( x.has("Type") ){
        if( x.has("ID") ){
            globalModuleManager.createModule( x["Type"].s(), x["ID"].u());
        }else{
            globalModuleManager.createModule( x["Type"].s());
        }
    } else {
        return crow::response(400);
    }
    return crow::response(200);
});
CROW_ROUTE(app, "/ModuleManager/module").methods("DELETE"_method)
([](const crow::request& req){
    std::string jsonStr = req.body;
    auto x = crow::json::load(jsonStr);
    if (!x){
        return crow::response(400);
    }
    if( x.has("ID") ){
        globalModuleManager.deleteModule(x["ID"].u());
    }else{
        return crow::response(400);
    }
    return crow::response(200);
});
#include "parameters/index.cpp"
