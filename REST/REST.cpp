#include <string>
#include "REST.h"
#include "crow_all.h"
#include "../IoD/IoD.h"
#include "../ModuleFramework/modulemanager.h"
#include "../ModuleFramework/module.h"
#include "../mysqlcon.h"

#include <vector>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "eventhandler.h"
namespace pt = boost::property_tree;

extern IoD::IoD globalIoD;
extern Module::ModuleManager globalModuleManager;
extern mSQL::mysqlcon globalSQLCon;

namespace REST{

    crow::response EchoJSONSuccessful(const std::string& specialInformation)
    {
        // echo response
        crow::response resp;
        resp.code = 200;
        resp.add_header("Access-Control-Allow-Origin", "*");
        resp.add_header("Content-Type", "application/json");
        resp.write(specialInformation);
        return resp;
    }
    crow::response responseOPTIONS()
    {
        crow::response resp;
        resp.code = 200;
        resp.add_header("Access-Control-Allow-Origin", "*");
        resp.add_header("Access-Control-Allow-Headers", "*");
        resp.add_header("Access-Control-Allow-Methods", "*");
        resp.add_header("Content-Type", "application/json");
        resp.write("");
        return resp;
    }
    crow::response EchoJSONFailed(const std::string& specialInformation)
    {
        // echo response
        crow::response resp;
        resp.code = 400;
        resp.add_header("Access-Control-Allow-Origin", "*");
        resp.add_header("Content-Type", "application/json");
        resp.write(specialInformation);
        return resp;
    }
    crow::response EchoJSONEventResult(bool success)
    {
        crow::response resp;
        if(success){
            resp.code = 200;
            resp.write("{\"result\" : \"success\"}");
        }else{
            resp.code = 400;
            resp.write("{\"result\" : \"fail\"}");
        }
        resp.add_header("Access-Control-Allow-Origin", "*");
        resp.add_header("Content-Type", "application/json");
        return resp;
    }
    void restMain()
    {

        crow::SimpleApp app;
        CROW_ROUTE(app, "/ModuleManager/modules/<uint>")
        ([](unsigned int moduleID)
        {
            Module::Module* module = globalModuleManager.getModule(moduleID);
            if(module!=nullptr){
                pt::ptree resTree = module->getProperties();
                std::stringstream ss;
                pt::json_parser::write_json(ss, resTree);
                return EchoJSONSuccessful( ss.str() );
            }else{
                return EchoJSONFailed( "" );
            }

        });
        CROW_ROUTE(app, "/ModuleManager")
        ([]()
        {
            pt::ptree resTree = globalModuleManager.getProperties();
            std::stringstream ss;
            pt::json_parser::write_json(ss, resTree);
            return EchoJSONSuccessful( ss.str() );
        });
        CROW_ROUTE(app, "/EVENT")
                .methods("OPTIONS"_method)
        ([]()
        {
            return responseOPTIONS();
        });
        CROW_ROUTE(app, "/EVENT")
                .methods("POST"_method)
        ([](const crow::request& req)
        {
            pt::ptree requestTree;
            std::stringstream ss;
            ss << req.body;
            pt::read_json(ss, requestTree);
            if( requestTree.count("TYPE") > 0 && requestTree.count("PASSEDPARAMS") > 0 ){
                std::string eventType = requestTree.get("TYPE", "TYPE NOT FOUND");
                pt::ptree passedParams = requestTree.get_child("PASSEDPARAMS", pt::ptree());
                bool result = false;
                if(eventType == "ButtonClick"){
                    result = EventHandler::POST_ButtonClick(passedParams);
                }else if(eventType == "UpdateModuleParam"){
                    result = EventHandler::POST_UpdateModuleParam(passedParams);
                }
                return EchoJSONEventResult(result);
            }else{
                return EchoJSONEventResult(false);//TYPE not Found
            }

            return EchoJSONEventResult(true);
        });
        CROW_ROUTE(app, "/CMS")
                .methods("OPTIONS"_method)
        ([]()
        {
            return responseOPTIONS();
        });
        CROW_ROUTE(app, "/CMS")
        ([]()
        {
            pt::ptree resTree;

            pt::ptree PagesTree;
            std::string query = "select * from CMSPages";
            MYSQL_RES *result = globalSQLCon.sendCommand(query);

            if (result != nullptr) {
                PagesTree = globalSQLCon.MYSQL_RES_to_ptree(result, 0);
                mysql_free_result(result);
                result = nullptr;
            }

            query = "select * from CMSPipes";
            result = globalSQLCon.sendCommand(query);
            if (result != nullptr) {
                while (MYSQL_ROW row = mysql_fetch_row(result)) {
                    std::string key = row[1];   // "row[1]" should be the page_id !!!
                    key.append(".pipes.");
                    key.append(row[0]);         // "row[0]" should be the id !!!
                    pt::ptree pipeTree;
                    for (uint i = 0; i<result->field_count;i++){
                       pipeTree.put(result->fields[i].name, row[i]);
                    }
                    PagesTree.add_child(key, pipeTree);
                }

                mysql_free_result(result);
            }

            query = "select * from CMSPumps";
            result = globalSQLCon.sendCommand(query);
            if (result != nullptr) {
                while (MYSQL_ROW row = mysql_fetch_row(result)) {
                    std::string key = row[1];   // "row[1]" should be the page_id !!!
                    key.append(".pumps.");
                    key.append(row[0]);         // "row[0]" should be the id !!!
                    pt::ptree pumpTree;
                    for (uint i = 0; i<result->field_count;i++){
                       pumpTree.put(result->fields[i].name, row[i]);
                    }
                    PagesTree.add_child(key, pumpTree);
                }

                mysql_free_result(result);
            }

            resTree.add_child("pages", PagesTree);

            std::stringstream ss;
            pt::json_parser::write_json(ss, resTree);
            return EchoJSONSuccessful( ss.str() );
        });
        #include "HardwareModule/index.cpp" //include HardwaremoduleHandlers
        #include "ModuleManager/index.cpp"  //include ModulemanagerHandlers



        app.loglevel(crow::LogLevel::Warning);
        app.port(18080).multithreaded().run();
    }
}


