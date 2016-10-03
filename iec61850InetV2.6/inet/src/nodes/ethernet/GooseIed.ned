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
import inet.linklayer.ethernet.EtherEncap;
import inet.linklayer.ethernet.EtherMACFullDuplex;
module GooseIed
{
    @display("bgb=86,233");
    gates:
        inout ethg @labels(EtherFrame-conn);
    submodules:
        encap: EtherEncap {
            @display("p=41,111");
        }
        mac: EtherMACFullDuplex {
            @display("p=41,190");
        }
        cli: EtherAppGoose {
            @display("p=41,34");
        }
    connections:
        mac.upperLayerOut --> encap.lowerLayerIn;
        encap.lowerLayerOut --> mac.upperLayerIn;
        mac.phys <--> ethg;
        cli.out --> encap.upperLayerIn;
        encap.upperLayerOut --> cli.in;
}