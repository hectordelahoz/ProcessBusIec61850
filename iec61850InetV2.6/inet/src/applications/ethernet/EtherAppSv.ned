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

package inet.applications.ethernet;

// A simple traffic generator for the SV publisher model,
// generally is a L2 model that accepts Ieee80211Ctrl control info
// on the packets. It generates ~EtherSv packets. It should be connected
// directly to ~EtherEncap or a ~EtherEncapExt module.
// when not generating SV pakets should be in the same compound module with a EtherAppGoose module
simple EtherAppSv
{
    parameters:
        //Module Parameter, for now just support for Protection Application 
        string pcIed = default(".^.goMod"); // The path to the goose module .^.goMod
        string destAddress = default("");  // destination MAC address, or module path name of destination station; empty means off
        double startTime @unit(s) = default(this.sendInterval);  // time of sending the first request
        double stopTime @unit(s) = default(-1s);  // time of finishing sending, negative values mean forever
        double sendInterval @unit(s) = default(208333e-9s);  // interval between sending msg: 208333e-9 for protection, 520833e-9 for measure, 0 just receive
        double sampleInterval @unit(s) = default(208333e-9s);  // interval between adquiring samples: 208333e-9 for protection, 65104e-9 for measure.
        double samplingRate = default(4800);//80ppc/4800pps for protection or 15360pps for measure
        
        //IEC 61850 Parameter
        double noASDU 	  = default(1);// For now just one ASDU in APDU
        string cbID		  = default("hectorSampleValue");//example: hectorSampleValue
        double confRev	  = default(1);
        double synchValue = default(0);
        
        //IEEE 802.1q Parameter
        int priority	= default(4);
        int vlanid		= default(0);
        
        @display("i=block/fork");
        @signal[sentPk](type=cPacket);
        @signal[rcvdPk](type=cPacket);
        @signal[eteDelay](type=long);
        
        @statistic[rcvdPk](title="packets received"; source=rcvdPk; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[sentPk](title="packets sent"; source=sentPk; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="messageAge(rcvdPk)"; unit=s; record=histogram,vector; interpolationmode=none);
        @statistic[eteDelay](title="end-to-end average delay"; source=eteDelay; unit=s; record=vector; interpolationmode=none);
    gates:
        input  in @labels(Ieee802Ctrl/up);        
        output out @labels(Ieee802Ctrl/up);
}