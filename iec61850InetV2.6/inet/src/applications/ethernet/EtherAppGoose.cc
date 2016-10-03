/*
 * EtherGooseApp.cc
 *
 *  Created on: 17/04/2015
 *      Author: Hector
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <simutil.h>

#include "EtherAppGoose.h"
#include "EtherGoose_m.h"
#include "EtherGooseStruct.h"
#include "Ieee802Ctrl_m.h"
#include "NodeOperations.h"
#include "ModuleAccess.h"
#include "Asn1.h"


Define_Module(EtherAppGoose);

simsignal_t EtherAppGoose::sentPkSignal = registerSignal("sentPk");
simsignal_t EtherAppGoose::rcvdPkSignal = registerSignal("rcvdPk");


EtherAppGoose::EtherAppGoose()
{
    timerMsg = NULL;
    nodeStatus = NULL;
    //nodeType;
}

EtherAppGoose::~EtherAppGoose()
{
    cancelAndDelete(timerMsg);
}

void EtherAppGoose::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == 0)
    {   alarmState = false;
        WATCH(alarmState);

        myAppID = par("myAppID").doubleValue();

        pcAppID[0] = par("pcAppID0").doubleValue();
        pcAppID[1] = par("pcAppID1").doubleValue();
        pcAppID[2] = par("pcAppID2").doubleValue();

        interAppID[0] = par("interAppID0").doubleValue();
        interAppID[1] = par("interAppID1").doubleValue();

        comAppID[0] = par("comAppID0").doubleValue();
        comAppID[1] = par("comAppID1").doubleValue();
        comAppID[2] = par("comAppID2").doubleValue();

        /*pcAppID = par("pcAppID").doubleValue();
        interAppID = par("interAppID").doubleValue();
        comAppID = par("comAppID").doubleValue();//*/

        sendInterval = par("sendInterval").doubleValue();
        WATCH(sendInterval);
        sInterval = sendInterval;

        seqNum = 0;
        stChng = 1;
        WATCH(seqNum);

        // statistics
        packetsSent = packetsReceived = 0;
        WATCH(packetsSent);
        WATCH(packetsReceived);

        startTime = par("startTime");
        stopTime = par("stopTime");
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            error("Invalid startTime/stopTime parameters");

        strcpy(nodeType,par("iedType"));
        printf("GOSSE IED TYPE = %s\n",nodeType);
        if (nodeType == NULL)
                    error("Invalid iedType parameter");

        ppc = par("priority").doubleValue();
        vid = par("vlanid").doubleValue();
    }
    else if (stage == 3)
    {
        if (isGenerator())
            timerMsg = new cMessage("generateNextGossePacket");

        nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));

        if (isNodeUp() && isGenerator())
            scheduleNextPacket(true);
    }
}

void EtherAppGoose::handleMessage(cMessage *msg)
{
    if (!isNodeUp())
        throw cRuntimeError("Application is not running");
    if (msg->isSelfMessage())
    {
        if (msg->getKind() == START)
        {
            destMACAddress = resolveDestMACAddress();

            // if no dest address given, nothing to do
            if (destMACAddress.isUnspecified())
                return;
        }
        sendPacket();
        scheduleNextPacket(false);
    }
    else
        receivePacket(check_and_cast<cPacket*>(msg));
}

bool EtherAppGoose::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    Enter_Method_Silent();
    if (dynamic_cast<NodeStartOperation *>(operation)) {
        if (stage == NodeStartOperation::STAGE_APPLICATION_LAYER && isGenerator())
            scheduleNextPacket(true);
    }
    else if (dynamic_cast<NodeShutdownOperation *>(operation)) {
        if (stage == NodeShutdownOperation::STAGE_APPLICATION_LAYER)
            cancelNextPacket();
    }
    else if (dynamic_cast<NodeCrashOperation *>(operation)) {
        if (stage == NodeCrashOperation::STAGE_CRASH)
            cancelNextPacket();
    }
    else throw cRuntimeError("Unsupported lifecycle operation '%s'", operation->getClassName());
    return true;
}

