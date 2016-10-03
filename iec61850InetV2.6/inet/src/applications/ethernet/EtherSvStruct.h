/*
 * EtherSvStruct.h
 *
 *  Created on: 20/04/2015
 *      Author: Hector
 */

#ifndef ETHERSVSTRUCT_H_
#define ETHERSVSTRUCT_H_

//  Defining some values used by ASN.1 to codify SMV information.
//  For more see 9-2 document in the project folder (SMV ASN.1 esquema definition).
//  Exist a diagrmam with an example of a SMV packet. This code is based in that example.
//  ANS.1 can be implemented as a separate layer, passsing the value and creating function such as:
//  codeInt(), codeSize(), etc. The present code is just a proof of concept.

//Defining header (within app layer) between IEEE802.1q and SMV APDU Sequence

#define SV_HEADER_APP_ID                   0x4000 //Used to distinguish between GOOSE and SV packet
#define SV_HEADER_RESERVED1                0x8000 //Reserved byte 1, value defined by 9-2LE document
#define SV_HEADER_RESERVED2                0x0000 //Reserved byte 1, value defined by 9-2LE document

//---------Defining APDU size with ASN.1----------
#define SV_START_OF_APDU_TAG                   0x60 //Tag of APDU following ASN.1 (Tag|Size|Value) 60 = APLICATION-ConstructedType
#define SV_LENGTH_TAG                          0x82 //APDU size using ASN.1 82 = CONTEXTSPECIFIC-PrimitiveType-Int
#define SV_CONS_LENGTH_TAG                     0xA2 //Size of ASDU Sequence A2 =  CONTEXTSPECIFIC-ConstructedType
#define SV_ASDU_SEQ_LENGTH_TAG                 0x30 //Size of individual ASDU

//----------------APDU SEQUENCE-------------------
//Option 0: Number (N) of ASDU within the APDU (Mandatory) The same value of ASDU SEQUENCE OPTION 0
//Option 1: Sequrity (Optional), not defined
//Option 2: Start of ASDU Sequence (SIZE OF N-ASDU) (Mandatory) The same value of ASDU CONSTRUCTED
//---------------ASDU SEQUENCE-------------------
/* Option 0: MSVID (visible string) -- Should be a system wide unique ID. NS3Generated
 *
 * Option 2: SmpCnt (uint16_t) -- Will be incremented each time a new sampling value is taken.
 *                                The counter shall be set to zero if the sampling is synchronized by clock
 *                                signal (SmpSynch = TRUE) and the synchronizing signal occurs.
 *
 * Option 3: ConfRev (uint32_t)-- The attribute ConfRev shall represent a count of the number of times that the configuration of
 *                                the DATA-SET referenced by DatSet has been changed. Changes that shall be counted are:
 *                                – any deletion of a member of the DATA-SET; and
 *                                – the reordering of members of the DATA-SET.
 *                                The counter shall be incremented when the configuration changes.
 *                                NOTE Configuration changes of DATA-SETs due to processing of services are not allowed (see DATA-SET model).
 *                                Changes to be taken into account for the ConfRev are those made by local means like system configuration.
 *
 * Option 5: SmpSynch (Boolean)-- TRUE = SV are synchronized by a clock signal. FALSE = SV are not synchronized.
 *
 * Option 7: Samples -- List of data values related to data set definition
 */

#define SV_CONTEXT_SPECIFIC_OPTION0     0x80
#define SV_CONTEXT_SPECIFIC_OPTION2     0x82
#define SV_CONTEXT_SPECIFIC_OPTION3     0x83
#define SV_CONTEXT_SPECIFIC_OPTION5     0x85
#define SV_CONTEXT_SPECIFIC_OPTION7     0x87
//------------------------------------------------

//----------------OTHER VALUES--------------------
#define SV_MAX_ID   65
#define SV_1Q_PPC   4
#define SV_1Q_DE    0
#define SV_1Q_VID   0

/*Defining Structures for APDU size*/
struct stAPDUStart
{   char            savPdu     ;//=   SV_START_OF_APDU_TAG = 0x60
    unsigned char   savPduSize ;//=   SV_LENGTH_TAG = 0x82
    uint16_t        savPduValue;//=   Can be calculated from the others parameter. example in Sv Diagram 0x006C;
};


