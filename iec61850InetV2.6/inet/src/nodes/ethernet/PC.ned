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

import inet.applications.ethernet.EtherAppGoose;
import inet.applications.ethernet.EtherAppSv;
import inet.linklayer.ethernet.EtherEncapExt;
import inet.linklayer.ethernet.EtherMACFullDuplex;

//This module acts as a protection and control (P&C) IED acording to IEC 61850-8-1.

module PC
{
    @display("bgb=202,195;i=abstract/server");
    gates:
        inout ethg @labels(EtherFrame-conn);
    submodules:
        goMod: EtherAppGoose {
            @display("p=30,32");
        }
        mac: EtherMACFullDuplex {
            @display("p=96,146");
        }
        encap: EtherEncapExt {
            @display("p=96,78");
        }
        subs: EtherAppSv {
            @display("p=169,32");
        }
    connections:
        mac.phys <--> ethg;
        goMod.out --> encap.upperLayerIn;
        encap.upperLayerOut --> goMod.in;
        encap.lowerLayerOut --> mac.upperLayerIn;
        mac.upperLayerOut --> encap.lowerLayerIn;
        subs.out --> encap.upperLayerInSv;
        encap.upperLayerOutSv --> subs.in;
}