bool EtherAppGoose::isNodeUp()
{
    return !nodeStatus || nodeStatus->getState() == NodeStatus::UP;
}

bool EtherAppGoose::isGenerator()
{
    return par("destAddress").stringValue()[0];
}

void EtherAppGoose::scheduleNextPacket(bool start)
{
    simtime_t cur = simTime();
    simtime_t next;

    if (start)
    {
        next = cur <= startTime ? startTime : cur;
        timerMsg->setKind(START);
    }
    else
    {
        next = cur + sendInterval;
        timerMsg->setKind(NEXT);
        EV<<"Next "<<next<<" SendInterval "<<sendInterval<<"\n";
    }

    if (stopTime < SIMTIME_ZERO || next < stopTime)
    {   scheduleAt(next, timerMsg);
        if(sendInterval < sInterval)
        {   sendInterval = sendInterval*2;
        }
        EV<<"Next "<<next<<" SendInterval "<<sendInterval<<"\n";
    }
}

void EtherAppGoose::cancelNextPacket()
{
    if (timerMsg)
        cancelEvent(timerMsg);
}

MACAddress EtherAppGoose::resolveDestMACAddress()
{
    MACAddress destMACAddress;
    const char *destAddress = par("destAddress");
    if (destAddress[0])
    {
        // try as mac address first, then as a module
        if (!destMACAddress.tryParse(destAddress))
        {
            cModule *destStation = simulation.getModuleByPath(destAddress);
            if (!destStation)
                error("cannot resolve MAC address '%s': not a 12-hex-digit MAC address or a valid module path name", destAddress);

            cModule *destMAC = destStation->getSubmodule("mac");
            if (!destMAC)
                error("module '%s' has no 'mac' submodule", destAddress);

            destMACAddress.setAddress(destMAC->par("address"));
        }
    }
    return destMACAddress;
}


