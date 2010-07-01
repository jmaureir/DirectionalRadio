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

import inet.base.NotificationBoard;
import inet.mobility.NullMobility;
import inet.linklayer.etherswitch.MACRelayUnitNP;
import inet.linklayer.ethernet.EtherEncap;
import inet.linklayer.ethernet.EtherMAC;
//
// TODO auto-generated module
//
module MeshNode
{
    @node();
    @display("bgb=467,321;i=device/accesspoint");
    string mgmtWlan0 = default("Ieee80211MgmtAPExtended");
    string mgmtWlan1 = default("Ieee80211MgmtAPExtended");
    gates:
        input radioIn[2] @directIn;
        inout ethg[];
    submodules:
        notificationBoard: NotificationBoard {
            @display("p=63,42");
        }
        nullMobility: NullMobility {
            @display("p=401,42");
        }
        wlan0: Ieee80211NicCustom {
            @display("p=108,252");
            mgmtType = mgmtWlan0;
        }
        wlan1: Ieee80211NicCustom{
            @display("p=348,252");
            mgmtType = mgmtWlan1;
        }
        macRelayUnitNP: MACRelayUnitNP {
            @display("p=229,77;i=old/greenbox");
        }
        etherEncap: EtherEncap {
            @display("p=108,155");
        }
        etherEncap1: EtherEncap {
            @display("p=348,155");
        }
        ethg[sizeof(ethg)]: EtherMAC {
            @display("p=229,252");
            promiscuous = true;
        }
    connections allowunconnected:
        radioIn++ --> wlan0.radioIn;
        radioIn++ --> wlan1.radioIn;
        wlan0.uppergateOut --> etherEncap.upperLayerIn;
        etherEncap.lowerLayerOut --> macRelayUnitNP.lowerLayerIn++;
        etherEncap.upperLayerOut --> wlan0.uppergateIn;
        macRelayUnitNP.lowerLayerOut++ --> etherEncap.lowerLayerIn;
        wlan1.uppergateOut --> etherEncap1.upperLayerIn;
        etherEncap1.lowerLayerOut --> macRelayUnitNP.lowerLayerIn++;
        macRelayUnitNP.lowerLayerOut++ --> etherEncap1.lowerLayerIn;
        etherEncap1.upperLayerOut --> wlan1.uppergateIn;

        for i=0..sizeof(ethg)-1 {
            ethg[i].phys <--> ethg[i];
            ethg[i].upperLayerOut --> macRelayUnitNP.lowerLayerIn++;
            macRelayUnitNP.lowerLayerOut++ --> ethg[i].upperLayerIn;
        }
}