/*Defining Structures for number of ASDU in APDU*/
struct stNoASDU
{   unsigned char        noASDU     ;//=   SV_CONTEXT_SPECIFIC_OPTION0 (0x80)
    char                 noASDUSize ;//=   Obtained form the parameter (example in Sv Diagram 0x01)
    uint16_t             noASDUValue;//=   Obtained as parameter (example in Sv Diagram 0x01)
};

/*Defining Structures for ASDU sequence size*/
struct stASDUseqStart
{   unsigned char        seqSASDU     ;//=   SV_CONS_LENGTH_TAG          (0xA2);
    unsigned char        seqSASDUSize ;//=   SV_CONTEXT_SPECIFIC_OPTION2 (0x82);
    uint16_t             seqSASDUValue;//=   Can be calculated from the others parameter. example in Sv Diagram 0x0065;
};

/*Defining Structures for one ASDU size*/
struct stIndASDUseqStart
{   unsigned char        seqASDU    ;//= SV_ASDU_SEQ_LENGTH_TAG   (0x30);
    uint8_t              seqASDUSize;//= Can be calculated from the others parameter. example in Sv Diagram 0x63
};

/*Defining Structures for SV ID string*/
struct stSvID
{   unsigned char        svID            ;// = SV_CONTEXT_SPECIFIC_OPTION0     0x80;
    unsigned char        svIDSize        ;// = Obtained from the reference string received as parameter
    char      svIDValue[SV_MAX_ID]       ;//   Obtained as parameter
};

/*Defining Structures for number of SV without synchronization*/
struct stSmpCnt
{   unsigned char        smpCnt          ;// = SV_CONTEXT_SPECIFIC_OPTION2     0x82;
    unsigned char        smpCntSize      ;// = Obtained form the Variable (example in Sv Diagram 0x02);
    uint16_t             smpCntValue     ;// = Variable counting transmitted package.
};

/*Defining Structures for number of configuration change in the Control Block*/
struct stConfRev
{   unsigned char        confRev          ;// = SV_CONTEXT_SPECIFIC_OPTION3     0x83;
    unsigned char        confRevSize      ;// = Obtained from parameter or variable. (example in Sv Diagram 0x04);
    uint32_t             confRevValue     ;// = Obtained as a parameter (example in Sv Diagram 0x000001);
};

/*Defining Structures for indicating that the device use or not synch clock*/
struct stSmpSynch
{   unsigned char        smpSynch          ;// = SV_CONTEXT_SPECIFIC_OPTION5     0x85;
    unsigned char        smpSynchSize      ;// = Obtained from parameter (example in Sv Diagram 0x01);
    uint8_t              smpSynchValue     ;// = Obtained as a parameter (example in Sv Diagram 0x01);
};

/*Defining Structures for data set size*/
struct stDataSetStart
{   unsigned char        dataSet     ;// = SV_CONTEXT_SPECIFIC_OPTION7     0x87;
    uint8_t              dataSetSize ;// = Can be calculated from the others parameter. example in Sv Diagram  0x40;
};


/*Defining Structures for DataSet fields following PhsMeas1 from IEC61850-9-2LE*/

//Quality Structure (Review Quality Definition in IEC-61850-7-3)
struct stQData1
{   char        empty1      :8;
    char        empty1a     :8;
    char        empty2      :2;
    char        der         :1; // derived???
    char        opB         :1; // Blocked by operator
    char        test        :1; // Test
    char        source      :1; // Source
    char        DetailQua   :2; // Inaccurate | Inconsistent
};

//Quality Structure (Review Quality Definition in IEC-61850-7-3)
struct stQData2
{   char        DetailQua   :6; // Old Data|Failure|Oscillatory|Bad Reference|Out of Range|OverFlow
    char        validity    :2; //Validity
};

//Quality Structure (Review Quality Definition in IEC-61850-7-3)
struct stQuality
{   stQData1    firstQByte;
    stQData2    secondQByte;
};

//Analog Data Structure (Review Quality Definition in IEC-61850-7-3)
struct stAData
{   int32_t     aValue;
    stQuality   quality;
};


#endif /* ETHERSVSTRUCT_H_ */
