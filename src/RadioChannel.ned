//***************************************************************************
// * file:        RadioChannel.ned
// *
// * author:      Juan-Carlos Maureira, Paula Uribe
// * author:      Andras Varga, Steffen Sroka, Daniel Willkomm, Alfonso Ariza, 
// *
// * copyright:   (C) 2004 Telecommunication Networks Group (TKN) at
// *              Technische Universitaet Berlin, Germany.
// *              (C) 2009 Project MASCOTTE at
// *              INRIA Sophia-Antipolis, Cote d'azur, France.
// *
// *              This program is free software; you can redistribute it
// *              and/or modify it under the terms of the GNU General Public
// *              License as published by the Free Software Foundation; either
// *              version 2 of the License, or (at your option) any later
// *              version.
// *              For further information see file COPYING
// *              in the top level directory
// ***************************************************************************
// * part of:     framework implementation developed by tkn
// **************************************************************************/

package DirectionalRadio;

//
// RadioChannel has exactly one instance in every network model that 
// contains mobile or wireless nodes. This module gets informed about the 
// location and movement of nodes, and determines which nodes are within
// communication or interference distance. This info is then used by the 
// radio interfaces of nodes at transmissions.
//
// Side effect: updates the containing compound module's display string
// according to the given playground size (sets <tt>"p=0,0;b=$playgroundSizeX,
// $playgroundSizeY"</tt>).
//
// This RadioChannel is a different implementation from the one in 
// Mobility Framework 1.0a5: here we use sendDirect(), while the MF version 
// used normal send() and dynamic connections.
//
// @author Andras Varga (based on MF's ChannelControl by Steffen Sroka and Daniel Willkomm) 
// @author Juan-Carlos Maureira, Alfonso Ariza, Paula Uribe (Based on Andras Varga Code)
//
simple RadioChannel
{
    parameters:
        @class(RadioModel::RadioChannel);
		bool coreDebug = default(false); // debug switch for core framework
        double playgroundSizeX = default(600); // x size of the playground (in meters)
        double playgroundSizeY = default(400); // y size of the playground (in meters)
        double alpha = default(2); // path loss coefficient
        double carrierFrequency @unit("Hz") = default(2.4GHz); // carrier frequency of the channel (in Hz)
        double percentage = default(0.1); // channel overlapping interference?
        string neighborsAlgorithm = default("BruteForce"); // can be BruteFroce and NeighborGraph
        string propagationModel = default("PathLossReceptionModel") @enum("","PathLossReceptionModel","TwoRayGroundModel","RiceModel","RayleighModel","NakagamiModel","FreeSpaceModel","LogNormalShadowingModel");
        int numChannels = default(1); // number of radio channels (frequencies)
        @display("i=misc/cloud");
}

