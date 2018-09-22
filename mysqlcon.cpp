#include "mysqlcon.h"
#include <string>
#include <iostream>
#include <mysql/mysql.h>
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
		this->connected = true;
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
	if(connected){
        return sendCommand(sendstring, m_mysql);
	}
	return NULL;
}
}
