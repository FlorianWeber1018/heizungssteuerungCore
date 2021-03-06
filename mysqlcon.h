#ifndef __mysqlcon_h
#define __mysqlcon_h
#include <string>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <mutex>
#include <boost/property_tree/ptree_fwd.hpp>
namespace pt = boost::property_tree;

namespace mSQL{


class mysqlcon {

public:
	mysqlcon(std::string host, unsigned int port, std::string user,
		std::string pw, std::string db
	);
    std::string getTimeString();
	std::mutex _mutex;
	~mysqlcon();
	bool connect();
	void disconnect();
	MYSQL_RES* sendCommand(std::string sendstring);
    bool sendCUD(const std::string& sendstring);
    static pt::ptree MYSQL_RES_to_ptree(MYSQL_RES* resultset, unsigned int keyColNumber);
private:
	MYSQL_RES* sendCommand(std::string, MYSQL* connectionObject);
	MYSQL* m_mysql;
	bool connected;
	std::string host;
	unsigned int port;
	std::string user;
	std::string pw;
    std::string db;
    unsigned int getAffectedRows();
    void printError(int ErrCode);
};
}
#endif
