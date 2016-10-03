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

package inet.linklayer.ethernet;

//
// Simple classifier, that forwards 802.1Q
// to the PRIORITYOut gates, and other frames to the defaultOut
// gate.
//
// This component can be used to build a QoS queue that gives
// the 802.1Q frames the highest priority.
//
simple Ether1qClassifier
{
    parameters:
        @display("i=block/classifier");
    gates:
        input In[];
        output Out[];
        output defaultOut;
}