#include <string>
#include <map>
#include "IoD.h"
#include <thread>
#include <iostream>
#include <iomanip>
#include <boost/asio/serial_port.hpp>
#include <boost/asio.hpp>
#include <mysql/mysql.h>
#include "COMprotocol.h"
#include <functional>
#include "../util.h"
extern mSQL::mysqlcon globalSQLCon;

namespace IoD{
unsigned int Pin::get_number()
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    return number;
}
bool Pin::get_inUse()
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    return inUse;
}
bool Pin::get_valueSynced()
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    return valueSynced;
}
bool Pin::get_configSynced()
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    return configSynced;
}
int16_t Pin::get_value()
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    return value;
}
uint8_t Pin::get_config()
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    return config;
}
const Module::Signal& Pin::get_signal()
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    return signal;
}


void Pin::set_number(unsigned int _number)
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    number=_number;
}
void Pin::set_inUse(bool _inUse)
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    inUse=_inUse;
}
void Pin::set_valueSynced(bool _valueSynced)
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    valueSynced=_valueSynced;
}
void Pin::set_configSynced(bool _configSynced)
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    configSynced=_configSynced;
}
void Pin::set_value(int16_t _value)
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    value=_value;
}
void Pin::set_config(uint8_t _config)
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    config=_config;
}
void Pin::change_config(uint8_t _config)
{
    set_config(_config);
    set_configSynced(false);
}



IoPin::IoPin(unsigned int number = 0, unsigned char value = 0, IoConfig config = IoConfig::nInvInput)
{
    this->number = number;
    this->value = value;
    this->config = config;
}
IoPin& IoPin::operator =(const IoPin& other)
{
    if( this!= &other){
        std::lock(_mutex, other._mutex);
        std::lock_guard<std::mutex> lhs_lg(_mutex, std::adopt_lock);
        std::lock_guard<std::mutex> rhs_lg(other._mutex, std::adopt_lock);
        number = other.number;
        inUse = other.inUse;
        valueSynced = other.valueSynced;
        configSynced = other.configSynced;
        value = other.value;
        config = other.config;
        signal = other.signal;
        slot = other.slot;
    }
    return *this;
}
AdcPin::AdcPin(unsigned int number = 0, unsigned char value = 0, AdcConfig config = AdcConfig::pt2000_11)
{
    this->number = number;
    this->value = value;
    this->config = config;
}
AdcPin& AdcPin::operator =(const AdcPin& other)
{
    if( this!= &other){
        std::lock(_mutex, other._mutex);
        std::lock_guard<std::mutex> lhs_lg(_mutex, std::adopt_lock);
        std::lock_guard<std::mutex> rhs_lg(other._mutex, std::adopt_lock);
        number = other.number;
        inUse = other.inUse;
        valueSynced = other.valueSynced;
        configSynced = other.configSynced;
        value = other.value;
        config = other.config;
        signal = other.signal;
    }
    return *this;
}

serialCmdInterface::serialCmdInterface(std::string device, int baudrate) : ioService(), port(ioService)
{
	this->device=device;
	this->baudrate=baudrate;
	std::cout << "serialCmdInterface::serialCmdInterface" << std::endl;
}

serialCmdInterface::~serialCmdInterface()
{
	stopSending();
	stopListening();
	if(connectionEstablished){
		disconnect();
	}
}

void serialCmdInterface::run()
{
	if(connectionEstablished){
		startListening();
		startSending();
	}else{
		std::cout<<"run::noConnection!"<<std::endl;
	}
}
void serialCmdInterface::stop()
{
	stopSending();
	stopListening();
}

bool serialCmdInterface::getConnectionState()
{
	return connectionEstablished;
}

