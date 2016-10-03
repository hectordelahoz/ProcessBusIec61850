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
//
// A simple traffic generator for the GOOSE publisher model,
// generally is a L2 model that accepts Ieee80211Ctrl control info
// on the packets. It generates ~EtherGOOSE packets. It should be connected
// directly to ~EtherEncap or a Ieee802NicXXX module.
//
simple EtherAppGoose
{
    parameters:
        //Module Parameter
        string destAddress = default("");  // destination MAC address, or module path name of destination station; empty means off        
        double startTime @unit(s) = default(this.sendInterval);  // time of sending the first request
        double stopTime @unit(s) = default(-1s);  // time of finishing sending, negative values mean forever
        double sendInterval @unit(s) = default(16400e-3s);  // interval between sending requests ex: 16400ms
        string iedType = default("pc"); //pc -- Protection And Control (PDIS and PCTR), inter --Interlocking (CSWI-CILO),com -- Command (XCBR).

        //IEC61850 Parameters
        string cbRef = default(""); //"example: IEDScout/LLN0$GO$EVal"
        string dataSetRef = default(""); //"example: IEDScout/LLN0$Eval$Eval_DataSet"
        string goID = default(""); //"example: GOOSEID"        
        
        double myAppID = default(0x3FFF);
        //double pcAppID = default(0x3FFF);
        //double interAppID = default(0x3FFE);
        //double comAppID = default(0x3FFD);
        
        double pcAppID0 = default(0x0000);
        double pcAppID1 = default(0x0000);
        double pcAppID2 = default(0x0000);
        
        double interAppID0 = default(0x0000);
        double interAppID1 = default(0x0000);
        
        double comAppID0 = default(0x0000);
        double comAppID1 = default(0x0000);
        double comAppID2 = default(0x0000);
        
                
        
        double confRev = default(1);
        double numDataSet = default(1);        
        bool tst = default(true);
        bool ndsCom = default(false);
        //double bStringDataSet = default(1088);        
        //bool boolDataSet = default(true);
       // double intDataSet = default(1);
       
       //IEEE 802.1q Parameter
        double priority	= default(4);
        double vlanid	= default(0);
       
        

        @display("i=block/fork");
        @signal[sentPk](type=cPacket);
        @signal[rcvdPk](type=cPacket);
        @statistic[rcvdPk](title="packets received"; source=rcvdPk; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[sentPk](title="packets sent"; source=sentPk; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="messageAge(rcvdPk)"; unit=s; record=histogram,vector; interpolationmode=none);
    gates:
        input in @labels(Ieee802Ctrl/up);
        output out @labels(Ieee802Ctrl/down);
}
