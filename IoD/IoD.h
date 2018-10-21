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
#include <boost/property_tree/ptree_fwd.hpp>

#ifndef asio_serial
#define asio_serial boost::asio::serial_port_base
#endif

namespace pt = boost::property_tree;


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
    int16_t get_targetValue();
    uint8_t get_targetConfig();
    Module::Signal& get_signal();
    //setter
    void set_number(unsigned int _number);
    void set_inUse(bool _inUse);
    void set_targetValue(int16_t _targetValue);     //only for changing from outside
    void set_targetConfig(uint8_t _targetConfig);   //
    void set_value(int16_t _value);     //only for dispatcher
    void set_config(uint8_t _config);   //
    Module::Signal signal;

protected:

    mutable std::mutex _mutex;
    unsigned int number;
    bool inUse = false;
    int16_t targetValue = 0;
    uint8_t targetConfig = 0;
    int16_t value = 0;
    uint8_t config = 0;
};
struct IoPin : public Pin
{
    pt::ptree getProperties();
    IoPin(unsigned int number, unsigned char value, IoConfig config);
    IoPin& operator =(const IoPin& other);
    void syncInUse(); //determine inUse
    Module::Slot& get_slot();

    Module::Slot slot;
};
struct AdcPin : public Pin
{
    pt::ptree getProperties();
    AdcPin(unsigned int number, unsigned char value, AdcConfig config);
    AdcPin& operator =(const AdcPin& other);
    void syncInUse(); //determine inUse
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
        //debug Functions
		const void plotFlushStringToConsole(const std::string& flushString);
        size_t getSizeBufOut();
        bool getEmptyBufOut();
	protected:
        unsigned long recCount = 0;     // only for debug information
        unsigned long sendCount = 0;    //
        unsigned long flushCount = 0;   //
		void startListening();
		void startSending();
		void stopSending();
		void stopListening();
		virtual void serialDispatcher(std::string cmd);
		void Sending();
		void Listening();
        void addElementToBufOut(const std::string& cmd);
        std::string takeElementFromBufOut();

		std::string device;
		int baudrate;
		bool sendEnable;
		bool listenEnable;
		bool connectionEstablished;
		boost::asio::io_service ioService;
		boost::asio::serial_port port;
		int pollOne(char* buffer);
        bool sendOne(std::string &m_string);

		volatile bool rtr;
		//convert Functions especialy for the Protocol
        std::string to_flushString(int16_t number);
        std::string to_flushString(uint8_t number);
        std::string to_flushString(IoConfig config);
        std::string to_flushString(AdcConfig config);
        uint8_t to_uint8_t(const std::string& flushString);
        int16_t to_int16_t(const std::string& flushString);
private:
        std::mutex mutexBufOut;
        std::list<std::string> bufOut;
};
class IoD : protected serialCmdInterface{
public:
    IoD(bool cyclicSend,
        unsigned int milliseconds,
        const std::string& device,
        unsigned int baudrate
        );
    ~IoD();
    unsigned long getRecCount();        // only for debug information
    unsigned long getSendCount();       //
    unsigned long getFlushCount();      //
    unsigned long getClockCount();      //

    void getDataFromSqlServer();    //called once in constructor AND CONSTRUCTS THE PINS

    void readInputs(bool readUnusedToo);                        //  used in mainloop
    void writeOutputs(bool writeUnusedToo);                     //
    void triggerPostModules();                                  //                      trigger all modules from all signals of the input / adc pins

    void syncInUse(); // look in all signals/slots and set in use in the pin objekt true / fals eif the signal/slot is connected/not
    void test();
    ////// REST interface
    pt::ptree getProperties();
    void getValues(std::map<std::string, int>& outMap, bool io, bool adc, int number = -1); //number == -1 --> all configs returned
    void getConfigs(std::map<std::string, int>& outMap, bool io, bool adc, int number = -1); //number == -1 --> all configs returned
    void changeConfig(char portType, int number, uint8_t newConfig);
    //////
    Module::Signal* getSignal(char portType, int pinNumber);
    Module::Slot* getSlot(int pinNumber);
protected:
    void writeConfig(bool writeUnusedToo);
    void changeConfigOnSqlServer(char portType, int number, uint8_t newConfig);
    void resetMCU();
    void cyclicSync();
    void initMCU();
    void initClock(unsigned int milliseconds);
    bool mcuResetDone=false;
    unsigned long clockCount = 0;   // only for debug information
    Clock::Clock* m_clock = nullptr;
    void serialDispatcher(std::string cmd) override;
    bool allInputValuesSynced();
    bool allOutputValuesSynced();
    std::map<int,IoPin> ioMapInput;
    std::map<int,IoPin> ioMapOutput;
    std::map<int,AdcPin> adcMap;
    mutable std::mutex _mutex;
};
}


#endif
