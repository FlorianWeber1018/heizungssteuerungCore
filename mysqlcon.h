#ifndef __mysqlcon_h
#define __mysqlcon_h
#include <string>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <mutex>
namespace mSQL{


class mysqlcon {

public:
	mysqlcon(std::string host, unsigned int port, std::string user,
		std::string pw, std::string db
	);
	std::mutex _mutex;
	~mysqlcon();
	bool connect();
	void disconnect();
	MYSQL_RES* sendCommand(std::string sendstring);
private:
	MYSQL_RES* sendCommand(std::string, MYSQL* connectionObject);
	MYSQL* m_mysql;
	bool connected;
	std::string host;
	unsigned int port;
	std::string user;
	std::string pw;
	std::string db;
};
}
#endif