void EtherAppGoose::createGoosePacket(EtherGooseData *goosePacket)
{   uint16_t totalLenghtCounter = 0;
    uint16_t totalDataLenghtCounter = 0;

    //Filling the GOOSE packet field
    //Application ID
    uint16_t appID;
    appID = (uint16_t)myAppID;
    /*if((strcmp(nodeType,"pc")==0))
        appID = (uint16_t)pcAppID;
    else if((strcmp(nodeType,"inter")==0))
        appID = (uint16_t)interAppID;
    else if((strcmp(nodeType,"com")==0))
        appID = (uint16_t)comAppID;//*/

    goosePacket->setHdrAppID(appID);
    //Reserved 1
    goosePacket->setReserved1(GOOSE_HEADER_RESERVED1);
    //Reserved 2
    goosePacket->setReserved2(GOOSE_HEADER_RESERVED2);


    stGocbRef   goCbRef;
    goCbRef.goCbRef = GOOSE_CONTEXT_SPECIFIC_OPTION0;
    const char *cbRefValue = par("cbRef");
    goCbRef.goCbRefSize = strlen(cbRefValue)+1; //0x15
    strcpy(goCbRef.goCbRefValue,cbRefValue);
    goosePacket->setGoCbRef(goCbRef);
    totalLenghtCounter += sizeof(goCbRef.goCbRef)+ sizeof(goCbRef.goCbRefSize)+goCbRef.goCbRefSize;


    stGoTTL goTTL;
    goTTL.goTtl     = GOOSE_CONTEXT_SPECIFIC_OPTION1;
    goTTL.goTtlSize = sizeof(goTTL.goTtlValue);
    goTTL.goTtlValue = (double)(1000*sendInterval);
    goosePacket->setGoTtl(goTTL);
    totalLenghtCounter += sizeof(goTTL.goTtl)+ sizeof(goTTL.goTtlSize)+goTTL.goTtlSize;


    stGoDataSet goDataSet;
    goDataSet.goDataSetRef = GOOSE_CONTEXT_SPECIFIC_OPTION2;
    const char *DataSetRef = par("dataSetRef");
    goDataSet.goDataSetRefSize = strlen(DataSetRef)+1;//0x1A;
    strcpy(goDataSet.goDataSetRefValue,DataSetRef);
    goosePacket->setDataSetRef(goDataSet);
    totalLenghtCounter += sizeof(goDataSet.goDataSetRef)+ sizeof(goDataSet.goDataSetRefSize)+goDataSet.goDataSetRefSize;


    stGoID goID;
    goID.GoID = GOOSE_CONTEXT_SPECIFIC_OPTION3;
    const char *GoId = par("goID");
    goID.GoIDSize = strlen(GoId)+1;//0X07;
    strcpy(goID.GoIDValue,GoId);
    goosePacket->setGoOptID(goID);
    totalLenghtCounter += sizeof(goID.GoID)+ sizeof(goID.GoIDSize)+goID.GoIDSize;


    //TODO: Convert Actual Time into UTC
    stGoUtc goUtc;
    goUtc.goUtc = GOOSE_CONTEXT_SPECIFIC_OPTION4;
    goUtc.goUtcSize = GOOSE_UTC_LENGHT;
    goUtc.goUtcValue[0] = 0x00;
    goUtc.goUtcValue[1] = 0xB7;
    goUtc.goUtcValue[2] = 0x2D;
    goUtc.goUtcValue[3] = 0xB2;
    goUtc.goUtcValue[4] = 0xD3;
    goUtc.goUtcValue[5] = 0x3C;
    goUtc.goUtcValue[6] = 0x9C;
    goUtc.goUtcValue[7] = 0x4E;
    goosePacket->setGoUTC(goUtc);
    totalLenghtCounter += sizeof(goUtc.goUtc)+ sizeof(goUtc.goUtcSize)+goUtc.goUtcSize;

    stGoStNum goStNum;
    goStNum.goStNum = GOOSE_CONTEXT_SPECIFIC_OPTION5;
    goStNum.goStNumSize = 1;//sizeof(seqNum);
    goStNum.goStNumValue = stChng;
    goosePacket->setGoStNum(goStNum);
    totalLenghtCounter += sizeof(goStNum.goStNum)+ sizeof(goStNum.goStNumSize)+goStNum.goStNumSize;


    stGoSqNum goSqNum;
    goSqNum.goSqNum = GOOSE_CONTEXT_SPECIFIC_OPTION6;
    goSqNum.goSqNumSize = 1;//sizeof(seqNum);
    goSqNum.goSqNumValue = (uint32_t)seqNum;
    goosePacket->setGoSqNum(goSqNum);
    totalLenghtCounter += sizeof(goStNum.goStNum)+ sizeof(goStNum.goStNumSize)+goStNum.goStNumSize;


    stGoTst goTst;
    goTst.goTst = GOOSE_CONTEXT_SPECIFIC_OPTION7;
    uint8_t Tst = (uint8_t)par("tst").boolValue();
    goTst.goTstSize = sizeof(Tst);
    goTst.goTstValue = Tst;
    goosePacket->setGoTst(goTst);
    totalLenghtCounter += sizeof(goTst.goTst)+ sizeof(goTst.goTstSize)+goTst.goTstSize;


    stGoConfRev goConfRev;
    goConfRev.goConfRev     = GOOSE_CONTEXT_SPECIFIC_OPTION8;
    uint32_t ConfRev = par("confRev").doubleValue();
    goConfRev.goConfRevSize = 1;//sizeof(ConfRev);
    goConfRev.goConfRevValue = ConfRev;
    goosePacket->setGoConfRev(goConfRev);
    totalLenghtCounter += sizeof(goConfRev.goConfRev)+ sizeof(goConfRev.goConfRevSize)+goConfRev.goConfRevSize;


    stGoNdsCom goNdsCom;
    goNdsCom.goNdsCom   = GOOSE_CONTEXT_SPECIFIC_OPTION9;
    uint8_t NdsCom = (uint8_t)par("ndsCom").boolValue();
    goNdsCom.goNdsComSize = sizeof(NdsCom);
    goNdsCom.goNdsComValue = NdsCom;
    goosePacket->setGoNdsCom(goNdsCom);
    totalLenghtCounter += sizeof(goNdsCom.goNdsCom)+ sizeof(goNdsCom.goNdsComSize)+goNdsCom.goNdsComSize;


    stGoNumDataSet goNumDataSet;
    goNumDataSet.goNumDataSet = GOOSE_CONTEXT_SPECIFIC_OPTIONA;
    uint8_t NumDataSet = (uint8_t)par("numDataSet").doubleValue();
    goNumDataSet.goNumDataSetSize = sizeof(NumDataSet);
    goNumDataSet.goNumDataSetValue = NumDataSet;
    goosePacket->setGoNumDataSet(goNumDataSet);
    totalLenghtCounter += sizeof(goNumDataSet.goNumDataSet)+ sizeof(goNumDataSet.goNumDataSetSize)+goNumDataSet.goNumDataSetSize;

    stGoDataEntries boolean;
    boolean.goDataTag = ASN1_CONTEXT_SPECIFIC_BOOL;
    uint8_t boolDataSet = (uint8_t)alarmState;//(uint8_t)par("boolDataSet").boolValue();
    boolean.goDataSize = sizeof(boolDataSet);
    boolean.goDataValue = boolDataSet;
    goosePacket->setGoData(0,boolean);
    totalDataLenghtCounter += sizeof(boolean.goDataTag)+ sizeof(boolean.goDataSize)+boolean.goDataSize;


    //Set Data Entries
    /*stGoDataEntries boolean;
    boolean.goDataTag = ASN1_CONTEXT_SPECIFIC_BOOL;
    uint8_t boolDataSet = (uint8_t)par("boolDataSet").boolValue();
    boolean.goDataSize = sizeof(boolDataSet);
    boolean.goDataValue = boolDataSet;
    goosePacket->setGoData(0,boolean);
    totalDataLenghtCounter += sizeof(boolean.goDataTag)+ sizeof(boolean.goDataSize)+boolean.goDataSize;


    stGoDataEntries inter;
    inter.goDataTag = ASN1_CONTEXT_SPECIFIC_INT;
    uint8_t intDataSet = (uint8_t)par("intDataSet").doubleValue();
    inter.goDataSize = sizeof(intDataSet);
    inter.goDataValue = intDataSet;
    goosePacket->setGoData(1,inter);
    totalDataLenghtCounter += sizeof(inter.goDataTag)+ sizeof(inter.goDataSize)+inter.goDataSize;


    stGoDataEntries btstring;
    btstring.goDataTag = ASN1_CONTEXT_SPECIFIC_BITSTRING;
    uint16_t bStringDataSet = (uint16_t)par("bStringDataSet").doubleValue();
    btstring.goDataSize = sizeof(bStringDataSet);
    btstring.goDataValue = bStringDataSet;
    goosePacket->setGoData(2,btstring);
    totalDataLenghtCounter += sizeof(btstring.goDataTag)+ sizeof(btstring.goDataSize)+btstring.goDataSize;
    //*/


    //Adding the DataSetEntries Lenght to the total message Lenght.
    totalLenghtCounter += totalDataLenghtCounter;

    //Set the DataSet Length (this is the start of the Data Set)
    stGoDataStart goDataStart;
    goDataStart.goDataStart = GOSSE_CONTEXT_SPECIFIC_CONSTRUCT;
    goDataStart.goDataStartLenghtTag = GOOSE_LENGHT_TAG;
    goDataStart.goDataStartSize = (uint16_t)totalDataLenghtCounter;
    goosePacket->setGoDataStart(goDataStart);
    totalLenghtCounter += sizeof(goDataStart.goDataStart)+ sizeof(goDataStart.goDataStartLenghtTag)+sizeof(goDataStart.goDataStartSize);


    //Total Message Length (this is at the start of the GOOSE apdu)
    stGoAPDUStart apduStart;
    apduStart.goPdu          = GOOSE_START_OF_APDU_TAG;
    apduStart.goPduLenghtTag = GOOSE_LENGHT_TAG;
    apduStart.goPduLenght    = totalLenghtCounter;
    goosePacket->setApduStart(apduStart);
    totalLenghtCounter += sizeof(apduStart.goPdu) + sizeof(apduStart.goPduLenghtTag) + sizeof(apduStart.goPduLenght);


    //Adding 8 bytes to the counter representing the header (reserved 1, reserved 2, APPID, TotalLength)
    totalLenghtCounter += 8;
    goosePacket->setTotalLenght(totalLenghtCounter);

    /*printf("%2X ",goosePacket->getHdrAppID());
    printf("%2X", goosePacket->getTotalLenght());
    printf("%2X ",goosePacket->getReserved1());
    printf("%2X ",goosePacket->getReserved2());
    printf("%X %X %4X ", goosePacket->getApduStart().goPdu,goosePacket->getApduStart().goPduLenghtTag,goosePacket->getApduStart().goPduLenght);
    printf("%X %X %s ",goosePacket->getGoCbRef().goCbRef,goosePacket->getGoCbRef().goCbRefSize,goosePacket->getGoCbRef().goCbRefValue);
    printf("%X %X %2X ", goosePacket->getGoTtl().goTtl,goosePacket->getGoTtl().goTtlSize,goosePacket->getGoTtl().goTtlValue);
    printf("%X %X %s ", goosePacket->getDataSetRef().goDataSetRef,goosePacket->getDataSetRef().goDataSetRefSize,goosePacket->getDataSetRef().goDataSetRefValue);
    printf("%X %X %s ", goosePacket->getGoOptID().GoID,goosePacket->getGoOptID().GoIDSize,goosePacket->getGoOptID().GoIDValue);
    printf("%X %X %X %X %X %X %X %X %X %X ", goosePacket->getGoUTC().goUtc,goosePacket->getGoUTC().goUtcSize,
                                                                           goosePacket->getGoUTC().goUtcValue[7],
                                                                           goosePacket->getGoUTC().goUtcValue[6],
                                                                           goosePacket->getGoUTC().goUtcValue[5],
                                                                           goosePacket->getGoUTC().goUtcValue[4],
                                                                           goosePacket->getGoUTC().goUtcValue[3],
                                                                           goosePacket->getGoUTC().goUtcValue[2],
                                                                           goosePacket->getGoUTC().goUtcValue[1],
                                                                           goosePacket->getGoUTC().goUtcValue[0]);
    printf("%X %X %2X ", goosePacket->getGoStNum().goStNum,goosePacket->getGoStNum().goStNumSize,goosePacket->getGoStNum().goStNumValue);
    printf("%X %X %2X ", goosePacket->getGoSqNum().goSqNum,goosePacket->getGoSqNum().goSqNumSize,goosePacket->getGoSqNum().goSqNumValue);
    printf("%X %X %X ", goosePacket->getGoTst().goTst,goosePacket->getGoTst().goTstSize,goosePacket->getGoTst().goTstValue);
    printf("%X %X %X ", goosePacket->getGoConfRev().goConfRev,goosePacket->getGoConfRev().goConfRevSize,goosePacket->getGoConfRev().goConfRevValue);
    printf("%X %X %X ", goosePacket->getGoNdsCom().goNdsCom,goosePacket->getGoNdsCom().goNdsComSize,goosePacket->getGoNdsCom().goNdsComValue);
    printf("%X %X %X ", goosePacket->getGoNumDataSet().goNumDataSet,goosePacket->getGoNumDataSet().goNumDataSetSize,goosePacket->getGoNumDataSet().goNumDataSetValue);
    printf("%X %X %2X ", goosePacket->getGoDataStart().goDataStart,goosePacket->getGoDataStart().goDataStartLenghtTag,goosePacket->getGoDataStart().goDataStartSize);
    printf("%X %X %X \n", goosePacket->getGoData(0).goDataTag,goosePacket->getGoData(0).goDataSize,goosePacket->getGoData(0).goDataValue);
    //printf("%X %X %X ", goosePacket->getGoData(1).goDataTag,goosePacket->getGoData(1).goDataSize,goosePacket->getGoData(1).goDataValue);
    //printf("%X %X %2X\n", goosePacket->getGoData(2).goDataTag,goosePacket->getGoData(2).goDataSize,goosePacket->getGoData(2).goDataValue);
    //*/
}

