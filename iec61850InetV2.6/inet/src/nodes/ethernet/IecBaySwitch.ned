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
import inet.linklayer.ethernet.EtherQoSQueue;
import inet.linklayer.ethernet.EtherQueue1q;
import inet.linklayer.ethernet.switch.MACAddressTable;
import inet.linklayer.ethernet.switch.MACRelayUnit;
import inet.status.NodeStatus;

// This is a Ethernet Switch that uses FIFO if queueType variable is empty
// and uses Fixed Priority schedule otherwise.
// I couldn't make a Switch
// That creates this type of gates automatically each time a node is attached to it (OMNeT++ says I have errors -- ???).
// Another problem is how to reference vector modules (for example for the mac's queue parameter).
// So each time a node is attached to the host the user
// must replicate the code between asteircs lines.
// In this example this is a switch with three ports.
// Remember to set mac's queue parameter if want to use external module. 
//
module IecSwitch
{
    parameters:
        @node();
        @labels(node,ethernet-node);
        @display("bgb=476,291;i=abstract/switch");
        bool hasStatus = default(false);
        string queueType = default(""); // DropTailQueue, DiffServQueue, or "" to use the internal queue
    gates:
        inout ethg[] @labels(EtherFrame-conn);
    submodules:
        macTable: MACAddressTable {
            @display("p=44,37");
        }
        relayUnit: MACRelayUnit {
            @display("p=301,37");
            gates:
                ifIn[sizeof(ethg)];
                ifOut[sizeof(ethg)];
        }
        status: NodeStatus if hasStatus {
            @display("p=44,116");
        }
		//*****************************************
        mac4: EtherMACFullDuplex {
            @display("p=435,255");            
            promiscuous = true;
        }
		//*****************************************
        mac3: EtherMACFullDuplex {
            @display("p=329,255");            
            promiscuous = true;
        }
        mac2: EtherMACFullDuplex {
            @display("p=235,255");            
            promiscuous = true;
        }
        mac1: EtherMACFullDuplex {
            @display("p=123,255");            
            promiscuous = true;
        }
		//*****************************************
        queue4: EtherQueue1q {
            @display("p=413,132");
        }
		//*****************************************
        queue3: EtherQueue1q {
            @display("p=340,164");
        }
        queue2: EtherQueue1q {
            @display("p=235,164");
        }
        queue1: EtherQueue1q {
            @display("p=162,132");
        }
    connections:
       //*****************************************
        mac1.phys <--> ethg++;
        //*****************************************
        mac2.phys <--> ethg++;
        mac3.phys <--> ethg++;
        mac4.phys <--> ethg++;

        //*****************************************
        mac1.upperLayerOut --> relayUnit.ifIn++;
        //*****************************************
        mac2.upperLayerOut --> relayUnit.ifIn++;
        mac3.upperLayerOut --> relayUnit.ifIn++;
        mac4.upperLayerOut --> relayUnit.ifIn++;
		
		//*****************************************
        relayUnit.ifOut++ --> mac1.upperLayerIn if queueType == "";
        relayUnit.ifOut++ --> queue1.in if queueType != "";
        //*****************************************

        relayUnit.ifOut++ --> mac2.upperLayerIn if queueType == "";
        relayUnit.ifOut++ --> queue2.in if queueType != "";

        relayUnit.ifOut++ --> mac3.upperLayerIn if queueType == "";
        relayUnit.ifOut++ --> queue3.in if queueType != "";

        relayUnit.ifOut++ --> mac4.upperLayerIn if queueType == "";
        relayUnit.ifOut++ --> queue4.in if queueType != "";

		//*****************************************
        queue1.out --> mac1.upperLayerIn if queueType != "";
        //*****************************************
        queue2.out --> mac2.upperLayerIn if queueType != "";
        queue3.out --> mac3.upperLayerIn if queueType != "";
        queue4.out --> mac4.upperLayerIn if queueType != "";
}
