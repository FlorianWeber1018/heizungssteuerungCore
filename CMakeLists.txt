cmake_minimum_required(VERSION 2.8)

project(heizungssteuerung)

add_executable(${PROJECT_NAME} "main.cpp" "mysqlcon.cpp" "clock.cpp" "IoD/IoD.cpp" "REST/REST.cpp" "ModuleFramework/module.cpp" "ModuleFramework/control.cpp" "ModuleFramework/timer.cpp")

target_link_libraries(${PROJECT_NAME} "boost_system" "pthread" "mysqlclient")



set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)


