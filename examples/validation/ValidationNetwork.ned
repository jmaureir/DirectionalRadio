//
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

import DirectionalRadio.RadioChannel;
import inet.nodes.inet.StandardHost;
import channels.MyThroughputMeteringChannel;
//
// TODO auto-generated module
//

network ValidationNetwork
{
    @display("bgb=1100,500");
    submodules:
        channelControl: RadioChannel {
            @display("p=67,32");
            numChannels = default(11);
        }

        sender: StandardHost {
            @display("p=100,364");
            routingFile = "sender.rt";
        }
        receiver: StandardHost {
            @display("p=1000,364");
            routingFile = "receiver.rt";
        }

        meshNode1: MeshNode {
            @display("p=100,250");
            mgmtWlan1 = default("Ieee80211MgmtWDSExtended");
        }
        meshNode2: MeshNode {
            @display("p=200,250");
            mgmtWlan0 = default("Ieee80211MgmtWDSExtended");
            mgmtWlan1 = default("Ieee80211MgmtWDSExtended");
        }
        meshNode3: MeshNode {
            @display("p=300,250");
            mgmtWlan0 = default("Ieee80211MgmtWDSExtended");
            mgmtWlan1 = default("Ieee80211MgmtWDSExtended");
        }
        meshNode4: MeshNode {
            @display("p=400,250");
            mgmtWlan0 = default("Ieee80211MgmtWDSExtended");
            mgmtWlan1 = default("Ieee80211MgmtWDSExtended");
        }
        meshNode5: MeshNode {
            @display("p=500,250");
            mgmtWlan0 = default("Ieee80211MgmtWDSExtended");
            mgmtWlan1 = default("Ieee80211MgmtWDSExtended");
        }
        meshNode6: MeshNode {
            @display("p=600,250");
            mgmtWlan0 = default("Ieee80211MgmtWDSExtended");
            mgmtWlan1 = default("Ieee80211MgmtWDSExtended");
        }
        meshNode7: MeshNode {
            @display("p=700,250");
            mgmtWlan0 = default("Ieee80211MgmtWDSExtended");
            mgmtWlan1 = default("Ieee80211MgmtWDSExtended");
        }
        meshNode8: MeshNode {
            @display("p=800,250");
            mgmtWlan0 = default("Ieee80211MgmtWDSExtended");
            mgmtWlan1 = default("Ieee80211MgmtWDSExtended");
        }
        meshNode9: MeshNode {
            @display("p=900,250");
            mgmtWlan0 = default("Ieee80211MgmtWDSExtended");
            mgmtWlan1 = default("Ieee80211MgmtWDSExtended");
        }
        meshNode10: MeshNode {
            @display("p=1000,250");
            mgmtWlan0 = default("Ieee80211MgmtWDSExtended");
        }

    connections allowunconnected:
        sender.ethg++ <--> MyThroughputMeteringChannel {  delay = 100us; format = "b B"; } <--> meshNode1.ethg++;
        receiver.ethg++ <--> MyThroughputMeteringChannel {  delay = 100us; format = "b B"; } <--> meshNode10.ethg++;
}
