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

import inet.linklayer.ethernet.EtherMACFullDuplex;
import inet.linklayer.ethernet.switch.MACAddressTable;
import inet.linklayer.ethernet.switch.MACRelayUnit;
import inet.status.NodeStatus;

//This is a normal Ethernet Switch that uses FIFO as schedule politic
module IecFifoSwitch
{
    parameters:
        @node();
        @labels(node,ethernet-node);
        @display("bgb=406,276;i=abstract/switch");
        bool hasStatus = default(false);
    gates:
        inout ethg[] @labels(EtherFrame-conn);
    submodules:
        mac[sizeof(ethg)]: EtherMACFullDuplex {
            @display("p=116,171,m,3,80,80");
            promiscuous = true;
        }
        macTable: MACAddressTable {
            @display("p=44,37");
        }
        relayUnit: MACRelayUnit {
            @display("p=116,37");
            gates:
                ifIn[sizeof(ethg)];
                ifOut[sizeof(ethg)];
        }

        status: NodeStatus if hasStatus {
            @display("p=44,116");
        }
    connections:
        //mac.phys <--> ethg++;
        for i=0..sizeof(ethg)-1 {
            mac[i].upperLayerIn <-- relayUnit.ifOut++;
            mac[i].upperLayerOut --> relayUnit.ifIn++;
            mac[i].phys <--> ethg[i];
        }
}
