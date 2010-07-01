//
// Paula Uribe
// INRIA - Sophia Antipolis
// September 2009
// "Interface for the antenna pattern calculation"
// This interface was created for allowing the user to change the antenna radiation pattern to any shape.
// For implementing a new pattern shape, the user must inherit from this interface and re-implement the
// initialize() and calculateGain() methods.
// The antenna parameters are read using templates, where the parameter type must be indicated (bool, double,
// int, string or custom).
//

#ifndef IANTENNAPATTERN_H
#define IANTENNAPATTERN_H

#include <typeinfo>

#include <omnetpp.h>
#include "INETDefs.h"
#include "Ieee80211DirectionalRadio.h"

namespace RadioModel {

class INET_API IAntennaPattern : public cOwnedObject {
  friend class Ieee80211DirectionalRadio;

  protected:
	  Ieee80211DirectionalRadio *radioRef;

	  virtual void setRadioRef(Ieee80211DirectionalRadio *radioRef) {
		  this->radioRef = radioRef;
	  }

  public:

	template <typename T> T par(const char* name) {
		cConfiguration* config = ev.getConfig();
		cConfigOption* o = NULL;
		if (typeid(T) == typeid(double)) {
			o = new cConfigOption(name,true,false,cConfigOption::CFG_DOUBLE,"","-1","antennaPatternParameter");
			T value = config->getAsDouble(this->getFullPath().c_str(),o,-1);
			delete(o);
			return value;
		}
		else if (typeid(T) == typeid(bool)) {
			o = new cConfigOption(name,true,false,cConfigOption::CFG_BOOL,"","-1","antennaPatternParameter");
			T value = config->getAsDouble(this->getFullPath().c_str(),o,-1);
			delete(o);
			return value;
		}
		else if (typeid(T) == typeid(int)) {
					o = new cConfigOption(name,true,false,cConfigOption::CFG_INT,"","-1","antennaPatternParameter");
					T value = config->getAsDouble(this->getFullPath().c_str(),o,-1);
					delete(o);
					return value;
		}
		else if (typeid(T) == typeid(std::string)) {
					o = new cConfigOption(name,true,false,cConfigOption::CFG_STRING,"","-1","antennaPatternParameter");
					T value = config->getAsDouble(this->getFullPath().c_str(),o,-1);
					delete(o);
					return value;
		}
		else if (typeid(T) == typeid("")) {
					o = new cConfigOption(name,true,false,cConfigOption::CFG_CUSTOM,"","-1","antennaPatternParameter");
					T value = config->getAsDouble(this->getFullPath().c_str(),o,-1);
					delete(o);
					return value;
		}

		return 0;
	}

    virtual void initialize() { };
    virtual double calculateGain(Coord& posA, Coord& posB) = 0;

};

}

#endif


