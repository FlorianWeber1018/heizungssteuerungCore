#include "mysqlcon.h"
#include <string>
#include <iostream>
#include <mysql/mysql.h>
#include <mutex>
#include <thread>
namespace mSQL{
mysqlcon::mysqlcon(std::string host, unsigned int port, std::string user, std::string pw, std::string db)
{
	this->host = host;
	this->port = port;
	this->user = user;
	this->pw = pw;
	this->db = db;
	this->connected = false;
	std::cout << "mysqlcon::mysqlcon" << std::endl;
    while(connect() == false){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "mysqlcon::mysqlcon:: sucessfully Connected" << std::endl;
}
mysqlcon::~mysqlcon() {
	disconnect();
}
bool mysqlcon::connect()
{
	//initialize MYSQL object for connections
	m_mysql = mysql_init(NULL);
	if (m_mysql == NULL)
	{
		return false;
	}

	//Connect to the database
	if (mysql_real_connect(m_mysql, host.c_str(), user.c_str(), pw.c_str(), db.c_str(), port, NULL, 0) == NULL)
	{
        this->connected = true;//wird in disconnect false
		this->disconnect();
		return false;
	}
	else
	{
		this->connected = true;
		return true;
	}
}
void mysqlcon::disconnect()
{
	if(this->connected){
        mysql_close(m_mysql);
		this->connected = false;
	}
}
MYSQL_RES* mysqlcon::sendCommand(std::string sendstring, MYSQL* connectionObject)
{
	if (!mysql_query(connectionObject, sendstring.c_str()))
	{
		//std::cout << "got recordset" << std::endl;
		return mysql_store_result(connectionObject);
	}
	//std::cout << "no recordset" << std::endl;
	return NULL;
}
MYSQL_RES* mysqlcon::sendCommand(std::string sendstring)
{
	_mutex.lock();
	std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
	if(connected){
        return sendCommand(sendstring, m_mysql);
	}
    std::cout << "mysqlcon::sendCommand::noConnection" << std::endl;
	return NULL;
}
bool mysqlcon::sendCUD(const std::string& sendstring)
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    if(connected){
        int ErrCode = mysql_query(m_mysql, sendstring.c_str());
        if(ErrCode){
            printError(ErrCode);
            return false;
        }else{
            return true;
        }
    }
}
unsigned int mysqlcon::getAffectedRows(){
    return mysql_affected_rows(m_mysql);
}
void mysqlcon::printError(int ErrCode){
    switch(ErrCode){
    case 0:{

    }break;
    case 2014:{
        std::cout << "mysqlcon::sendCommand::printErr(ErrCode): " << "Commands were executed in an improper order" << std::endl;
    }break;
    case 2006:{
        std::cout << "mysqlcon::sendCommand::printErr(ErrCode): " << "MySQL server has gone away" << std::endl;
    }break;
    case 2013:{
        std::cout << "mysqlcon::sendCommand::printErr(ErrCode): " << "The connection to the server was lost during the query" << std::endl;
    }break;
    case 2000:{
        std::cout << "mysqlcon::sendCommand::printErr(ErrCode): " << "An unknown error occurred" << std::endl;
    }break;
    default:{
        std::cout << "mysqlcon::sendCommand::printErr(ErrCode): " << "IDontKnowErr?!?" << std::endl;
    }
    }
}
}
