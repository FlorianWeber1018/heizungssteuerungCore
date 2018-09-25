CROW_ROUTE(app, "/ModuleManager/module")
([]{
    crow::json::wvalue outValue;
    std::map<unsigned int, Module::Module*> moduleMap;
    moduleMap = globalModuleManager.getAllModules();
    for(auto&& element : moduleMap){
        outValue[std::to_string(element.first)] = element.second->getModuleType();
    }
    return outValue;
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
