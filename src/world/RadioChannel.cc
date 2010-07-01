/***************************************************************************
 * file:        RadioChannel.cc
 *
 * copyright:   (C) 2005 Andras Varga
 * copyright:   (C) 2009 Juan-Carlos Maureira
 * copyright:   (C) 2009 Alfonso Ariza
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 ***************************************************************************
 * part of:     framework implementation developed by tkn
 **************************************************************************/


#include "RadioChannel.h"
#include "src/RadioModel/AbstractRadio.h"
#include "FWMath.h"
#include <cassert>

#define coreEV (ev.isDisabled()||!coreDebug) ? ev : ev << "RadioChannel: "

Define_Module(RadioModel::RadioChannel);

// New operator to show the HostEntry now showing each radio channel and gate id
std::ostream& RadioModel::operator<<(std::ostream& os, const RadioModel::RadioChannel::HostEntry& h)
{
	// TODO: write the NIC's number and the neighbors per NIC
    os << h.host->getFullPath() << " (x=" << h.pos.x << ",y=" << h.pos.y << ")" << endl;
    int count = 0;
    for(RadioModel::RadioChannel::RadioList::const_iterator it = h.radioList.begin();it!=h.radioList.end();it++) {
         os << "                 radio " << count << " " << (*it)->radioModule << " channel " << (*it)->channel << " gate id " << (*it)->hostGateId << endl;
         count++;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, RadioModel::RadioChannel::HostEntry* h) {
	return os << *h;
}
std::ostream& operator<<(std::ostream& os, const RadioModel::RadioChannel::TransmissionList& tl)
{
    for (RadioModel::RadioChannel::TransmissionList::const_iterator it = tl.begin(); it != tl.end(); ++it)
        os << endl << *it;
    return os;
}

// NEW METHODS TO SUPPORT MULTIPLES RADIOS

/* Check if the host entry has a radio in the given channel */
bool RadioModel::RadioChannel::HostEntry::isReceivingInChannel(int channel,double freq) {
   /* if there is some radio on the channel on this host */
   for(RadioChannel::RadioList::const_iterator it = radioList.begin();it!=radioList.end();it++)
   {
      if (((*it)->channel==channel) && (fabs(((*it)->radioCarrier - freq)/freq)<=percentage))
	  {
         return(true);
      }
   }
   return(false);
}

/* return a std::list with the radioIn host gates that points to radios on the given channel */
RadioModel::RadioChannel::radioGatesList RadioModel::RadioChannel::HostEntry::getHostGatesOnChannel(int channel,double freq) {

	RadioModel::RadioChannel::radioGatesList theRadioList;

   if (freq==0.0)
	   freq=carrierFrequency;

   for(RadioModel::RadioChannel::RadioList::const_iterator it = radioList.begin();it!=radioList.end();it++) {
	   int channelGate = (*it)->channel;
	   double freqGate = (*it)->radioCarrier;
	   if ((channelGate==channel) && (fabs((freqGate - freq)/freqGate)<=percentage)) {
         cGate* radioGate = NULL;
         if ((*it)->radioInGate) {
            radioGate = (*it)->radioInGate;
         } else {
        	 if (!this->radioInGate)
        		 this->radioInGate = (this->host)->gate("radioIn");
            radioGate = this->radioInGate;
         }
         if (radioGate != NULL ){
            theRadioList.push_back(radioGate);
         }
      }
   }
   return(theRadioList);
}

void RadioModel::RadioChannel::HostEntry::registerRadio(cModule* ar) {
	RadioModel::RadioChannel::RadioEntry* ra = new RadioModel::RadioChannel::RadioEntry(this);
    ra->radioModule = ar;
    ra->channel = -1;
    ra->radioInGate = NULL;
    ra->radioCarrier=carrierFrequency;

    cGate* radioIn = ar->gate("radioIn");

    for(RadioModel::RadioChannel::RadioList::iterator it = radioList.begin();it!=radioList.end();it++)
    {
    	if ((*it)->hostGateId == radioIn->getId())
          return; // Is register
    }

    while (radioIn!=NULL && radioIn->getOwnerModule() != host ) {
       radioIn = radioIn->getPreviousGate();
    }

    if (radioIn!=NULL && radioIn->getOwnerModule() == host) {
       ra->hostGateId = radioIn->getId();
       ra->radioInGate = radioIn;
    } else {
       ra->hostGateId = -1;
       ra->radioInGate = NULL;
    }

    if (this->radioChannel->neighbor_algorithm == NEIGHBORGRAPH) {

    	RadioModel::AbstractRadio* ar = check_and_cast<RadioModel::AbstractRadio*>(ar);

    	double intDist = ar->getInterferenceDistance();

    	ra->influence_offset[0] = -intDist;
    	ra->influence_offset[1] = intDist;
    	ra->influence_offset[2] = intDist;
    	ra->influence_offset[3] = -intDist;


    	//std::cout << "host " << ra->hostRef->host->getFullName() << endl;
    	//for(int i=0;i<4;i++) {
		//	std::cout << "boundary " << i << " " << ra->influence_offset[i] << std::endl;
    	//}

		// JcM Add: add the host into the neighbors graph
		this->radioChannel->neighbor_graph.insert(ra);
		ra->node_id = ra->radioModule->getId();
    }

    RadioModel::RadioChannel::HostEntry::radioList.push_back(ra);
}


void RadioModel::RadioChannel::HostEntry::unregisterRadio(cModule* ar) {
	// TODO: remove the node from neighborgraph
    for(RadioModel::RadioChannel::RadioList::iterator it = radioList.begin();it!=radioList.end();it++)
    {
    	RadioModel::RadioChannel::RadioEntry* ra = (*it);
    	if (ra->radioModule == ar)
    	{
    		RadioModel::RadioChannel::HostEntry::radioList.erase(it);
    		return;
    	}
    }
}

bool RadioModel::RadioChannel::HostEntry::updateRadioChannel(cModule* ar,int ch,double freq) {
   EV << "HostEntry " << this->host->getFullPath() << " updating radio channel to " << ar << " ch " << ch  << endl;
   for(RadioModel::RadioChannel::RadioList::iterator it = radioList.begin();it!=radioList.end();it++) {
      EV << "UpdateRadioChannel ar: " << ar->getFullPath() << " radioModule :" << (*it)->radioModule->getFullPath() << endl;
      if ((*it)->radioModule==ar) {
        (*it)->channel = ch;
		(*it)->radioCarrier=carrierFrequency;
		if (freq>0.0)
			(*it)->radioCarrier=freq;
        return(true);
      }
   }
   return(false);
}

bool RadioModel::RadioChannel::HostEntry::isRadioRegistered(cModule* r) {
   for(RadioModel::RadioChannel::RadioList::iterator it = radioList.begin();it!=radioList.end();it++) {
      if ((*it)->radioModule==r) {
        return(true);
      }
   }
   return(false);
}

RadioModel::RadioChannel::RadioEntry* RadioModel::RadioChannel::HostEntry::getRadioEntry(cModule* radio) {
   for(RadioModel::RadioChannel::RadioList::iterator it = radioList.begin();it!=radioList.end();it++) {
	  if ((*it)->radioModule==radio) {
		RadioEntry* re = (*it);
		return(re);
	  }
   }
   return(NULL);
}

cModule* RadioModel::RadioChannel::getHostByRadio(RadioModel::AbstractRadio* ar) {
    Enter_Method_Silent();
    for (HostList::iterator it = hosts.begin(); it != hosts.end(); it++) {

    	HostEntry* h = (RadioChannel::HostEntry*)(*it);

        if (h->isRadioRegistered((cModule*)ar)) {
            return (*it)->host;
        }
    }
    return NULL;
}

// END NEW METHODS FOR SUPPORT MULTIPLES RADIOS

RadioModel::RadioChannel::RadioChannel()
{

}

RadioModel::RadioChannel::~RadioChannel()
{
    for (unsigned int i = 0; i < transmissions.size(); i++)
        for (TransmissionList::iterator it = transmissions[i].begin(); it != transmissions[i].end(); it++)
            delete *it;
}

RadioModel::RadioChannel* RadioModel::RadioChannel::get() {
	RadioModel::RadioChannel* radioChannel = dynamic_cast<RadioModel::RadioChannel *>(simulation.getModuleByPath("channelControl"));

	if (!radioChannel) {
	   throw cRuntimeError("Could not find RadioChannel module");
	}

    return radioChannel;
}

/**
 * Sets up the playgroundSize and calculates the
 * maxInterferenceDistance
 *
 * @ref calcInterfDist
 */
void RadioModel::RadioChannel::initialize(int stage)
{
	if (stage == 0) {
		playgroundSize.x = atoi(this->getParentModule()->getDisplayString().getTagArg("bgb",0));
		playgroundSize.y = atoi(this->getParentModule()->getDisplayString().getTagArg("bgb",1));

		this->par("playgroundSizeX") = playgroundSize.x;
		this->par("playgroundSizeY") = playgroundSize.y;
	} else {
		numChannels = par("numChannels");
		transmissions.resize(numChannels);

		lastOngoingTransmissionsUpdate = 0;

		carrierFrequency = par("carrierFrequency");

		std::string alg_type = par("neighborsAlgorithm");
		if (alg_type=="NeighborGraph") {
			EV << "USING NEIGHBORGRAPH to compute the neighbors" << endl;
			this->neighbor_algorithm = NEIGHBORGRAPH;
		} else {
			EV << "USING BRUTEFORCE to compute the neighbors (DEFAULT)" << endl;
			this->neighbor_algorithm = BRUTEFORCE;
		}

		WATCH_LIST(hosts);
		WATCH_VECTOR(transmissions);
	}
}


ChannelControl::HostRef RadioModel::RadioChannel::registerHost(cModule * host, const Coord& initialPos,cGate *radioInGate)
{
    Enter_Method_Silent();
    if (lookupHost(host) != NULL)  {
    	// host already registered.
    	return lookupHost(host);
    }

    RadioModel::RadioChannel::HostEntry* he = new RadioModel::RadioChannel::HostEntry(this);
    he->host = host;
    he->pos = initialPos;
    he->radioInGate = radioInGate;
    he->carrierFrequency=par("carrierFrequency");;
    he->percentage=par ("percentage");
    he->isNeighborListValid = false;
    // TODO: get it from caller
    he->channel = 0;
    hosts.push_back(he);

    return hosts.back(); // last element
}

void RadioModel::RadioChannel::unregisterHost(cModule *host)
{
	// TODO: Fix this : remove asymmetry assumtion and remove the host from the neighbors graph
    Enter_Method_Silent();
    for (HostList::iterator it = hosts.begin(); it != hosts.end(); it++) {
        if ((*it)->host == host) {
            HostRef h = *it;

            // erase host from all registered hosts' neighbor list
            for (HostList::iterator i2 = hosts.begin(); i2 != hosts.end(); ++i2) {
                HostRef h2 = *i2;
                h2->neighbors.erase(h);
                h2->isNeighborListValid = false;
                h->isNeighborListValid = false;
            }

            // erase host from registered hosts
            hosts.erase(it);
            return;
        }
    }
    error("unregisterHost failed: no such host");
}

ChannelControl::HostRef RadioModel::RadioChannel::lookupHost(cModule *host)
{
    Enter_Method_Silent();
    for (HostList::iterator it = hosts.begin(); it != hosts.end(); it++)
        if ((*it)->host == host)
            return (*it);
    return NULL;
}

const RadioModel::RadioChannel::HostList RadioModel::RadioChannel::getNeighbors(RadioModel::RadioChannel::HostEntry* haux, cModule* r)
{
    Enter_Method_Silent();
    HostEntry* h = dynamic_cast <RadioModel::RadioChannel::HostEntry*>(haux);
    if (!h)
    	error(" not HostEntry in getNeighbors");

    // get the radio entry corresponding to the radio module r
	RadioEntry* ra = h->getRadioEntry(r);

	//std::cout << "getNeighbors host " << h->host->getFullName() << " isListValid " <<  ra->isNeighborListValid << " neighbors size " << ra->neighbors.size() << endl;

    if (ra!=NULL) {
    	if (!ra->isNeighborListValid) {
    		// recalculate the neighbors list
    		//EV << "host " << h->host->getFullName() << " updating connections from getNeighbors" << endl;
			this->updateConnections(h);
    	}
    	return ra->neighbors;
    } else {
    	error("getNeighbors::RadioEntry does not exist!!");
    }
    return HostList();
}

const RadioModel::RadioChannel::TransmissionList& RadioModel::RadioChannel::getOngoingTransmissions(const int channel)
{
    Enter_Method_Silent();

    checkChannel(channel);
    purgeOngoingTransmissions();
    return transmissions[channel];
}

void RadioModel::RadioChannel::addOngoingTransmission(RadioModel::RadioChannel::HostEntry* h, AirFrame *frame)
{
    Enter_Method_Silent();

    // we only keep track of ongoing transmissions so that we can support
    // NICs switching channels -- so there's no point doing it if there's only
    // one channel
    if (numChannels==1)
    {
        delete frame;
        return;
    }

    // purge old transmissions from time to time
    if (simTime() - lastOngoingTransmissionsUpdate > TRANSMISSION_PURGE_INTERVAL)
    {
        purgeOngoingTransmissions();
        lastOngoingTransmissionsUpdate = simTime();
    }

    // register ongoing transmission
    take(frame);
    frame->setTimestamp(); // store time of transmission start
    AirFrameExtended * frameExtended = dynamic_cast<AirFrameExtended*>(frame);
    if (!frameExtended)
    {
    	frameExtended = new AirFrameExtended();
    	AirFrame * faux = frameExtended;
    	*faux = (*frame);
    	delete frame;
    }

    transmissions[frame->getChannelNumber()].push_back(frameExtended);
}

void RadioModel::RadioChannel::purgeOngoingTransmissions()
{
    for (int i = 0; i < numChannels; i++)
    {
        for (TransmissionList::iterator it = transmissions[i].begin(); it != transmissions[i].end();)
        {
            TransmissionList::iterator curr = it;
            AirFrame *frame = *it;
            it++;

            if (frame->getTimestamp() + frame->getDuration() + TRANSMISSION_PURGE_INTERVAL < simTime())
            {
                delete frame;
                transmissions[i].erase(curr);
            }
        }
    }
}

void RadioModel::RadioChannel::updateConnections(RadioModel::RadioChannel::HostEntry* h)
{
	if (this->neighbor_algorithm == BRUTEFORCE) {
		//Paula Uribe: Implementation considering asymmetric communications
		//we update all the radios in the given host

		for (RadioModel::RadioChannel::RadioList::iterator it = h->radioList.begin();it!= h->radioList.end();it++)
		{
			RadioModel::AbstractRadio* ar = (RadioModel::AbstractRadio*)(*it)->radioModule;

			// we clean the radio neighbors list to rebuild it
			(*it)->neighbors.clear();

			for (HostList::iterator h_it = hosts.begin(); h_it != hosts.end(); ++h_it)
			{
				HostEntry* hi = (*h_it);
				if (hi == h)
					continue;
				bool inRange = ar->isInCoverageArea(hi->pos);

				if (inRange)
				{
					(*it)->neighbors.push_back(hi);
					(*it)->isNeighborListValid = true;
				}
				else
				{
					(*it)->neighbors.remove(hi);
					(*it)->isNeighborListValid = true;
				}
			}
		}
	} else {
		// Juan-Carlos Maureira: Neighbors calculation by using neighbors graph
		// we calculate the neighbors for the given host and we invalidate
		// the neighbors list for all the affected hosts
		for (RadioModel::RadioChannel::RadioList::iterator it = h->radioList.begin();it!= h->radioList.end();it++) {
			// get the last host position and update the host

			RadioEntry* re = (*it);

			// check if radio position differs with the new host position
			if (re->_pos != h->pos) {
				//std::cout << "updating radio " << re->radioModule->getId() << " in host " << re->hostRef->host->getFullName() << " old pos " << (*it)->_pos << " new pos " << h->pos << endl;
				// update the node position in the graph
				this->neighbor_graph.updateNodePosition(re, h->pos);
				//std::cout << "updated done" << endl;
			} else {
				//std::cout << "updating radio " << re->radioModule->getId() << " no need to update the position in the neighbor graph (node static) " << endl;
			}

			NeighborGraph::NodeList neigbors_nodes;
			NeighborGraph::NodeList affected_radios;
			this->neighbor_graph.getNeighborsForNode(re, neigbors_nodes, affected_radios );

			//std::cout << "neighbors size :" << neigbors_nodes.size() << " toUpdate size :" << affected_radios.size() << std::endl;

			// calculate the neighbors hosts for the nodes neighbors
			re->neighbors.clear();
			RadioModel::AbstractRadio* ar = (RadioModel::AbstractRadio*)(re->radioModule);

			bool nodesUpdated = true;
			for(NeighborGraph::NodeList::iterator itr = neigbors_nodes.begin();itr!=neigbors_nodes.end();itr++) {
				RadioEntry* radioEntry = (RadioEntry*)(*itr);
				bool inRange = ar->isInCoverageArea(radioEntry->hostRef->pos);
				if (inRange) {
					//std::cout << "host " << radioEntry->hostRef->host->getFullName() << " is neighbor for " << h->host->getFullName() << endl;
					re->neighbors.push_back(radioEntry->hostRef);
				} else {
					//std::cout << "host " << radioEntry->hostRef->host->getFullName() << " is NOT neighbor for " << h->host->getFullName() << endl;
					nodesUpdated = false;
				}
			}
			// mark this neighborlist valid if all nodes are correctly updated
			if (nodesUpdated) {
				re->isNeighborListValid = true;
			} else {
				re->isNeighborListValid = false;
			}


			// invalidate the list for affected_radios
			for(NeighborGraph::NodeList::iterator itr = affected_radios.begin();itr!=affected_radios.end();itr++) {
				RadioEntry* radioEntry = (RadioEntry*)(*itr);
				//std::cout << "host " << radioEntry->hostRef->host->getFullName() << " is invalidated by " << h->host->getFullName() << endl;
				radioEntry->isNeighborListValid = false;
				radioEntry->hostRef->isNeighborListValid = false;
			}

			//std::cout << "done updating connections for " << h->host->getFullName() << endl;
		}
	}
}

// Paula Uribe: add new method for updating all the hosts connections, not only the ones that have moved.
void RadioModel::RadioChannel::updateAllConnections()
{
	for (HostList::iterator h = hosts.begin(); h != hosts.end(); ++h)
	{
		HostEntry* hi = (RadioChannel::HostEntry*)(*h);
		this->updateConnections(hi);
	}
}

// Paula Uribe: redefine method. Now it calls to updateAllConnections method
void RadioModel::RadioChannel::updateHostPosition(HostEntry* h, const Coord& pos)
{
    Enter_Method_Silent();

    //std::cout << "updating host " << h << " old pos " << h->pos << " new pos " << pos << endl;

    h->pos = pos;

    // TODO: Update the neighbors graph node's position

    if (this->neighbor_algorithm == NEIGHBORGRAPH) {
		this->updateConnections(h);
    } else {
        this->updateAllConnections();
    }
}

void RadioModel::RadioChannel::updateHostChannel(HostEntry* h, const int channel)
{
    Enter_Method_Silent();
    checkChannel(channel);
    if (h->radioList.empty())
    {
    	cModule *module = h->host;
    	RadioModel::RadioChannel::RadioEntry* ra = new RadioModel::RadioChannel::RadioEntry(h);
    	cGate *radioIn = module->gate("radioIn");
        ra->radioModule = radioIn->getOwnerModule();
        ra->channel = channel;
        ra->hostGateId = radioIn->getId();
        ra->radioCarrier = carrierFrequency;
        h->radioList.push_back(ra);
    }
    h->radioList.front()->channel= channel;
}

void RadioModel::RadioChannel::updateHostChannel(HostEntry* h, const int channel, cModule* ca,double freq)
{
    Enter_Method_Silent();

    checkChannel(channel);

    h->updateRadioChannel(ca,channel,freq);
}

void RadioModel::RadioChannel::sendToChannel(cSimpleModule *srcRadioMod, HostEntry* srcHost, AirFrame *airFrame)
{
	// JcM fix: get the neighbors from the source radio.
	const HostList& neighbors = getNeighbors(srcHost,srcRadioMod);

	//int n = neighbors.size();
    AirFrameExtended * msgAux = dynamic_cast<AirFrameExtended*>(airFrame);
    // JcM Fix: neighbors is a list, so, let's use iterators
    //for (int i=0; i<n; i++)
	for (HostList::const_iterator h_it = neighbors.begin(); h_it != neighbors.end(); ++h_it)
    {
    	HostEntry* h = dynamic_cast<HostEntry*>((*h_it));

    	RadioModel::RadioChannel::radioGatesList theRadioList;
        if (msgAux)
        	theRadioList = h->getHostGatesOnChannel(airFrame->getChannelNumber(),msgAux->getCarrierFrequency());
        else
        	theRadioList = h->getHostGatesOnChannel(airFrame->getChannelNumber(),0.0);
        // if there are some radio on the channel.
        int sizeGate = theRadioList.size();
        if (sizeGate>0) {
        	for(RadioModel::RadioChannel::radioGatesList::iterator rit=theRadioList.begin();rit != theRadioList.end();rit++) {
        		cGate* radioGate = (*rit);
				//EV << "sending message to host listening on the same channel\n";
				// account for propagation delay, based on distance in meters
				// Over 300m, dt=1us=10 bit times @ 10Mbps
				simtime_t delay = srcHost->pos.distance(h->pos) / LIGHT_SPEED;
				srcRadioMod->sendDirect((cMessage *)airFrame->dup(),delay, airFrame->getDuration(),radioGate);
			}
        } else {
			//EV << "skipping host listening on a different channel\n";
        }
    }
    // register transmission in ChannelControl
   	addOngoingTransmission(srcHost, airFrame);
}