bool serialCmdInterface::serialFlush(std::string cmdstr)
{
	if (connectionEstablished)
	{
		cmdstr += eot;
		while(bufOut.size() >= SizeBufOutMax){
			std::cout<< "BUFFER FULL !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
        //std::cout << "now flushing:";
        //plotFlushStringToConsole(cmdstr);
        //std::cout<< std::endl;
		bufOut.push_back(cmdstr);
        flushCount++;
		return false;
	}
	else
	{
		std::cout<<"serialFlush::noConnection"<<std::endl;
		return true;
	}
	return true;
}

bool serialCmdInterface::connect()
{
	if(connectionEstablished){
		disconnect();
	}
	const char *_device = device.c_str();
	port.open(_device);

	if(port.is_open()){
        port.set_option( asio_serial::baud_rate(baudrate));
		port.set_option( asio_serial::flow_control( asio_serial::flow_control::none ) );
		port.set_option( asio_serial::parity( asio_serial::parity::none ) );
		port.set_option( asio_serial::stop_bits( asio_serial::stop_bits::one ) );
		port.set_option( asio_serial::character_size( 8 ) );
        //std::cout<< "option were configured" << std::endl;
		connectionEstablished=true;
		//rtr=true;
	}else{
		connectionEstablished=false;
		//rtr=false;
	}
	return connectionEstablished;
}

void serialCmdInterface::disconnect()
{
	if(connectionEstablished){
		port.close();
		connectionEstablished = false;
	}
}

void serialCmdInterface::serialDispatcher(std::string cmd)
{
    //std::cout<<"serialCmdInterface::serialDispatcher:"<<std::endl;
	plotFlushStringToConsole(cmd);
	std::cout << "-->";

	unsigned char cmdByte = cmd[0];
  if(cmd.length() < 1){
    return;
  }
	std::string port;
    std::string pin;
    std::string payloadStr = cmd.substr(1);
	std::string payload;

    if(cmd.length() == 3){
        payload = std::to_string(static_cast<unsigned char>(to_uint8_t(payloadStr)));
    }else if(cmd.length() == 5){
        payload = std::to_string(static_cast<short>(to_int16_t(payloadStr)));
    }
    if(cmdByte==cmdDebug0){
        std::cout << "debug0: "<< payload << std::endl;
    }
    if(cmdByte==cmdDebug1){
        std::cout << "debug1: "<< payload << std::endl;
    }
    if(cmdByte==cmdDebug2){
        std::cout << "debug2: "<< payload << std::endl;
    }
    if(cmdByte==cmdresetMCU){
        std::cout << "reset done" << std::endl;
    }
  if(cmdByte >= setVA0 && cmdByte <= setVA15){
    std::cout << "Set Value A";
    port =  std::to_string((cmdByte-setVA0)/8);
    pin = std::to_string((cmdByte-setVA0)%8);

		std::cout<<port<<"."<<pin<<" = " << payload << std::endl;
  }else{
    if(cmdByte >= setCA0 && cmdByte <= setCA15){
			std::cout << "Set Config A";
	    port =  std::to_string((cmdByte-setCA0)/8);
	    pin = std::to_string((cmdByte-setCA0)%8);

			std::cout<<port<<"."<<pin<<" = " << payload << std::endl;
    }else{
      if(cmdByte >= setVI0 && cmdByte <= setVI39){
				std::cout << "Set Value I";
		    port =  std::to_string((cmdByte-setVI0)/8);
		    pin = std::to_string((cmdByte-setVI0)%8);

				std::cout<<port<<"."<<pin<<" = " << payload << std::endl;
      }else{
        if(cmdByte >= setCI0 && cmdByte <= setCI39){
					std::cout << "Set Config I";
			    port =  std::to_string((cmdByte-setCI0)/8);
			    pin = std::to_string((cmdByte-setCI0)%8);

					std::cout<<port<<"."<<pin<<" = " << payload << std::endl;
        }
      }
    }
  }

}

void serialCmdInterface::stopListening()
{
	listenEnable=false;
}
void serialCmdInterface::stopSending()
{
	sendEnable=false;
}
void serialCmdInterface::startSending()
{
//	cout << "here should sending thread start" << endl;
	std::thread m_thread(&serialCmdInterface::Sending, this);
	m_thread.detach();

}
void serialCmdInterface::startListening()
{
//	cout << "here should listening thread start" << endl;
	std::thread m_thread(&serialCmdInterface::Listening, this);
	m_thread.detach();
}
void serialCmdInterface::Listening()
{
	//cout << "listen thread!" << endl;
	listenEnable=true;
	std::string tempIn;
	while (listenEnable) {

		char polledChar = 0;
        serialCmdInterface::pollOne(&polledChar);
        polledChar += number0;


		if(polledChar == eot){
			serialDispatcher(tempIn);
            recCount++;
            std::this_thread::sleep_for(std::chrono::microseconds(200));
			tempIn = "";
		}else{
			tempIn += polledChar;
		}
	}
}

void serialCmdInterface::Sending()
{
	//cout << "send thread!"<<endl;
	sendEnable=true;
	while (sendEnable) {

		if(!bufOut.empty()){
			std::string temp = bufOut.front();
			sendOne(temp);
			bufOut.pop_front();
            sendCount++;
			//}
		}else{
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
            //std::cout << "wait, bufout empty";
		}
	}
}

bool serialCmdInterface::sendOne(std::string m_string)
{
	int bytesToSend = m_string.length();
	for(int i = 0; i < bytesToSend; i++){
		m_string[i] -= number0;
	}
	if(connectionEstablished){
			boost::asio::write( port, boost::asio::buffer( m_string, bytesToSend ) );
	}else{

		return true;
	}
	return false;
}


int serialCmdInterface::pollOne(char* buffer)
{
	return port.read_some(boost::asio::buffer(buffer, 1));
}


std::string serialCmdInterface::to_flushString(int16_t number)
{
	std::string result = {1,1,1,1,0};  //nullterminated
	result[0] = (unsigned char)(   number & 0x000F ) + number0;
	result[1] = (unsigned char)( ( number >> 4 ) & 0x000F) + number0;
	result[2] = (unsigned char)( ( number >> 8 ) & 0x000F) + number0;
	result[3] = (unsigned char)( ( number >> 12 ) & 0x000F) + number0;
	return result;
}
std::string serialCmdInterface::to_flushString(uint8_t number)
{
	std::string result = "00";  //nullterminated
    result[0] = ( number & 0x0F ) + number0;
	result[1] = (( number >> 4 ) & 0x0F) + number0;
	return result;
}
std::string serialCmdInterface::to_flushString(IoConfig config)
{
    return to_flushString(static_cast<unsigned char>(config));
}
std::string serialCmdInterface::to_flushString(AdcConfig config)
{
    return to_flushString(static_cast<unsigned char>(config));
}
uint8_t serialCmdInterface::to_uint8_t(const std::string& flushString)
{
    uint8_t result = 0;
	result |= ( flushString[0] - number0 ) & 0x0F  ;
	result |= ( ( flushString[1] - number0 ) & 0x0F ) << 4;
	return result;
}
int16_t serialCmdInterface::to_int16_t(const std::string& flushString)
{
    int16_t result = 0;
	result |= ( flushString[0] - number0 ) & 0x0F  ;
	result |= ( ( flushString[1] - number0 ) & 0x0F ) << 4;
	result |= ( ( flushString[2] - number0 ) & 0x0F ) << 8;
	result |= ( ( flushString[3] - number0 ) & 0x0F ) << 12;
	return result;
}
const void serialCmdInterface::plotFlushStringToConsole(const std::string& flushString)
{
	for (std::string::size_type i = 0; i < flushString.length(); ++i){
		  std::cout << " " << std::hex << std::setfill('0') << std::setw(2) << std::nouppercase << (unsigned char)flushString[i] - 1;
	}
}




IoD::IoD(bool cyclicSend,
         unsigned int milliseconds,
         const std::string& device,
         unsigned int baudrate) : serialCmdInterface(device, baudrate)
{
    getDataFromSqlServer();

    serialCmdInterface::connect();
    serialCmdInterface::run();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    initMCU();

    if(cyclicSend){
        initClock(milliseconds);
    }


}
void IoD::initMCU()
{
    while(!mcuResetDone){
        resetMCU();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    cyclicSync();
}
void IoD::initClock(unsigned int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::seconds(2));
    m_clock = new Clock::Clock(std::chrono::milliseconds(milliseconds), std::bind(&IoD::cyclicSync,this) );
    std::thread t(&Clock::Clock::run, m_clock);//start clock async
    t.detach();
}
IoD::~IoD()
{
    if(m_clock != nullptr){
        m_clock->stop();
        delete m_clock;
    }

}
unsigned long int IoD::getRecCount()
{
    return recCount;
}
unsigned long int IoD::getSendCount()
{
    return sendCount;
}
unsigned long int IoD::getFlushCount()
{
    return flushCount;
}
unsigned long IoD::getClockCount()
{
    return clockCount;
}
void IoD::getDataFromSqlServer()
{
    std::string query = "SELECT portType, number, config, value FROM IoDPins";// ORDER BY portType, number;";
    MYSQL_RES *result = globalSQLCon.sendCommand(query);

    MYSQL_ROW row;
    if (result != nullptr) {
        unsigned int test = mysql_num_fields(result);
        while (row = mysql_fetch_row(result)) {
            std::string portType = row[0];
            int number = std::stoi(row[1]);
            int config = std::stoi(row[2]);
            int value = std::stoi(row[3]);
            if(portType == "I"){
                if( config == IoConfig::invInput ||
                    config == IoConfig::nInvInput)
                {//input
                    ioMapInput[number] = IoPin(number, value, static_cast<IoConfig>(config) );
                }else if( config == IoConfig::invOutput ||
                          config == IoConfig::nInvOutput ||
                          config == IoConfig::invPwmOutput ||
                          config == IoConfig::nInvPwmOutput)
                {//output
                    ioMapOutput[number] = IoPin(number, value, static_cast<IoConfig>(config) );
                }
            }else if(portType == "A"){
                adcMap[number] = AdcPin(number, value, static_cast<AdcConfig>(config) );
            }
        }
        mysql_free_result(result);
      }
}
void IoD::changeConfigOnSqlServer(char portType, int number, uint8_t newConfig)
{
    std::string query = "UPDATE IoDPins SET config = ";
    query.append(std::to_string(newConfig));
    query.append(" WHERE portType = '");
    query += portType;
    query.append("' AND number = ");
    query.append(std::to_string(number));
    query.append(" ;");

    MYSQL_RES *result = globalSQLCon.sendCommand(query);

    if (result != nullptr) {
        mysql_free_result(result);
    }
}
void IoD::readInputs(bool readUnusedToo){

    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);

    for(auto&& element : adcMap){
        if( element.second.get_inUse() || readUnusedToo ){
            element.second.set_valueSynced(false);
            std::string flushStr = "0";
            flushStr[0] = getVA0 + element.second.get_number();
            serialFlush(flushStr);
        }
    }
    for(auto&& element : ioMapInput){
        if( element.second.get_inUse() || readUnusedToo ){
            element.second.set_valueSynced(false);
            std::string flushStr = "0";
            flushStr[0] = getVI0 + element.second.get_number();
            serialFlush(flushStr);
        }
    }
}
void IoD::writeOutputs(bool writeAll, bool writeUnusedToo){

    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);

    for ( auto&& element : ioMapOutput ){
        if( ( element.second.get_inUse() || writeUnusedToo ) && ( !element.second.get_valueSynced() || writeAll ) ){
            std::string flushStr = "0";
            flushStr[0] = setVI0 + element.second.get_number();
            flushStr += to_flushString(static_cast<uint8_t>(element.second.get_value()));
            serialFlush(flushStr);
        }
    }
}
void IoD::writeConfig(bool writeAll, bool writeUnusedToo){

    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);

    for ( auto&& element : ioMapInput ){
        if( ( element.second.get_inUse() || writeUnusedToo ) && ( !element.second.get_configSynced() || writeAll ) ){
            std::string flushStr = "0";
            flushStr[0] = setCI0 + element.second.get_number();
            flushStr += to_flushString(element.second.get_config());
            serialFlush(flushStr);
        }
    }
    for ( auto&& element : ioMapOutput ){
        if( ( element.second.get_inUse() || writeUnusedToo ) && ( !element.second.get_configSynced() || writeAll ) ){
            std::string flushStr = "0";
            flushStr[0] = setCI0 + element.second.get_number();
            flushStr += to_flushString(element.second.get_config());
            serialFlush(flushStr);
        }
    }
    for ( auto&& element : adcMap ){
        if( ( element.second.get_inUse() || writeUnusedToo ) && ( !element.second.get_configSynced() || writeAll ) ){
            std::string flushStr = "0";
            flushStr[0] = setCA0 + element.second.get_number();
            flushStr += to_flushString(element.second.get_config());
            serialFlush(flushStr);
        }
    }
}
void IoD::resetMCU()
{
    std::string flushStr = "0";
    flushStr[0] = cmdresetMCU;
    serialFlush(flushStr);
}
void IoD::cyclicSync()
{


    writeConfig(true, true);
    readInputs(true);
    writeOutputs(true, true);
    clockCount++;
}
void IoD::serialDispatcher(std::string cmd)
{


    //serialCmdInterface::serialDispatcher(cmd);



    uint8_t cmdByte = cmd[0];
    if(cmd.length() < 1){
        return;
    }
    uint8_t number = 0;
    std::string payloadStr = cmd.substr(1);
    if(cmd.length() == 5){

    }else if(cmd.length() == 3){

    }

    Pin* pin = nullptr;

    if(cmdByte >= setVA0 && cmdByte <= setVA15){
        if(cmd.length() == 5){
            number = cmdByte - setVA0;
            pin = &adcMap[number];
            pin->set_value(to_int16_t(payloadStr));
            pin->set_valueSynced(true);
        }
    }else if(cmdByte >= setCA0 && cmdByte <= setCA15){
        if(cmd.length() == 3){
            number = cmdByte - setCA0;
            pin = &adcMap[number];
            if(pin->get_config() == to_uint8_t(payloadStr)){
                pin->set_configSynced(true);
            }
        }
    }else if(cmdByte >= setVI0 && cmdByte <= setVI39){
        if(cmd.length() == 3){
            number = cmdByte - setVI0;
            uint8_t value = to_uint8_t(payloadStr);
            if(ioMapInput.find(number) != ioMapInput.end()){
                pin = &ioMapInput[number];
                pin->set_value(static_cast<int16_t>(value));
                pin->set_valueSynced(true);
            }else if(ioMapOutput.find(number) != ioMapOutput.end()){
                pin = &ioMapOutput[number];
                if(pin->get_value() == value){
                    pin->set_valueSynced(true);
                }
            }

        }
    }else if(cmdByte >= setCI0 && cmdByte <= setCI39){
        if(cmd.length() == 3){
            number = cmdByte - setCI0;
            uint8_t config = to_uint8_t(payloadStr);
            if(ioMapInput.find(number) != ioMapInput.end()){
                pin = &ioMapInput[number];
            }else if(ioMapOutput.find(number) != ioMapOutput.end()){
                pin = &ioMapOutput[number];
            }
            if(pin->get_config() == config){
                pin->set_configSynced(true);
            }
        }
    }else if(cmdByte == cmdresetMCU){
        mcuResetDone=true;
    }
}
bool IoD::allInputValuesSynced()
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);

    bool result = true;
    for(auto&& element : ioMapInput){
        result &= element.second.get_valueSynced();
    }
    for(auto&& element : adcMap){
        result &= element.second.get_valueSynced();
    }
    return result;
}
bool IoD::allOutputValuesSynced()
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);

    bool result = true;
    for(auto&& element : ioMapOutput){
        result &= element.second.get_valueSynced();
    }
    return result;
}
void IoD::test()
{
            /*std::string flushStr = "000";
            flushStr[0] = setCI0 + 16;
            flushStr[1] = 5;
            flushStr[2] = 1;
            serialFlush(flushStr);
            flushStr[0] = setVI0 + 16;
            flushStr[1] = 0x10;
            flushStr[2] = 0x08;
            serialFlush(flushStr);

    writeConfig(true, true);
    writeOutputs(true, true);*/

}

