//
// Paula Uribe
// INRIA - Sophia ANtipolis
// September 2009
// A new module is created for simulating a directional antenna (radio). Is mainly based in the omni-directional
// radio (Ieee80211Radio, but new parameters have been added and new method for calculating the tx/rx angle, tx power,
// rx and arriving power.
// Since we are making an extension of the current omni-directional antenna to an "any-shaped" directional antenna, the
// methods for transmitting and received had to be modified in order to include the tx/rx gain, depending on the direction
// of the transmission.
// A 2D model is used for the directional antenna. A variable shape main beam can be used. We provide 4 different patterns
// (folium, cardioid, rose and circular, which corresponds to an omni-directional antenna pattern). The side/back lobes were
// modeled as an omni-directional antenna. The gain in a given direction is calculated selecting the greater value between
// the main lobe gain and the side lobe gain in the given direction.
// The IAntennaPattern allows the user to implement new antenna pattern shapes, by re-implementing the initialize() and the
// calculateGain() methods, and add the needed parameters for describing the curve.
//

#ifndef IEEE80211DIRECTIONALRADIO_H
#define IEEE80211DIRECTIONALRADIO_H

#include <omnetpp.h>
#include "RadioChannel.h"
#include "src/RadioModel/AbstractRadio.h"

namespace RadioModel {

class IAntennaPattern;

class INET_API Ieee80211DirectionalRadio : public RadioModel::AbstractRadio {
  protected:
	/**
	 * Paula Uribe: redefine methods declared in AbstractRadioExtended. Some modifications were done to make possible the
	 * interaction between directional and omni-directional antennas.
	 */
	// Initialize stage
	virtual void initialize(int stage);
    // Include the directional reception gain when calculating the received power.
	virtual void handleLowerMsgStart(AirFrame *airframe);
	// Include the directional transmission gain when adding the Psend information to the airframe.
    virtual void sendToChannel(AirFrame *msg);
    // Decides whether a host is within the coverage area of the other or not
	virtual bool isInCoverageArea(Coord& c);

	/**
	 * Paula Uribe: interfaces
	 */
	virtual IReceptionModel *createReceptionModel() {return (IReceptionModel *)createOne("PathLossReceptionModel");}
    virtual IRadioModel *createRadioModel() {return (IRadioModel *)createOne("Ieee80211RadioModel");}
    virtual IAntennaPattern *createAntennaPattern(const char *patternType);

  public:
	/**
	 * Paula Uribe: add methods for directional antenna
	 */
	// Perform the calculation of the transmission/reception angle
	double calculateAngle(double distanceX, double distanceY);
	// Perform the calculation of the arrival power (different to the received power, because this power includes the directional reception gain at the receiver.
    double calculateEffectiveReceivedPower(double Prx, Coord& tx, Coord& rx);
    // Perform the calculation of the sending power. The power amount radiated in each direction depends on the gain function of the directional antenna
    double calculateEffectiveTransmissionPower(double pTx, Coord& tx, Coord& rx);


  protected:
	/**
	 * Paula Uribe: add antenna pattern interface reference
	 */
    IAntennaPattern *antennaPattern;
    RadioChannel* cc;

	/**
	 * Paula Uribe: add directional antenna parameters
     */
	double mainLobeGain;
	double sideLobeGain;
	double beamWidth;
	double mainLobeOrientation;
	double sideLobeInterfDist;
	double dBThreshold;

	/**
	 * Paula Uribe: add 'get' methods for obtaining the antenna parameters from the different patterns implementations.
	 */
  public:
	double getMainLobeGain();
	double getSideLobeGain();
	double getBeamWidth();
	double getMainLobeOrientation();
	double getDBThreshold();

	virtual void drawCoverageArea();

};
}

#endif

