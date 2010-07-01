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

#include <math.h>
#include "Ieee80211DirectionalRadio.h"
#include "IAntennaPattern.h"

Define_Module(RadioModel::Ieee80211DirectionalRadio);

void RadioModel::Ieee80211DirectionalRadio::initialize(int stage)
{
	AbstractRadio::initialize(stage);

	EV << "Initializing Ieee80211DirectionalRadio, stage=" << stage << endl;

	if (stage == 0)
	{

		// Obtain antenna parameters
		this->dBThreshold = this->par("dBThreshold");
		this->mainLobeGain = this->par("mainLobeGain");
		this->sideLobeGain = this->par("sideLobeGain");
		this->beamWidth = (this->beamWidth * M_PI) / 360.0;
		this->mainLobeOrientation = this->par("mainLobeOrientation");	// *NOTE that the mainLobeOrientation angle is measured
																		//  according to the x-y axis of the simulations playground
		this->mainLobeOrientation = (this->mainLobeOrientation * M_PI) / 180.0;

		double alpha = this->radioChannel->par("alpha");
		// Side lobe interference distance calculation

	    double waveLength = (LIGHT_SPEED / this->carrierFrequency);
	    double minReceiverPower = pow(10.0, this->thermalNoise / 10.0);
	    this->sideLobeInterfDist = pow(this->transmitterPower * pow(10.0, this->sideLobeGain / 10.0) * waveLength * waveLength / (16 * M_PI * M_PI * minReceiverPower), 1.0 / alpha);

		// Own Antenna Pattern
		this->antennaPattern = this->createAntennaPattern(this->par("patternType"));
	}

}

RadioModel::IAntennaPattern* RadioModel::Ieee80211DirectionalRadio::createAntennaPattern(const char *patternType)
{
	EV << "searching antenna pattern; " << patternType  << endl;
	if (cObjectFactory::find(patternType))
	{
		this->antennaPattern = (IAntennaPattern *)createOne(patternType);
		this->antennaPattern->setRadioRef(this);
		this->antennaPattern->setName(patternType);
		this->antennaPattern->initialize();
		take(this->antennaPattern);
	}
	else
	{
		error("Antenna Pattern not valid");
	}
	return this->antennaPattern;

}

double RadioModel::Ieee80211DirectionalRadio::calculateEffectiveReceivedPower(double pTx, Coord& tx, Coord& rx)
{
	//std::cout << "Calculating the arrival power, directional radio........" << endl;
	double distance = tx.distance(rx);
	double rcvdPower = receptionModel->calculateReceivedPower(pTx, carrierFrequency, distance);
    double gainRx = (this->antennaPattern)->calculateGain(rx, tx);
    double arrivalPower = rcvdPower * gainRx;
    return arrivalPower;
}

double RadioModel::Ieee80211DirectionalRadio::calculateEffectiveTransmissionPower(double pTx, Coord& tx, Coord& rx)
{
	//std::cout << "Calculating the sending power, directional radio........" << endl;
	double gain = (this->antennaPattern)->calculateGain(tx, rx);
	double Ptx = pTx * gain;
	return Ptx;
}

// Paula Uribe: add new method for calculating the transmission/reception angle
double RadioModel::Ieee80211DirectionalRadio::calculateAngle(double distanceX, double distanceY)
{
	//std::cout << "Calculating the tx/rx angle, directional radio........" << endl;
	double thetaPrima = 0;
	if ( distanceY >= 0 )
	{
		if ( distanceX >= 0 )
		{
			thetaPrima = atan(distanceY/distanceX) + M_PI;
		}
		else
		{
			thetaPrima = atan(distanceY/distanceX) + 2 * M_PI ;
		}
	}
	else
	{
		if ( distanceX >= 0 )
		{
			thetaPrima = atan(distanceY/distanceX) + M_PI ;
		}
		else
		{
			thetaPrima = atan(distanceY/distanceX);
		}
	}
	return thetaPrima;
}

void RadioModel::Ieee80211DirectionalRadio::handleLowerMsgStart(AirFrame* airframe)
{
	//std::cout << "handleLowerMsgStart, directional radio: new airframe arrived........" << endl;

    // Transmitter and receiver radio positions
    Coord& myPos = (Coord& )getMyPosition();
    Coord& framePos = airframe->getSenderPos();

    AirFrameExtended * airframeExt = dynamic_cast<AirFrameExtended * >(airframe);
    if (airframeExt)
    	if (airframeExt->getCarrierFrequency()>0.0)
    		carrierFrequency = airframeExt->getCarrierFrequency();

    // Paula Uribe: calculate the arrival power for a directional antenna.
    // It includes the directional reception gain
    double rcvdPower = calculateEffectiveReceivedPower(airframe->getPSend(), framePos, myPos);
    airframe->setPowRec(rcvdPower);
    // store the receive power in the recvBuff
    recvBuff[airframe] = rcvdPower;

    // if receive power is bigger than sensitivity and if not sending
    // and currently not receiving another message and the message has
    // arrived in time
    // NOTE: a message may have arrival time in the past here when we are
    // processing ongoing transmissions during a channel change
    if (airframe->getArrivalTime() == simTime() && rcvdPower >= sensitivity && rs.getState() != RadioState::TRANSMIT && snrInfo.ptr == NULL)
    {
        // Put frame and related SnrList in receive buffer
        SnrList snrList;
        snrInfo.ptr = airframe;
        snrInfo.rcvdPower = rcvdPower;
        snrInfo.sList = snrList;

        // add initial snr value
        addNewSnr();

        if (rs.getState() != RadioState::RECV)
        {
            // publish new RadioState
        	//std::cout << "publish new RadioState:RECV\n";
            setRadioState(RadioState::RECV);
        }
    }
    else
    {
        //add receive power to the noise level
        noiseLevel += rcvdPower;

        // if a message is being received add a new snr value
        if (snrInfo.ptr != NULL)
        {
            // update snr info for currently being received message
        	//std::cout << "adding new snr value to snr list of message being received\n";
            addNewSnr();
        }

        // update the RadioState if the noiseLevel exceeded the threshold
        // and the radio is currently not in receive or in send mode
        if (noiseLevel >= sensitivity && rs.getState() == RadioState::IDLE)
        {
        	//std::cout << "setting radio state to RECV\n";
            setRadioState(RadioState::RECV);
        }
    }
}

