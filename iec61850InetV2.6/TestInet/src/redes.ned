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

package testinet;

import inet.nodes.ethernet.EtherHost2;
import inet.nodes.ethernet.EtherSwitch;
import inet.nodes.ethernet.GooseIed;
import inet.nodes.ethernet.IecFifoSwitch;
import inet.nodes.ethernet.IecNportSwitch;
import inet.nodes.ethernet.IecSwitch;
import inet.nodes.ethernet.PC;
import inet.nodes.ethernet.mu;
import ned.DatarateChannel;


//
// TODO documentation
//
network Network
{
    parameters:
        @display("bgb=714,199");
        int h = default(5);
    submodules:
        HostA: EtherHost2 {
            @display("p=55,66");
        }
        HostB: EtherHost2 {
            @display("p=304,76");
        }

        HostB1: EtherHost2 {
            @display("p=94,157");
        }

        Host[h]: EtherHost2 {
            @display("p=265,157,r,150");
        }

        iecNportSwitch: IecNportSwitch {
            @display("p=179,34");
        }
    connections:

        iecNportSwitch.port++ <--> DatarateChannel {  datarate = default(100Mbps); } <--> HostA.ethg;
        iecNportSwitch.port++ <--> DatarateChannel {  datarate = default(100Mbps); } <--> HostB.ethg;
        iecNportSwitch.port++ <--> DatarateChannel {  datarate = default(100Mbps); } <--> HostB1.ethg;
        for i=0..h-1 {
            iecNportSwitch.port++ <--> DatarateChannel {  datarate = default(100Mbps); } <--> Host[i].ethg;
        }
}

//
// TODO documentation
//
network Bay
{
    @display("bgb=269,189");
    submodules:
        MU: mu {
            @display("p=43,74");
        }
        PC: PC {
            @display("p=217,74");
        }
        inter: GooseIed {
            @display("p=82,152");
        }
        com: GooseIed {
            @display("p=174,152");
        }
        iecNportSwitch: IecNportSwitch {
            @display("p=135,42");
        }
    connections:
        iecNportSwitch.port++ <--> DatarateChannel {  datarate = default(100Mbps); } <--> MU.ethg;
        iecNportSwitch.port++ <--> DatarateChannel {  datarate = default(100Mbps); } <--> inter.ethg;
        iecNportSwitch.port++ <--> DatarateChannel {  datarate = default(100Mbps); } <--> com.ethg;
        iecNportSwitch.port++ <--> DatarateChannel {  datarate = default(100Mbps); } <--> PC.ethg;
}
//
// TODO documentation
//
network BayFifo
{
    @display("bgb=269,189");
    submodules:
        MU: mu {
            @display("p=43,74");
        }
        PC: PC {
            @display("p=217,74");
        }
        inter: GooseIed {
            @display("p=82,152");
        }
        com: GooseIed {
            @display("p=174,152");
        }
        iecFifoSwitch: IecFifoSwitch {
            @display("p=135,35");
        }
    connections:
        iecFifoSwitch.ethg++ <--> DatarateChannel {  datarate = default(100Mbps); } <--> MU.ethg;
        iecFifoSwitch.ethg++ <--> DatarateChannel {  datarate = default(100Mbps); } <--> inter.ethg;
        iecFifoSwitch.ethg++ <--> DatarateChannel {  datarate = default(100Mbps); } <--> com.ethg;
        iecFifoSwitch.ethg++ <--> DatarateChannel {  datarate = default(100Mbps); } <--> PC.ethg;
}
//
// TODO documentation
//
network T1_1
{
    @display("bgb=580,435;i=block/control");
    submodules:
        SwC: IecNportSwitch {
            @display("p=289,215");
        }
        Sw1: IecNportSwitch {
            @display("p=149,260");
        }
        Sw2: IecNportSwitch {
            @display("p=433,260");
        }
        Sw3: IecNportSwitch {
            @display("p=289,135");
        }
        pc2: PC {
            @display("p=472,375");
        }
        pc1: PC {
            @display("p=110,375");
        }
        pc3: PC {
            @display("p=227,57");
        }
        mu1: mu {
            @display("p=188,375");
        }
        mu2: mu {
            @display("p=400,375");
        }
        mu3: mu {
            @display("p=361,135");
        }
        inter1: GooseIed {
            @display("p=82,292");
        }
        inter2: GooseIed {
            @display("p=512,292");
        }
        inter3: GooseIed {
            @display("p=227,135");
        }
        com1: GooseIed {
            @display("p=227,292");
        }
        com2: GooseIed {
            @display("p=348,292");
        }
        com3: GooseIed {
            @display("p=328,57");
        }
        SCADA: EtherHost2 {
            @display("p=512,174");
        }
    connections:
        mu1.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> Sw1.port++;
        inter1.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> Sw1.port++;
        com1.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> Sw1.port++;
        pc1.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> Sw1.port++;
        
        mu2.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> Sw2.port++;
        inter2.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> Sw2.port++;
        com2.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> Sw2.port++;
        pc2.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> Sw2.port++;
        
        mu3.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> Sw3.port++;
        inter3.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> Sw3.port++;
        com3.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> Sw3.port++;
        pc3.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> Sw3.port++;
        
        Sw1.port++ <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SwC.port++;
        Sw2.port++ <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SwC.port++;
        Sw3.port++ <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SwC.port++;        
        SwC.port++ <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SCADA.ethg;
}

//
// TODO documentation
//
network T1_1_ECO
{
    @display("bgb=580,435;i=block/control");
    submodules:
        SwC: IecNportSwitch {
            @display("p=289,215");
        }
        pc2: PC {
            @display("p=450,319");
        }
        pc1: PC {
            @display("p=92,294");
        }
        pc3: PC {
            @display("p=225,75");
        }
        mu1: mu {
            @display("p=150,357");
        }
        mu2: mu {
            @display("p=399,361");
        }
        mu3: mu {
            @display("p=322,67");
        }
        inter1: GooseIed {
            @display("p=96,207");
        }
        inter2: GooseIed {
            @display("p=490,245");
        }
        inter3: GooseIed {
            @display("p=138,120");
        }
        com1: GooseIed {
            @display("p=237,383");
        }
        com2: GooseIed {
            @display("p=321,385");
        }
        com3: GooseIed {
            @display("p=433,84");
        }
        SCADA: EtherHost2 {
            @display("p=482,165");
        }
    connections:
        mu1.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SwC.port++;
        mu2.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SwC.port++;
        mu3.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SwC.port++;
        inter1.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SwC.port++;
        com1.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SwC.port++;
        pc1.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SwC.port++;
        inter2.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SwC.port++;
        com2.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SwC.port++;
        pc2.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SwC.port++;
        inter3.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SwC.port++;
        com3.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SwC.port++;
        pc3.ethg <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SwC.port++;
        SwC.port++ <--> DatarateChannel { @display("ls=,3"); datarate = default(100Mbps); } <--> SCADA.ethg;
}