void EtherAppGoose::sendPacket()
{   seqNum++;

    char msgname[30];
    sprintf(msgname, "req-%d-%ld", getId(), seqNum);
    EV << "Generating GOOSE packet `" << msgname << "'\n";

    EtherGooseData *goosePacket = new EtherGooseData(msgname, IEEE802CTRL_DATA);

    createGoosePacket(goosePacket);

    goosePacket->setByteLength(goosePacket->getTotalLenght());

    Ieee802Ctrl *etherctrl = new Ieee802Ctrl();
    etherctrl->setDest(destMACAddress);
    etherctrl->setEtherType(ETHERTYPE_802_1_Q);
    //etherctrl->setPpc(GOOSE_1Q_PPC);
    etherctrl->setPpc(ppc);
    etherctrl->setDe(GOOSE_1Q_DE);
    //etherctrl->setVid(GOOSE_1Q_VID);
    etherctrl->setVid(vid);
    etherctrl->setApptype(ETHERTYPE_GOOSE);

    goosePacket->setControlInfo(etherctrl);

    emit(sentPkSignal, goosePacket);
    send(goosePacket, "out");
    packetsSent++;
}

bool EtherAppGoose::getAlarmState()
{ return alarmState;
}

/* setAlarmState()
 * Set the variable alarmState that represents an abnormal condition in the system
 * it has several meaning depending on the IED type that is modeled.
 * for P&C IED the alarmState represents a detection of an out of range value for current or voltage.
 * for inter-locking IED the alarmState represents the state that the commanded breaker should have.
 * for command IED the alarmState represents the actual state of the breakers contact.
 */
