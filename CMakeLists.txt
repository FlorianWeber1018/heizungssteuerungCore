cmake_minimum_required(VERSION 2.8)

project(heizungssteuerung)

add_executable(${PROJECT_NAME} "main.cpp" "mysqlcon.cpp" "clock.cpp" "IoD/IoD.cpp" "util.cpp" "REST/REST.cpp" "REST/eventhandler.cpp" "ModuleFramework/module.cpp" "ModuleFramework/control.cpp" "ModuleFramework/timer.cpp" "ModuleFramework/modulemanager.cpp")

target_link_libraries(${PROJECT_NAME} "boost_system" "pthread" "mariadbclient")



set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O3 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
