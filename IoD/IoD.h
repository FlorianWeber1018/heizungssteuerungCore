#ifndef __IoD_h
#define __IoD_h

#include <boost/asio/serial_port.hpp>
#include <boost/asio.hpp>
#include <string>
#include <list>
#include "COMprotocol.h"
#include <thread>
#include <mutex>
#include "../mysqlcon.h"
#include "../ModuleFramework/module.h"
#include "../clock.h"

#ifndef asio_serial
#define asio_serial boost::asio::serial_port_base
#endif



namespace IoD{
enum IoConfig{
	nInvInput, invInput, nInvOutput, invOutput, nInvPwmOutput, invPwmOutput
};
enum AdcConfig{
	pt1000_00, pt1000_10, pt1000_01, pt1000_11, pt2000_00, pt2000_10, pt2000_01, pt2000_11
};
struct Pin{
    //getter
    unsigned int get_number();
    bool get_inUse();
    bool get_valueSynced();
    bool get_configSynced();
    int16_t get_value();
    uint8_t get_config();
    const Module::Signal& get_signal();
    //setter
    void set_number(unsigned int _number);
    void set_inUse(bool _inUse);
    void set_valueSynced(bool _valueSynced);
    void set_configSynced(bool _configSynced);
    void set_value(int16_t _value);
    void set_config(uint8_t _config);

protected:
    mutable std::mutex _mutex;
    unsigned int number;
    bool inUse = false;
    bool valueSynced = false;
    bool configSynced = false;
    int16_t value = 0;
    uint8_t config = 0;
    Module::Signal signal;

};
struct IoPin : public Pin
{
    IoPin(unsigned int number, unsigned char value, IoConfig config);
    IoPin& operator =(const IoPin& other);


    Module::Slot slot;
};
struct AdcPin : public Pin
{
    AdcPin(unsigned int number, unsigned char value, AdcConfig config);
    AdcPin& operator =(const AdcPin& other);
};
class serialCmdInterface
{
	public:
		serialCmdInterface(std::string device, int baudrate);
		~serialCmdInterface();
		bool getConnectionState();
		bool serialFlush(std::string cmd);
		bool connect();
		void disconnect();
		void run();
		void stop();
		//debug Function
		const void plotFlushStringToConsole(const std::string& flushString);
	protected:
        unsigned long recCount = 0;
        unsigned long sendCount = 0;
        unsigned long flushCount = 0;
		void startListening();
		void startSending();
		void stopSending();
		void stopListening();
		virtual void serialDispatcher(std::string cmd);
		void Sending();
		void Listening();

		std::string device;
		int baudrate;
		bool sendEnable;
		bool listenEnable;
		bool connectionEstablished;
		boost::asio::io_service ioService;
		boost::asio::serial_port port;
		int pollOne(char* buffer);
		bool sendOne(std::string m_string);
		std::list<std::string> bufOut;
		std::list<std::string> bufIn;
		volatile bool rtr;
		//convert Functions especialy for the Protocol
        std::string to_flushString(int16_t number);
        std::string to_flushString(uint8_t number);
        std::string to_flushString(IoConfig config);
        std::string to_flushString(AdcConfig config);
        uint8_t to_uint8_t(const std::string& flushString);
        int16_t to_int16_t(const std::string& flushString);
};
class IoD : protected serialCmdInterface{
public:
    IoD(bool cyclicSend,
        const std::string& device,
        unsigned int baudrate);
    ~IoD();
    unsigned long getRecCount();
    unsigned long getSendCount();
    unsigned long getFlushCount();
    unsigned long getClockCount();
    void getDataFromSqlServer();
    void readInputs(bool readUnusedToo);
    void writeOutputs(bool writeAll, bool writeUnusedToo);
    void writeConfig(bool writeAll, bool writeUnusedToo);
    void resetMCU();
    void cyclicSync();
    void mainloop();
    void test();
    void getAllSignals(std::map<std::string, int>& outMap);
    void getAllConfigs(std::map<std::string, int>& outMap);

    Module::Signal* getSignal(std::string SignalName);
    Module::Slot* getSlot(std::string SignalName);
protected:
    void initMCU();
    void initClock();
    bool mcuResetDone=false;
    unsigned long clockCount = 0;
    Clock::Clock* m_clock = nullptr;
    void serialDispatcher(std::string cmd) override;
    bool allInputValuesSynced();
    bool allOutputValuesSynced();
    std::map<int,IoPin> ioMapInput;
    std::map<int,IoPin> ioMapOutput;
    std::map<int,AdcPin> adcMap;
};
}


#endif
