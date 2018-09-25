#ifndef DB_h
#define DB_h
#include <map>
#include <set>
#include <string>
namespace DB{
  struct Portadress{
    char type;
    int port;
    int pin;
    int config;
    bool operator == (const Portadress& otherAdr) const {
      return (
        this->type    == otherAdr.type   &&
        this->port    == otherAdr.port   &&
        this->pin     == otherAdr.pin    &&
        this->config  == otherAdr.config
      );
    }
    bool operator < ( const Portadress& otherAdr ) const {
      return(
        ( this->type <  otherAdr.type )  ||
        ( this->type == otherAdr.type && this->port <  otherAdr.port ) ||
        ( this->type == otherAdr.type && this->port == otherAdr.port && this->pin <  otherAdr.pin ) ||
        ( this->type == otherAdr.type && this->port == otherAdr.port && this->pin == otherAdr.pin && this->config <  otherAdr.config )
      );
    }
  };
  struct Pin{
    Portadress adress;
    int value;
  };

  struct Parameteradress{
   int moduleID;
   std::string parameterKey;
   bool operator == (const Parameteradress& otherAdr) const {
     return (
       this->moduleID     == otherAdr.moduleID      &&
       this->parameterKey == otherAdr.parameterKey
     );
   }
   bool operator < ( const Parameteradress& otherAdr ) const {
     return(
       ( this->moduleID <  otherAdr.moduleID )  ||
       ( this->moduleID == otherAdr.moduleID && this->parameterKey <  otherAdr.parameterKey )
     );
   }
  };
  struct Parameter{
    Parameteradress adress;
    int value;
  };
  class IoDatabase{
  public:
    void resortIO();
    std::map<Portadress, Pin> all;
    std::map<Portadress, Pin*> input;
    std::map<Portadress, Pin*> output;
  };
  class ParameterDatabase{
    std::map<Parameteradress, Parameter> parameter;
  };
  struct Database{
    IoDatabase m_ioDB;
    ParameterDatabase m_paramDB;
  };

}
#endif
