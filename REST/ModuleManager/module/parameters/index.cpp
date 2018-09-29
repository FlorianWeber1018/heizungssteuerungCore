CROW_ROUTE(app, "/ModuleManager/module/parameters/<int>")
([](int moduleID){
    pt::ptree tree;
    Module::Module* module = globalModuleManager.getModule(moduleID);
    if(module != nullptr){
        const std::map<std::string, int>& paramMap = module->getAllParams();
        for(auto&& element : paramMap){
            tree.put(element.first, element.second);
        }
    }
    std::stringstream ss;
    pt::json_parser::write_json(ss, tree);
    return ss.str();
});
CROW_ROUTE(app, "/ModuleManager/module/parameters/<int>/<string>")
([](int moduleID, std::string paramKey){
    pt::ptree tree;
    Module::Module* module = globalModuleManager.getModule(moduleID);
    if(module != nullptr){
        const std::map<std::string, int>& paramMap = module->getAllParams();
        for(auto&& element : paramMap){
            if(element.first == paramKey){
                tree.put(paramKey, element.second);
            }
        }
    }
    std::stringstream ss;
    pt::json_parser::write_json(ss, tree);
    return ss.str();
});
CROW_ROUTE(app, "/ModuleManager/module/parameters/<int>/<string>").methods("PUT"_method)
([](const crow::request& req, int moduleID, std::string paramKey){
    std::string jsonStr = req.body;
    auto x = crow::json::load(jsonStr);
    if (!x){
        return crow::response(400);
    }
    Module::Module* module = globalModuleManager.getModule(moduleID);
    if(module != nullptr && x.has("parameter")){
        if(module->changeParam(paramKey, x["parameter"].i())){
           return crow::response(200);
        }
    }
    return crow::response(400);
});