void IoD::changeConfig(char portType, int number, uint8_t newConfig)
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);


    AdcPin* _adc_p = nullptr;
    IoPin* _io_p = nullptr;
    bool found = false;
    char oldMap;
    if(portType == 'A' || portType == 'a'){
        found = util::searchInMap(adcMap, number, _adc_p );
        oldMap = 'A';
    }else if(portType == 'I' || portType == 'i'){
        if(!found){
            found = util::searchInMap(ioMapInput, number, _io_p );
            oldMap = 'I';
            if(!found){
                found = util::searchInMap(ioMapOutput, number, _io_p );
                oldMap = 'O';
            }
        }
    }
    if(found){

        if(portType == 'A' || portType == 'a'){//look for sorting maps new
            _adc_p->change_config(newConfig); ///CHANGE CONFIG IN PIN
            changeConfigOnSqlServer(portType, number, newConfig);
            //nothing to sort, a adcpin can not be an output
        }else if(portType == 'I' || portType == 'i'){
            _io_p->change_config(newConfig); ///CHANGE CONFIG IN PIN
            changeConfigOnSqlServer(portType, number, newConfig);
            char newMap = oldMap; //init useless, only safety
            if (newConfig == 0 || newConfig == 1 ){
                if(oldMap == 'O'){//new=input old=output
                    ioMapInput[number] = ioMapOutput[number];
                    ioMapOutput.erase(number);
                }
            }else if(newConfig == 2 || newConfig == 3 || newConfig == 4 || newConfig == 5 ){
                if(oldMap == 'I'){//new=output old=input
                    ioMapOutput[number] = ioMapInput[number];
                    ioMapInput.erase(number);
                }
            }
        }
    }
}
void IoD::getAllSignals(std::map<std::string, int>& outMap, bool io, bool adc, int number)
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    if(adc){
        for(auto&& element : adcMap){
            if(number == -1 || number == element.first){
                std::string signalName = "ADC_";
                signalName += std::to_string(element.first);
                outMap[signalName] = element.second.get_value();
            }
        }
    }
    if(io){
        for(auto&& element : ioMapInput){
            if(number == -1 || number == element.first){
                std::string signalName = "IN_";
                signalName += std::to_string(element.first);
                outMap[signalName] = element.second.get_value();
            }
        }
        for(auto&& element : ioMapOutput){
            if(number == -1 || number == element.first){
                std::string signalName = "OUT_";
                signalName += std::to_string(element.first);
                outMap[signalName] = element.second.get_value();
            }
        }
    }
}
void IoD::getAllConfigs(std::map<std::string, int>& outMap, bool io, bool adc, int number)
{
    _mutex.lock();
    std::lock_guard<std::mutex> lg(_mutex, std::adopt_lock);
    if(adc){
        for(auto&& element : adcMap){
            if(number == -1 || number == element.first){
                std::string signalName = "ADC_";
                signalName += std::to_string(element.first);
                outMap[signalName] = element.second.get_config();
            }
        }
    }
    if(io){
        for(auto&& element : ioMapInput){
            if(number == -1 || number == element.first){
                std::string signalName = "IN_";
                signalName += std::to_string(element.first);
                outMap[signalName] = element.second.get_config();
            }
        }
        for(auto&& element : ioMapOutput){
            if(number == -1 || number == element.first){
                std::string signalName = "OUT_";
                signalName += std::to_string(element.first);
                outMap[signalName] = element.second.get_config();
            }
        }
    }
}
}