void RadioModel::Ieee80211DirectionalRadio::sendToChannel(AirFrame *msg) {
    const RadioChannel::HostList &neighbors= cc->getNeighbors(myHostRef, this);

    // loop through all hosts in range
    RadioChannel::HostList::const_iterator it;
    for (it = neighbors.begin(); it != neighbors.end(); ++it)
    {
    	cModule *mod = (*it)->host;

        // we need to send to each radioIn[] gate
        // Check if the host is registered
    	RadioChannel::HostEntry* h;
       	h = dynamic_cast<RadioChannel::HostEntry*> (radioChannel->lookupHost(mod));
        if (h == NULL)
            error("cannot find module in channel control");

        // Get the radioIn gates connected to a radio on the given channel
        AirFrameExtended * msgAux = dynamic_cast<AirFrameExtended*>(msg);
        RadioChannel::radioGatesList theRadioList;
        if (msgAux)
        	theRadioList = h->getHostGatesOnChannel(msg->getChannelNumber(),msgAux->getCarrierFrequency());
        else
        	theRadioList = h->getHostGatesOnChannel(msg->getChannelNumber(),0.0);
        // if there are some radio on the channel.
        if (theRadioList.size()>0) {

        	for(RadioChannel::radioGatesList::iterator rit=theRadioList.begin();rit != theRadioList.end();rit++)
        	{
        		cGate* radioGate = (*rit);

        		// Paula Uribe: Change the Psend parameter fixed in "encapsulatePacket" method.
				// The actual Psend is the nominal transmitter power plus the gain in the direction of the communication. (for directional antennas)
				Coord& myPos = myHostRef->pos;
				double pSend = calculateEffectiveTransmissionPower(transmitterPower, myPos, h->pos);
				msg->setPSend(pSend);

				double distance = myPos.distance(h->pos);
				sendDirect((cMessage *)msg->dup(), distance / LIGHT_SPEED, msg->getDuration(), mod, radioGate->getId());
			}
        }
    }
    // register transmission in ChannelControl
   	radioChannel->addOngoingTransmission(myHostRef, msg);
}


// Paula Uribe: define method
bool RadioModel::Ieee80211DirectionalRadio::isInCoverageArea(Coord& c)
{
    //std::cout << "Ieee80211DirectionalRadio::isInCoverageArea..." << endl;
    Coord& myPos = (Coord& )getMyPosition();
	double distance = myPos.distance(c);

	double pSend = calculateEffectiveTransmissionPower(transmitterPower, myPos, c);
	double rcvdPower = receptionModel->calculateReceivedPower(pSend, carrierFrequency, distance);
	double noise = par("thermalNoise");

	if (rcvdPower >= pow(10.0, noise / 10.0))
	{
		//std::cout << "in coverage"  << endl;
		return true;
	}
	//std::cout << "NOT in coverage\n";
    return false;
}

double RadioModel::Ieee80211DirectionalRadio::getMainLobeGain()
{
	return this->mainLobeGain;
}

double RadioModel::Ieee80211DirectionalRadio::getSideLobeGain()
{
	return this->sideLobeGain;
}

double RadioModel::Ieee80211DirectionalRadio::getBeamWidth()
{
	return this->beamWidth;
}

double RadioModel::Ieee80211DirectionalRadio::getMainLobeOrientation()
{
	return this->mainLobeOrientation;
}

double RadioModel::Ieee80211DirectionalRadio::getDBThreshold()
{
	return this->dBThreshold;
}

void RadioModel::Ieee80211DirectionalRadio::drawCoverageArea() {
	//EV << "drawing coverage area" << endl;

	std::string pattern = this->par("patternType").stringValue();
	const char* circular = "RadioModel::CircularPattern";

	if (pattern.compare(circular) == 0)
	{
		cModule* host = this->myHostRef->host;
		if (host!=NULL) {

			cDisplayString& d = host->getDisplayString();

			// draw the coverage considering the noise level

			double noise = par("thermalNoise");
			double radio_noise = this->getInterferenceDistance(noise);

			d.removeTag("r1");
			d.insertTag("r1");
			d.setTagArg("r1",0,(long) radio_noise);
			d.setTagArg("r1",2,"gray");

			// draw the coverage considering the sensitivity level
			double sens = par("sensitivity");
			double radio_sens = this->getInterferenceDistance(sens);
			d.removeTag("r2");
			d.insertTag("r2");
			d.setTagArg("r2",0,(long) radio_sens);
			d.setTagArg("r2",2,"black");

			EV << this << "-> radio noise = " << radio_noise << " , radio sens = " << radio_sens << endl;

		} else {
			EV << "Radio has no host associated. can not draw the coverage area" << endl;
		}
	}
	else
	{
		//TODO: Draw other shapes of antenna
	}
}
