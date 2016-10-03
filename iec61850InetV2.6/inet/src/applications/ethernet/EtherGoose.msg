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

cplusplus {{
#include "INETDefs.h"
#include "EtherGooseStruct.h"
}}

struct stGoAPDUStart;
struct stGocbRef;
struct stGoTTL;
struct stGoDataSet;
struct stGoID;
struct stGoUtc;
struct stGoStNum;
struct stGoSqNum;
struct stGoTst;
struct stGoConfRev;
struct stGoNdsCom;
struct stGoNumDataSet;
struct stGoDataStart;
struct stGoDataEntries;

//
// GOOSE packet sent by ~EtherGOOSEPub. Contains the dataset phsdMeas1 defined in IEC61850-8-1
//
packet EtherGooseData
{	uint16_t			hdrAppID;			//Unique application ID, diferenciate GOOSE de SV
    uint16_t			totalLenght;		//Total Packet Sizer APDU(m) + Header(8) 
    uint16_t			reserved1;			//Reserved 1
    uint16_t			reserved2;			//Reserved 2
    //----------------------------------------------------------------------------------------------------------
    stGoAPDUStart		apduStart;			//Start of APDU sequence and Size
    stGocbRef			goCbRef;			//Control Block Ref
    stGoTTL				goTtl;				//Time To Live 
    stGoDataSet			dataSetRef;			//Data Set Reference IED/LN.DataSet 			
    stGoID				goOptID;			//Name GOOSEID
    stGoUtc				goUTC;				//Time when packet was generated
    stGoStNum			goStNum;			//Number of State changes
    stGoSqNum			goSqNum;			//Increase each time a packet is generated
    stGoTst				goTst;				//Test
    stGoConfRev			goConfRev;			//Changes in Configuration
    stGoNdsCom			goNdsCom;
    stGoNumDataSet		goNumDataSet;		//Quantity of Data in the DataSet
    stGoDataStart		goDataStart;
   	stGoDataEntries		goData[1];			//Content following 9-2LE phsMeas1 dataset
}