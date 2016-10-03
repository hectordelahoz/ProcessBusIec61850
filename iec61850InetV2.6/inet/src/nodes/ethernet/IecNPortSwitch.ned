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

package inet.nodes.ethernet;

import inet.linklayer.ethernet.switch.MACAddressTable;
import inet.linklayer.ethernet.switch.MACRelayUnit;
import inet.status.NodeStatus;

//
// This is a compound module that encompass a switch gate module (@Gate) 
// a RelayUnit and an Address Table.
// Just like @IecFifoSwitch Module, this is a normal Ethernet Switch that uses Fixed Priority as schedule politic.
//

module IecNportSwitch
{
    parameters:
        @display("bgb=645,244;i=device/hub");
        bool hasStatus = default(false);
    gates:
        inout port[];
    submodules:
        gate[sizeof(port)]: Gate {
            @display("p=116,186,r,80,80,80");
        }

        relayUnit: MACRelayUnit {
            @display("p=362,37");
            gates:
                ifIn[sizeof(port)];
                ifOut[sizeof(port)];
        }
        macTable: MACAddressTable {
            @display("p=28,113");
        }
        status: NodeStatus if hasStatus {
            @display("p=20,29");
        }
    connections:
        for i=0..sizeof(port)-1 {
            gate[i].fromRelayUnit <-- relayUnit.ifOut++;
            gate[i].toRelayUnit --> relayUnit.ifIn++;
            gate[i].toMac <--> port[i];
        }
}
