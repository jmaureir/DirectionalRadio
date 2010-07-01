// INRIA 2009 - Juan-Carlos Maureira
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
// based on Andras Varga Ieee80211NIC 
// @author: Juan-Carlos Maureira
//

import inet.linklayer.ieee80211.mac.Ieee80211Mac;
import Ieee80211MgmtStack.Ieee80211MgmtInterface;
import DirectionalRadio.RadioModel.AbstractRadioInterface;

//
// An Abstract Wireless Network Card Interface. Mgmgt module is given by
// the user and agent is external to the NIC.
// 
module Ieee80211NicCustom
{
    parameters:
        string mgmtType; // name of the management module type
        string radioType = default("Ieee80211Radio"); // name of the radio module to use
        @display("bgb=190,326;i=block/ifcard");
    gates:
        input uppergateIn; // to upper layers
        output uppergateOut; // from upper layers
        input radioIn; // to receive AirFrames
        input agentIn; // to Agent
        output agentOut; // from Agent

    submodules:

        mgmt: <mgmtType> like Ieee80211MgmtInterface {
            parameters:
                @display("p=96,69;q=wlanDataQueue");
        }
        mac: Ieee80211Mac {
            parameters:
                queueModule = "mgmt";
                @display("p=96,155");
        }
        radio: <radioType> like AbstractRadioInterface {
            parameters:
                @display("p=96,240");
        }
    connections:
        radioIn --> radio.radioIn;
        radio.uppergateIn <-- mac.lowergateOut;
        radio.uppergateOut --> mac.lowergateIn;

        mac.uppergateOut --> mgmt.macIn;
        mac.uppergateIn <-- mgmt.macOut;

        mgmt.uppergateOut --> uppergateOut;
        mgmt.uppergateIn <-- uppergateIn;

        mgmt.agentOut --> agentOut;
        agentIn --> mgmt.agentIn;
}
