g++ -g -std=c++17 main.cpp clock.cpp IoD/IoD.cpp mysqlcon.cpp util.cpp ModuleFramework/control.cpp ModuleFramework/module.cpp ModuleFramework/modulemanager.cpp ModuleFramework/timer.cpp REST/REST.cpp REST/eventhandler.cpp -o CORE -L /usr/local/lib -lboost_system -pthread -lmariadbclient
chmod +x CORE
