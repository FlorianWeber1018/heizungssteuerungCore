CROW_ROUTE(app, "/ModuleManager/connection")
([]{
            std::map<std::pair<unsigned int, std::string>,std::pair<unsigned int, std::string>> connectionMap;
            connectionMap = globalModuleManager.getAllConnections();
            pt::ptree tree;
            for(auto&& element : connectionMap){
                std::string keyToConnection = std::to_string(element.first.first);
                keyToConnection += ".";
                keyToConnection += element.first.second;

                std::string keyToDestID = keyToConnection;
                keyToDestID += ".destModuleID";
                std::string keyToDestSlotName = keyToConnection;
                keyToDestSlotName += ".destSlotName";
                std::string keyToSrcID = keyToConnection;
                keyToSrcID += ".srcModuleID";
                std::string keyToSrcSignalName = keyToConnection;
                keyToSrcSignalName += ".srcSignalName";

                tree.put(keyToDestID,element.first.first);
                tree.put(keyToDestSlotName, element.first.second);
                tree.put(keyToSrcID ,element.second.first);
                tree.put(keyToSrcSignalName, element.second.second);
            }
            std::stringstream ss;
            pt::json_parser::write_json(ss, tree);
            return ss.str();
});
CROW_ROUTE(app, "/ModuleManager/connection").methods("POST"_method)
        ([](const crow::request& req){
            std::string jsonStr = req.body;
            auto x = crow::json::load(jsonStr);
            if (!x){
                return crow::response(400);
            }
            if( x.has("destModuleID") && x.has("destSlotName") && x.has("srcModuleID") && x.has("srcSignalName") ){
                unsigned int destID = x["destModuleID"].u();
                std::string destSlotName = x["destSlotName"].s();
                unsigned int srcID = x["srcModuleID"].u();
                std::string srcSignalName = x["srcSignalName"].s();

                globalModuleManager.makeConnection(destID, destSlotName, srcID, srcSignalName);

                return crow::response(200);
            } else {
                return crow::response(400);
            }
});
CROW_ROUTE(app, "/ModuleManager/connection").methods("DELETE"_method)
        ([](const crow::request& req){
            std::string jsonStr = req.body;
            auto x = crow::json::load(jsonStr);
            if (!x){
                return crow::response(400);
            }
            if( x.has("destModuleID") && x.has("destSlotName") ){
                unsigned int destID = x["destModuleID"].u();
                std::string destSlotName = x["destSlotName"].s();

                globalModuleManager.deleteConnection(destID, destSlotName);

                return crow::response(200);
            } else {
                return crow::response(400);
            }
});
