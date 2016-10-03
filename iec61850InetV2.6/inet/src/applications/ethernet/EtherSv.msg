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
#include "EtherSvStruct.h"
}}

//Struct defined in EtherIecStruct.h 
struct stAPDUStart;
struct stNoASDU;
struct stASDUseqStart;
struct stIndASDUseqStart;
struct stSvID;
struct stSmpCnt;
struct stConfRev;
struct stSmpSynch;
struct stDataSetStart;
struct stAData;

//
// SV packet sent by ~EtherSvPub. Contains the dataset phsdMeas1 defined in IEC61850-9-2le
//
packet EtherSvData
{	uint16_t			hdrAppID;			//Unique application ID, diferenciate GOOSE de SV
    uint16_t			totalLenght;		//Total Packet Sizer APDU(m) + Header(8) 
    uint16_t			reserved1;			//Reserved 1
    uint16_t			reserved2;			//Reserved 2
    //----------------------------------------------------------------------------------------------------------
    stAPDUStart			apduStart;			//Start of APDU sequence and Size
    stNoASDU			asduNumber;			//Number of ASDUs within APDU
    stASDUseqStart		asduSeqStart;		//ASDUs Sequence Start and Size 
    stIndASDUseqStart	firstAsdu;			//First ASDU and Size (just structure for one ASDU - proof of comcept)			
    stSvID				svID;				//ID do bloco de controle do SV (MSVCB)
    stSmpCnt			smpCnt;				//Numero de amostras acontecidas entre pulso de synch
    stConfRev			confRev;			//Controle de mudancas de configuracao no MSVCB
    stSmpSynch			smpSynch;			//Indica se as amostras sao ou nao sincronizadas com relogio
    stDataSetStart		dataSetStart;		//DataSet Start and Size			
   	stAData				phsMeasOne[8];		//Content following 9-2LE phsMeas1 dataset (4 current and 4 voltage)
}