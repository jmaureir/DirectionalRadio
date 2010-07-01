/* -*- mode:c++ -*- ********************************************************
 * file:        RadioChannel.h
 *
 * copyright:   (C) 2006 Levente Meszaros, 2005 Andras Varga
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

#ifndef RADIOCHANNEL_H
#define RADIOCHANNEL_H

#include <vector>
#include <list>
#include <deque>
#include <set>
#include <omnetpp.h>
#include "AirFrame_m.h"
#include "NeighborGraph.h"
#include "ChannelControl.h"

#define LIGHT_SPEED 3.0E+8
#define TRANSMISSION_PURGE_INTERVAL 1.0

/**
 * @brief Monitors which hosts are "in range". Supports multiple channels.
 *
 * @ingroup channelControl
 * @sa ChannelAccess
 */

namespace RadioModel {

class AbstractRadio;

class INET_API RadioChannel : public ChannelControl
{
  protected:
    typedef std::list<cModule*> ModuleList;
    enum NeighborAlgorithm {
    	BRUTEFORCE,
    	NEIGHBORGRAPH
    };

    NeighborGraph neighbor_graph;
    NeighborAlgorithm neighbor_algorithm;

  public:

    class RadioEntry;

    typedef std::list<RadioEntry*> RadioList;
    typedef std::list<AirFrameExtended*> TransmissionList;
    typedef std::list<cGate*> radioGatesList;

    class HostEntry : public ChannelControl::HostEntry {
	   public:

		 // pointer to the channel Controller
		 RadioChannel* radioChannel;
		 RadioList radioList;

		 double carrierFrequency;
		 double percentage;
		 HostEntry (RadioChannel* c) { radioChannel=c;percentage=carrierFrequency=-1;}

		 radioGatesList getHostGatesOnChannel(int,double);

		 bool isReceivingInChannel(int,double);
		 void registerRadio(cModule*);
		 void unregisterRadio(cModule*);
		 bool updateRadioChannel(cModule*,int,double);
		 bool isRadioRegistered(cModule*);

         virtual bool getIsModuleListValid(){return isNeighborListValid;}

		 // JcM add: get the correspondent radioEntry given a radio module
		 RadioEntry* getRadioEntry(cModule* radio);

	};

    typedef std::list<HostEntry*> HostList;
    typedef std::vector<HostEntry*> HostVector;

    class RadioEntry : public NeighborGraph::NeighborGraphNode {
    public:
    	RadioEntry(HostEntry* h) : NeighborGraph::NeighborGraphNode(h->pos) { hostRef = h; };
    	cModule* radioModule;
    	HostEntry* hostRef;
    	int channel;
    	int hostGateId; // gate id on the host compound radioIn gate array
    	double radioCarrier;
    	cGate *radioInGate;
    	// Paula Uribe: neighbors list for this radio entry (hostEntry)
    	HostList neighbors;  // cached neighbor list
        bool isNeighborListValid; // to inform that list has changed
    };

  protected:
    /**
     * Keeps track of hosts/NICs, their positions and channels;
     * also caches neighbor info (which other hosts are within
     * interference distance).
     */

     typedef std::vector<TransmissionList> ChannelTransmissionLists;
     ChannelTransmissionLists transmissions; // indexed by channel number (size=numChannels)

     HostList hosts;

     double carrierFrequency;
     double percentage;

     simtime_t lastOngoingTransmissionsUpdate;

  protected:
    virtual void updateConnections(HostEntry* h);
    virtual void updateAllConnections();

    virtual void initialize(int stage);
    virtual int numInitStages() const {return 2;}

    /** @brief Throws away expired transmissions. */
    virtual void purgeOngoingTransmissions();

    friend std::ostream& operator<<(std::ostream&, const HostEntry&);
    friend std::ostream& operator<<(std::ostream&, const RadioChannel::TransmissionList&);

  public:
	RadioChannel();
    virtual ~RadioChannel();

    static RadioChannel * get();

    /** @brief Registers the given host. If radioInGate==NULL, the "radioIn" gate is assumed */
    virtual HostRef registerHost(cModule *host, const Coord& initialPos, cGate *radioInGate=NULL);

    /** @brief Unregisters the given host */
    virtual void unregisterHost(cModule *host);

    /** @brief Returns the "handle" of a previously registered host */
    virtual HostRef lookupHost(cModule *host);

    //Paula Uribe: change method interface, now we say:
    //** @brief Get the list of modules in range of the given radio (implicitly inside a host) */
    const HostList getNeighbors(RadioChannel::HostEntry* h, cModule* r);

    /** @brief Called from ChannelAccess, to transmit a frame to the hosts in range, on the frame's channel */
    virtual void sendToChannel(cSimpleModule *srcRadioMod, RadioChannel::HostEntry* srcHost, AirFrame *airFrame);

    // Paula Uribe: redefine method.
    virtual void updateHostPosition(RadioChannel::HostEntry* h, const Coord& pos);

    /** @brief Called when host switches channel (cModule* ca is the channel access (radio)) */
    virtual void updateHostChannel(RadioChannel::HostEntry* h, const int channel,cModule* ca,double freq);
    virtual void updateHostChannel(RadioChannel::HostEntry* h, const int channel);

    /** @brief get the host that contains the given radio */
    cModule* getHostByRadio(AbstractRadio* r);

    /** @brief Provides a list of transmissions currently on the air */
    const TransmissionList& getOngoingTransmissions(const int channel);

    /** @brief Notifies the channel control with an ongoing transmission */
    virtual void addOngoingTransmission(RadioChannel::HostEntry* h, AirFrame *frame);

    /** @brief Returns the host's position */
    const Coord& getHostPosition(RadioChannel::HostEntry* h)  {return h->pos;}
     /** @brief Returns the number of radio channels (frequencies) simulated */
    const int getNumChannels() {return numChannels;}
	const double getPercentage(){return percentage;}

};

}

#endif
