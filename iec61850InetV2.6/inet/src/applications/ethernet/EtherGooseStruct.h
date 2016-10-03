/*
 * EtherGooseStruct.h
 *
 *  Created on: 16/04/2015
 *      Author: Hector
 */

#ifndef ETHERGOOSESTRUCT_H_
#define ETHERGOOSESTRUCT_H_

//----------------OTHER VALUES--------------------
#define GOOSE_MAX_REF   65 //Defined in IEC-61850-8-1 (doc pag 70/pdf pag 72)
#define GOOSE_1Q_PPC    4
#define GOOSE_1Q_DE     0
#define GOOSE_1Q_VID    0

#define GOOSE_HEADER_APP_ID                   0x3FFF //Used to distinguish between GOOSE and SV packet
#define GOOSE_HEADER_RESERVED1                0x0000 //Reserved byte 1, value defined by 9-2LE document
#define GOOSE_HEADER_RESERVED2                0x0000 //Reserved byte 1, value defined by 9-2LE documen

//---------Defining APDU size with ASN.1----------
#define GOOSE_START_OF_APDU_TAG                   0x61 //Tag of APDU following ASN.1 (Tag|Size|Value) 61 = APLICATION-ConstructedType
#define GOOSE_LENGHT_TAG                          0x82 //Tag of APDU following ASN.1 (Tag|Size|Value) 82 = APLICATION-ConstructedType
#define GOOSE_UTC_LENGHT                          0x08

//---------------ASDU SEQUENCE-------------------
/*
 * The abstract GOOSE message format shall specify the information to be included in the
 * GOOSE message. The structure of the GOOSE message shall be as specified in Table 29 of IEC-61850-7-2.
 * A GOOSE message shall at least be sent each time when a value from one or more members referenced by the DATA-SET change.
 *
 * Option 0: gcbRef -- The parameter GoCBReference shall specify the ObjectReference of the GoCB.
 *                     The service parameter GoCBReference shall be LDName/LLN0.GoCBName.
 *                     Should be a system wide unique ID of the GOOSE CONTROL BLOCK that is controlling the GOOSE message.
 *
 * Option 1: timeAllowedtoLive -- Time to wait for the next message.
 *                                Is an INTEGER in the range of 1 to 4294967295 (32 bits) and the unit should be given in ms.
 *
 * Option 2: dataSet           -- The parameter DatSet shall contain the ObjectReference of the DATA-SET (taken from the
 *                                GoCB) whose values of the members shall be transmitted .Could have the same value of the GoCRef.
 *                                Max Lenght 65 bytes.
 *
 * Option 3: goID (Optional)   -- The parameter AppID shall contain the identifier of the LOGICAL-DEVICE (taken from the
 *                                GoCB) in which the GoCB is located. Could have the same value of the GoCRef.
 *                                Max Lenght 65 bytes.
 *
 * Option 4: T                 -- The parameter T shall contain the time at which the attribute StNum was incremented.
 *                                This attribute’s type has been mapped from a ENTRYTIME to TIMESTAMP in order to allow
 *                                additional timestamp precision.
 *                                This TIMESTAMP has a size of 8 octets. It shall have the format as specified in 8.1.3.6.
 *
 * Option 5: stNum             -- The parameter StNum shall contain the counter that increments each time a GOOSE message
 *                                has been sent and a value change has been detected within the DATA-SET specified by DatSet.
 *                                The initial value for StNum shall be 1. The value of 0 shall be reserved.
 *                                This INTEGER value shall have a range of 1 to 4294967295 (32 bits).
 *
 * Option 6: sqNum             -- The parameter SqNum shall contain the counter that shall increment each time a GOOSE message
 *                                has been sent. The initial value for SqNum shall be 1. The value of 0 shall be reserved.
 *                                This INTEGER value shall have a range of 0 to 4294967295 (32 bits).
 *                                SqNum will increment for each transmission, but will rollover to a value of 1.
 *
 * Option 7: Test              -- The parameter Test shall indicate with the value of TRUE that the values of the message shall
 *                                not be used for operational purposes.
 *
 * Option 8: confRev           -- The parameter ConfRev (taken from the GoCB) shall contain the count of the number of times
 *                                that the configuration of the DATA-SET referenced by DatSet has been changed (32 bits).
 *
 * Option 9: ndsCom            -- The attribute NdsCom shall have a value of TRUE if the attribute DatSet has a value of NULL.
 *                                It shall be used to indicate that the GoCB requires further configuration.
 *                                If the number or size of values being conveyed by the elements in the DatSet referenced
 *                                DATA-SET exceeds the SCSM determined maximum number, then the NdsCom attribute shall
 *                                be set TRUE.
 *
 * Option 10: numDataSetEntries--
 *
 * Option 11: Data             --
 * */


#define GOOSE_CONTEXT_SPECIFIC_OPTION0     0x80
#define GOOSE_CONTEXT_SPECIFIC_OPTION1     0x81
#define GOOSE_CONTEXT_SPECIFIC_OPTION2     0x82
#define GOOSE_CONTEXT_SPECIFIC_OPTION3     0x83
#define GOOSE_CONTEXT_SPECIFIC_OPTION4     0x84
#define GOOSE_CONTEXT_SPECIFIC_OPTION5     0x85
#define GOOSE_CONTEXT_SPECIFIC_OPTION6     0x86
#define GOOSE_CONTEXT_SPECIFIC_OPTION7     0x87
#define GOOSE_CONTEXT_SPECIFIC_OPTION8     0x88
#define GOOSE_CONTEXT_SPECIFIC_OPTION9     0x89
#define GOOSE_CONTEXT_SPECIFIC_OPTIONA     0x8A
//------------------------------------------------
#define GOSSE_CONTEXT_SPECIFIC_CONSTRUCT   0xAB