void EtherAppGoose::setAlarmState()
{   Enter_Method("setAlarmState()");
    if(isGenerator())
    {   //Notify Alarm state
        EV<<"CHANGING ALARM STATE_ ANTIGO "<<alarmState <<"\n";
        alarmState = !alarmState;
        EV<<"CHANGING ALARM STATE_ NOVO "<<alarmState <<"\n";
        stChng++;

        cancelNextPacket();
        //Modify TTL
        //EV<<"CHANGING send interval "<<sendInterval <<"\n";
        sendInterval = sInterval/32;
        //Tx goose
        sendPacket();
        //Schedule Next Packet
        scheduleNextPacket(false);
    }
}

/*
 *  receivePacket(cPacket *msg)
 *  Process GOOSE msg received by the module.
 *  The modeled behavior depends on the IED modeled.
 *
 *  P&C Behavior:
 *  This IED must not process any GOOSE msg and SV are processed from an external sub-module
 *  (modeling a thread in the embedded implementation of the IED).
 *
 *  Interlocking IED Behavior:
 *  if msg came form P&C IED then verify emergency state
 *  else if msg came from command IED then verify if the contact actual state has the same value of alarmState
 *  in both cases if the alarmState differs the goose value then the behavior of the date must change.
 *  Other rules can be added in order to issue a command.
 *
 *  Command IED Behavior:
 *  just verify the actual alarmState and verify if the command require a change in it.
 */

