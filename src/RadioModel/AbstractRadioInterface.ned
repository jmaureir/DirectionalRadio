// INRIA 2009. Juan-Carlos Maureira
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

package DirectionalRadio.RadioModel;

//
// Abstract Radio Interface to allow polymorphism in Wireless NIC
// All radio modules comming from AbstractRadio should extends this
// interface
//
moduleinterface AbstractRadioInterface {
    gates:
        input uppergateIn;   // from higher layer protocol (MAC)
        output uppergateOut; // to decider (decider connects to higher layer protocol, i.e. the MAC)
        input radioIn; // to receive frames (AirFrame) on the radio channel
}