/*Defining Structures for APDU size*/
struct stGoAPDUStart
{   char            goPdu           ;//=   GOSSE_APPLICATION = 0x61;
    unsigned char   goPduLenghtTag  ;//=   GOOSE_LENGHT_TAG = 0x82;
    uint16_t        goPduLenght     ;//=   Can be calculated from the others parameter. example in kriger = 0x006B;
};

/*Defining Structures for GOOSE MCB Reference string*/
struct stGocbRef
{   unsigned char        goCbRef            ;//=   CONTEXT_SPECIFIC_GOOSE_ASDU_SEQ_0 = 0x80;
    unsigned char        goCbRefSize        ;//    Obtained from the reference string received as parameter
    char        goCbRefValue[GOOSE_MAX_REF] ;//    Obtained as parameter
};

/*Defining Structures for GOOSE Time Allowed To live*/
struct stGoTTL
{   unsigned char       goTtl            ;//=   CONTEXT_SPECIFIC_GOOSE_ASDU_SEQ_1 = 0x81;
    unsigned char       goTtlSize        ;//    Obtained from the variable of the parameter; typically = 0x02;
    uint16_t            goTtlValue       ;//    Obtained as parameter
};

/*Defining Structures for GOOSE Data Set Reference*/
struct stGoDataSet
{   unsigned char        goDataSetRef            ;//=   CONTEXT_SPECIFIC_GOOSE_ASDU_SEQ_2 = 0x82;
    unsigned char        goDataSetRefSize        ;//    Obtained from the reference string in the .cc code
    char        goDataSetRefValue[GOOSE_MAX_REF] ;//    Obtained as parameter
};


/*Defining Structures for GOOSE ID (OPTIONAL)*/
struct stGoID
{   unsigned char       GoID                    ;//=   CONTEXT_SPECIFIC_GOOSE_ASDU_SEQ_3 = 0x83; (OPTIONAL)
    unsigned char       GoIDSize                ;//    Obtained from the reference string received as parameter
    char                GoIDValue[GOOSE_MAX_REF];//    Obtained as parameter
};

/*Defining Structures for GOOSE UTC Time*/
struct stGoUtc
{   unsigned char       goUtc            ;//=   CONTEXT_SPECIFIC_GOOSE_ASDU_SEQ_4 = 0x84;
    unsigned char       goUtcSize        ;//    ALWAYS 0x08; GOOSE_UTC_LENGHT = 0x08;
    unsigned char  goUtcValue[8]         ;//    SIM TIME converted into UTC?
};

/*Defining Structures for GOOSE State Number*/
struct stGoStNum
{   unsigned char       goStNum            ;//=   CONTEXT_SPECIFIC_GOOSE_ASDU_SEQ_5 = 0x85;
    unsigned char       goStNumSize        ;//    Obtained from the variable; typically = 0x02;
    uint32_t            goStNumValue       ;//
};

/*Defining Structures for GOOSE State Number*/
struct stGoSqNum
{   unsigned char       goSqNum            ;//=   CONTEXT_SPECIFIC_GOOSE_ASDU_SEQ_6 = 0x86;
    unsigned char       goSqNumSize        ;//    Obtained from the variable; typically = 0x01;
    uint32_t            goSqNumValue       ;//
};

/*Defining Structures for GOOSE Test*/
struct stGoTst
{   unsigned char       goTst            ;//=   CONTEXT_SPECIFIC_GOOSE_ASDU_SEQ_7 = 0x87;
    unsigned char       goTstSize        ;//    Obtained from the variable; typically = 0x01;
    uint8_t             goTstValue       ;//    Obtained as parameter
};

/*Defining Structures for GOOSE ConfRev*/
struct stGoConfRev
{   unsigned char       goConfRev            ;//=   CONTEXT_SPECIFIC_GOOSE_ASDU_SEQ_8 = 0x88;
    unsigned char       goConfRevSize        ;//    Obtained from the variable; typically = 0x02;
    uint32_t            goConfRevValue       ;//
};


/*Defining Structures for GOOSE NdsCom*/
struct stGoNdsCom
{   unsigned char       goNdsCom            ;//=   CONTEXT_SPECIFIC_GOOSE_ASDU_SEQ_9 = 0x89;
    unsigned char       goNdsComSize        ;//    Obtained from the variable; typically = 0x01;
    uint8_t             goNdsComValue       ;//    Obtained as parameter
};

/*Defining Structures for GOOSE NumDataSetEntries*/
struct stGoNumDataSet
{   unsigned char       goNumDataSet            ;//=   CONTEXT_SPECIFIC_GOOSE_ASDU_SEQ_10 = 0x8A;
    unsigned char       goNumDataSetSize        ;//    Obtained from the reference string received as parameter. Typically = 0x01;
    uint8_t             goNumDataSetValue       ;//    Obtained as parameter
};

/*Defining Structures for GOOSE DataSet Start*/
struct stGoDataStart
{   unsigned char       goDataStart              ;//=   CONTEXT_SPECIFIC_CONSTRUCT = 0xAB;
    unsigned char       goDataStartLenghtTag     ;//=   GOOSE_LENGHT_TAG = 0x82;
    uint16_t            goDataStartSize          ;//    Can be calculated. example in kriger = 0x000A;
};

/*Defining Structures for GOOSE Data Set Entries*/
struct stGoDataEntries
{   unsigned char       goDataTag               ;//=   SEE_ASN1_TAG;
    unsigned char       goDataSize              ;//    0x01;
    uint16_t            goDataValue             ;//    obtained from parameter
};




#endif /* ETHERGOOSESTRUCT_H_ */