void EtherAppGoose::receivePacket(cPacket *msg)
{
    EV << "Received packet `" << msg->getName() << "'\n";

    packetsReceived++;
    emit(rcvdPkSignal, msg);

    Ieee802Ctrl *etherctrl = check_and_cast<Ieee802Ctrl*>(msg->removeControlInfo());

    if ((etherctrl->getApptype() == ETHERTYPE_GOOSE) && isGenerator())
    {   EtherGooseData *frame = check_and_cast<EtherGooseData*>(msg);
        double frmAppID = frame->getHdrAppID();

        if((bool)(frame->getGoData(0).goDataValue & 0x01) != alarmState)
        {   int i;
            if((strcmp(nodeType,"inter")==0))
            {   //printf("getID() = %d, alarmState = %d, frame->getGoData(0).goDataValue = %d\n",getId(), alarmState, (frame->getGoData(0).goDataValue & 0x01));
                for(i = 0; i<3; i++)
                {   if(frmAppID == pcAppID[i])
                    {   setAlarmState();
                        break;
                    }
                    else if(frmAppID == comAppID[i])
                    {   cancelNextPacket();
                        //Modify TTL
                        sendInterval = sInterval/32;
                        //Tx goose
                        sendPacket();
                        //Schedule Next Packet
                        scheduleNextPacket(false);
                        break;
                    }
                }
            }
            else if(strcmp(nodeType,"com")==0)
            {   //printf("getID() = %d, alarmState = %d, frame->getGoData(0).goDataValue = %d\n",getId(), alarmState, (frame->getGoData(0).goDataValue & 0x01));
                for(i = 0; i<2; i++)
                {   if(frmAppID == interAppID[i])
                    {   setAlarmState();
                        break;
                    }
                }
            }
            else if((strcmp(nodeType,"pc")==0))
            {   //printf("getID() = %d, alarmState = %d, frame->getGoData(0).goDataValue = %d\n",getId(), alarmState, (frame->getGoData(0).goDataValue & 0x01));
                for(i = 0; i<2; i++)
                {   if(frmAppID == interAppID[i])
                    {   cancelNextPacket();
                        //Modify TTL
                        sendInterval = sInterval/32;
                        //Tx goose
                        sendPacket();
                        //Schedule Next Packet
                        scheduleNextPacket(false);
                        break;
                    }
                }
            }
        }

        /*if((strcmp(nodeType,"inter")==0))
        {   //printf("getID() = %d, alarmState = %d, frame->getGoData(0).goDataValue = %d\n",getId(), alarmState, (frame->getGoData(0).goDataValue & 0x01));

            if(frmAppID == pcAppID)
            {   if((bool)(frame->getGoData(0).goDataValue & 0x01) != alarmState)
                    setAlarmState();
            }
            else if(frmAppID == comAppID)
            {   if((bool)(frame->getGoData(0).goDataValue & 0x01) != alarmState)
                {   cancelNextPacket();
                    //Modify TTL
                    sendInterval = sInterval/32;
                    //Tx goose
                    sendPacket();
                    //Schedule Next Packet
                    scheduleNextPacket(false);
                }
            }
        }
        else if((strcmp(nodeType,"com")==0))
        {   //printf("getID() = %d, alarmState = %d, frame->getGoData(0).goDataValue = %d\n",getId(), alarmState, (frame->getGoData(0).goDataValue & 0x01));
            if(frmAppID == interAppID)
            {    if((bool)(frame->getGoData(0).goDataValue & 0x01) != alarmState)
                    setAlarmState();
            }
        }//*/
    }
    delete msg;
}

void EtherAppGoose::finish()
{
    cancelAndDelete(timerMsg);
    timerMsg = NULL;
}
