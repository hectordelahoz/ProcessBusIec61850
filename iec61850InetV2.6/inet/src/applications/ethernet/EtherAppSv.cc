/*
 * EtherAppSvSubs.cc
 *
 *  Created on: 20/04/2015
 *      Author: Hector
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <simkerneldefs.h>

#include "EtherAppSv.h"

#include "EtherSv_m.h"
#include "EtherSvStruct.h"
#include "Ieee802Ctrl_m.h"
#include "NodeOperations.h"
#include "ModuleAccess.h"


#define DANGER_PHASE_CURRENT_VALUE 120000 //120 A
#define DANGER_PHASE_VOLTAGE_VALUE 136000 //136 V

Define_Module(EtherAppSv);

simsignal_t EtherAppSv::sentPkSignal = registerSignal("sentPk");
simsignal_t EtherAppSv::rcvdPkSignal = registerSignal("rcvdPk");

simsignal_t EtherAppSv::eteDelaySignal = registerSignal("eteDelay");

EtherAppSv::EtherAppSv()
{
    sendInterval = NULL;
    timerMsg = NULL;
    timerSmp = NULL;
    nodeStatus = NULL;
    gooseModule = NULL;
    eteAvg = 0;
}

EtherAppSv::~EtherAppSv()
{
    cancelAndDelete(timerMsg);
    cancelAndDelete(timerSmp);
}

void EtherAppSv::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == 0)
    {
        sendInterval = &par("sendInterval");
        sampleInterval = par("sampleInterval").doubleValue();
        samplingRate = par("samplingRate").doubleValue();
        ppc = (int)par("priority").doubleValue();
        vid = (int)par("vlanid").doubleValue();

        seqNum = 0;
        WATCH(seqNum);

        WATCH(eteAvg);

        alarmState = 0;
        WATCH(alarmState);

        // statistics
        packetsSent = packetsReceived = 0;
        WATCH(packetsSent);
        WATCH(packetsReceived);

        startTime = par("startTime");
        stopTime = par("stopTime");
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            error("Invalid startTime/stopTime parameters");
    }
    else if (stage == 3)
    {
        //If it is Generator (MU) then it can't process SV message.
        if (isGenerator())
        {   timerSmp = new cMessage("generateNextSample");
            timerMsg = new cMessage("generateNextSvPacket");
        }
        else
        {   gooseModule = check_and_cast<EtherAppGoose *>(getModuleByPath(par("pcIed")));
            const char *msvcbID = par("pcIed");
            printf("pcIed%s \n",msvcbID);
            if(gooseModule == NULL)
                throw cRuntimeError("No GOOSE module to Process SV");
        }

        nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));//???

        if (isNodeUp() && isGenerator())
        {   scheduleNextSample(true);
            scheduleNextPacket(true);
        }
    }
}

void EtherAppSv::handleMessage(cMessage *msg)
{
    if (!isNodeUp())
        throw cRuntimeError("Application is not running");
    if (msg->isSelfMessage())
    {
        if (msg->getKind() != SAMPLE)
        {   if (msg->getKind() == START)
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
        {   getSinValue();
            scheduleNextSample(false);
        }
    }
    else
        receivePacket(check_and_cast<cPacket*>(msg));
}

bool EtherAppSv::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    Enter_Method_Silent();
    if (dynamic_cast<NodeStartOperation *>(operation)) {
        if (stage == NodeStartOperation::STAGE_APPLICATION_LAYER && isGenerator())
        {   scheduleNextSample(true);
            scheduleNextPacket(true);
        }
    }
    else if (dynamic_cast<NodeShutdownOperation *>(operation)) {
        if (stage == NodeShutdownOperation::STAGE_APPLICATION_LAYER)
        {   cancelNextSample();
            cancelNextPacket();
        }
    }
    else if (dynamic_cast<NodeCrashOperation *>(operation)) {
        if (stage == NodeCrashOperation::STAGE_CRASH)
        {   cancelNextSample();
            cancelNextPacket();
        }
    }
    else throw cRuntimeError("Unsupported lifecycle operation '%s'", operation->getClassName());
    return true;
}

bool EtherAppSv::isNodeUp()
{
    return !nodeStatus || nodeStatus->getState() == NodeStatus::UP;
}

bool EtherAppSv::isGenerator()
{
    return par("sendInterval").doubleValue();
}

void EtherAppSv::scheduleNextSample(bool start)
{
    simtime_t cur = simTime();
    simtime_t next;
    if (start)
    {   next = cur <= startTime ? startTime : cur;
    }
    else
    {   next = cur + sampleInterval;
    }
    timerSmp->setKind(SAMPLE);
    if (stopTime < SIMTIME_ZERO || next < stopTime)
        scheduleAt(next, timerSmp);
}

void EtherAppSv::scheduleNextPacket(bool start)
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
        next = cur + sendInterval->doubleValue();
        timerMsg->setKind(NEXT);
    }
    if (stopTime < SIMTIME_ZERO || next < stopTime)
        scheduleAt(next, timerMsg);
}

void EtherAppSv::cancelNextPacket()
{
    if (timerMsg)
        cancelEvent(timerMsg);
}

void EtherAppSv::cancelNextSample()
{
    if (timerSmp)
        cancelEvent(timerSmp);
}

MACAddress EtherAppSv::resolveDestMACAddress()
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

void EtherAppSv::getSinValue()
{   int i = 0;

    double currentValue = (double)10*sin((120*PI/samplingRate)*(seqNum));
    currentValue = 1000*currentValue; //Converting in miliVolts.
    double voltageValue = 12*currentValue;

    for(i = 0; i<4; i++)
    {   currentSampleValue[i] = currentValue;
        voltageSampleValue[i] = voltageValue;
    }
}

void EtherAppSv::createSvPacket(EtherSvData *smvpacket)
{   uint16_t totalLenghtCounter = 0;
    uint16_t totalDataLenghtCounter = 0;
    uint16_t totalMessLenghtCounter = 0;

    //Filling the Sample Value packet field
    //Application ID
    smvpacket->setHdrAppID(SV_HEADER_APP_ID);
    //Reserved 1
    smvpacket->setReserved1(SV_HEADER_RESERVED1);
    //Reserved 2
    smvpacket->setReserved2(SV_HEADER_RESERVED2);

    //Number of ASDU
    stNoASDU noAsdu;
    noAsdu.noASDU = SV_CONTEXT_SPECIFIC_OPTION0;
    uint8_t numberASDU = (uint8_t)par("noASDU").doubleValue();
    noAsdu.noASDUSize = 1;//sizeof(noAsdu.noASDUValue);
    noAsdu.noASDUValue = numberASDU;
    smvpacket->setAsduNumber(noAsdu);

    //ID do MSVCB
    stSvID asduId;
    asduId.svID = SV_CONTEXT_SPECIFIC_OPTION0;
    const char *msvcbID = par("cbID");
    asduId.svIDSize = strlen(msvcbID)+1;
    strcpy(asduId.svIDValue,msvcbID);
    smvpacket->setSvID(asduId);
    totalDataLenghtCounter += sizeof(asduId.svID)+sizeof(asduId.svIDSize)+asduId.svIDSize;

    //Sample Count
    stSmpCnt asduSmpCnt;
    asduSmpCnt.smpCnt = SV_CONTEXT_SPECIFIC_OPTION2;
    asduSmpCnt.smpCntSize = sizeof(asduSmpCnt.smpCntValue);
    asduSmpCnt.smpCntValue = seqNum;
    smvpacket->setSmpCnt(asduSmpCnt);
    totalDataLenghtCounter += sizeof(asduSmpCnt.smpCnt)+sizeof(asduSmpCnt.smpCntSize)+asduSmpCnt.smpCntSize;

    //Configuration Revision
    stConfRev asduConfRev;
    asduConfRev.confRev = SV_CONTEXT_SPECIFIC_OPTION3;
    uint32_t confRev = (uint32_t)par("confRev").doubleValue();
    asduConfRev.confRevSize = sizeof(asduConfRev.confRevValue);
    asduConfRev.confRevValue = confRev;
    smvpacket->setConfRev(asduConfRev);
    totalDataLenghtCounter += sizeof(asduConfRev.confRev)+sizeof(asduConfRev.confRevSize)+asduConfRev.confRevSize;

    //Sample Synchronization
    stSmpSynch asduSmpSyn;
    asduSmpSyn.smpSynch = SV_CONTEXT_SPECIFIC_OPTION5;
    asduSmpSyn.smpSynchSize = sizeof(asduSmpSyn.smpSynchValue);
    uint8_t synchValue = (uint8_t)par("synchValue").doubleValue();
    asduSmpSyn.smpSynchValue = synchValue;
    smvpacket->setSmpSynch(asduSmpSyn);
    totalDataLenghtCounter += sizeof(asduSmpSyn.smpSynch)+sizeof(asduSmpSyn.smpSynchSize)+asduSmpSyn.smpSynchSize;

    //Data Set Values.
    stAData dataSetValues;

    //These values can be obtained as volatile parameters in order to simulate different situations.
    dataSetValues.quality.firstQByte.empty1     = 0x00;
    dataSetValues.quality.firstQByte.empty1a    = 0x00;
    dataSetValues.quality.firstQByte.empty2     = 0x00;
    dataSetValues.quality.firstQByte.der        = 0x00;
    dataSetValues.quality.firstQByte.opB        = 0x00;
    dataSetValues.quality.firstQByte.test       = 0x00;
    dataSetValues.quality.firstQByte.source     = 0x00;
    dataSetValues.quality.firstQByte.DetailQua  = 0x00;

    dataSetValues.quality.secondQByte.DetailQua = 0x00;
    dataSetValues.quality.secondQByte.validity  = 0x00;

    //The 16 members of the PhsMeas1 DataSet are filled with the same data.
    int i;
    for(i = 0; i<8; i++)
    {   //Verify the volatile option for this parameter in order to generate a sin wave or create a function getSinValue();
        if(i<4)
            dataSetValues.aValue = currentSampleValue[i];
        else
            dataSetValues.aValue = voltageSampleValue[i-4];
        smvpacket->setPhsMeasOne(i,dataSetValues);
        totalMessLenghtCounter += sizeof(dataSetValues);
    }

    //Start of DataSet Sequence
    stDataSetStart asduDataSetStart;
    asduDataSetStart.dataSet = SV_CONTEXT_SPECIFIC_OPTION7;
    asduDataSetStart.dataSetSize = (char)totalMessLenghtCounter;
    smvpacket->setDataSetStart(asduDataSetStart);
    totalDataLenghtCounter += sizeof(asduDataSetStart.dataSet) + sizeof(asduDataSetStart.dataSetSize) + totalMessLenghtCounter;

    //Start of the First ASDU of the sequence. The frame just have structure for one ASDU
    stIndASDUseqStart firstAsdu;
    firstAsdu.seqASDU = SV_ASDU_SEQ_LENGTH_TAG;
    firstAsdu.seqASDUSize = totalDataLenghtCounter;
    smvpacket->setFirstAsdu(firstAsdu);
    totalLenghtCounter += sizeof(firstAsdu.seqASDU)+sizeof(firstAsdu.seqASDUSize)+firstAsdu.seqASDUSize;

    //ASDU Sequence Start and Size
    stASDUseqStart asduSeqStart;
    asduSeqStart.seqSASDU = SV_CONS_LENGTH_TAG;
    asduSeqStart.seqSASDUSize = SV_LENGTH_TAG;
    asduSeqStart.seqSASDUValue = (uint16_t)totalDataLenghtCounter+2;
    smvpacket->setAsduSeqStart(asduSeqStart);
    totalLenghtCounter += sizeof(asduSeqStart.seqSASDU)+sizeof(asduSeqStart.seqSASDUSize)+sizeof(asduSeqStart.seqSASDUValue);

    totalLenghtCounter += sizeof(noAsdu.noASDU)+ sizeof(noAsdu.noASDUSize)+noAsdu.noASDUSize;
    //APDU size
    stAPDUStart apduStart;
    apduStart.savPdu        = SV_START_OF_APDU_TAG;
    apduStart.savPduSize    = SV_LENGTH_TAG;
    apduStart.savPduValue = totalLenghtCounter;
    smvpacket->setApduStart(apduStart);
    totalLenghtCounter += sizeof(apduStart.savPdu)+sizeof(apduStart.savPduSize)+sizeof(apduStart.savPduValue);

    smvpacket->setTotalLenght(totalLenghtCounter+8);

    //Process
    /*printf("%2X ", smvpacket->getHdrAppID());
    printf("%2X ",smvpacket->getTotalLenght());
    printf("%2X ", smvpacket->getReserved1());
    printf("%2X ", smvpacket->getReserved2());
    printf("%2X %2X %2X ",smvpacket->getApduStart().savPdu,smvpacket->getApduStart().savPduSize,smvpacket->getApduStart().savPduValue);
    printf("%2X %2X %2X ", smvpacket->getAsduNumber().noASDU, smvpacket->getAsduNumber().noASDUSize,smvpacket->getAsduNumber().noASDUValue);
    printf("%2X %2X %2X ",smvpacket->getAsduSeqStart().seqSASDU,smvpacket->getAsduSeqStart().seqSASDUSize,smvpacket->getAsduSeqStart().seqSASDUValue);
    printf("%2X %2X ",smvpacket->getFirstAsdu().seqASDU,smvpacket->getFirstAsdu().seqASDUSize);
    printf("%2X %2X %s ",smvpacket->getSvID().svID, smvpacket->getSvID().svIDSize, smvpacket->getSvID().svIDValue);
    printf("%2X %2X %2X ",smvpacket->getSmpCnt().smpCnt, smvpacket->getSmpCnt().smpCntSize, smvpacket->getSmpCnt().smpCntValue);
    printf("%2X %2X %4X ",smvpacket->getConfRev().confRev, smvpacket->getConfRev().confRevSize, smvpacket->getConfRev().confRevValue);
    printf("%2X %2X %2X ",smvpacket->getSmpSynch().smpSynch, smvpacket->getSmpSynch().smpSynchSize, smvpacket->getSmpSynch().smpSynchValue);
    printf("%2X %2X \n",smvpacket->getDataSetStart().dataSet, smvpacket->getDataSetStart().dataSetSize);
    for(i = 0; i<8; i++)
    {   if(i == 4)
            printf("\n");
        printf("%d%2X %2X ", smvpacket->getPhsMeasOne(i).aValue, smvpacket->getPhsMeasOne(i).quality.firstQByte, smvpacket->getPhsMeasOne(i).quality.secondQByte);
        if(i == 7)
             printf("\n");
    }//*/
}

void EtherAppSv::sendPacket()
{   char msgname[30];
    sprintf(msgname, "req-%d-%ld", getId(), seqNum);
    EV << "Generating SV packet `" << msgname << "'\n";

    EtherSvData *smvpacket  = new EtherSvData(msgname, IEEE802CTRL_DATA);

    createSvPacket(smvpacket);

    smvpacket->setByteLength(smvpacket->getTotalLenght());

    Ieee802Ctrl *etherctrl = new Ieee802Ctrl();
    etherctrl->setDest(destMACAddress);
    etherctrl->setEtherType(ETHERTYPE_802_1_Q);
    //etherctrl->setPpc(SV_1Q_PPC);
    etherctrl->setPpc(ppc);
    etherctrl->setDe(SV_1Q_DE);
    //etherctrl->setVid(SV_1Q_VID);
    etherctrl->setVid(vid);
    etherctrl->setApptype(ETHERTYPE_SV);
    smvpacket->setControlInfo(etherctrl);

    emit(sentPkSignal, smvpacket);
    send(smvpacket, "out");
    packetsSent++;

    if(seqNum == samplingRate)
        seqNum = 0;
    else
        seqNum++;

}

void EtherAppSv::receivePacket(cPacket *msg)
{
    EV << "Received packet `" << msg->getName() << "'\n";

    //If the module is a Generator, processing this data here doesn't make sense.
    //A subscriber will use these date to determine an abnormal situation.
    if(!isGenerator())
    {   Ieee802Ctrl *etherctrl = check_and_cast<Ieee802Ctrl*>(msg->removeControlInfo());
        EtherSvData *frame = check_and_cast<EtherSvData*>(msg);
        packetsReceived++;
        simtime_t eed = simTime() - msg->getCreationTime();
        eteAvg += eed.inUnit(-6);
        if (frame->getSmpCnt().smpCntValue == samplingRate)
        {   eteAvg /= samplingRate;
            emit(eteDelaySignal,eteAvg);
            eteAvg = 0;
        }
        emit(rcvdPkSignal, msg);

        if (etherctrl->getApptype() == ETHERTYPE_SV)
        {   //It goes in a for clause from 0 to less than 8
            int i;
            stAData dataSetValues;
            for(i = 0; i<8; i++)
            {   dataSetValues = frame->getPhsMeasOne(i);

                //Process
                //if (i<4)
                //    printf("dataSetValues: Current %d = %d, Quality = %x\n", i , dataSetValues.aValue, dataSetValues.quality.secondQByte.DetailQua);
                //else
                //    printf("dataSetValues: Voltage %d = %d, Quality = %x\n", i , dataSetValues.aValue, dataSetValues.quality.secondQByte.DetailQua);
                //Verify SV fields to Set alarm state
                if((i<4 && dataSetValues.aValue >= DANGER_PHASE_CURRENT_VALUE) ||
                   (i>=4 && dataSetValues.aValue >= DANGER_PHASE_VOLTAGE_VALUE))
                {  break;
                }
            }

            alarmState = gooseModule->getAlarmState();
            //*******test**********
            if((packetsReceived == 5) || (packetsReceived%50000 == 0))
            {   alarmState = true;
            }
            //**********************

            if ((i<8 && !alarmState)||(i>=8 && alarmState))
            {   //inform about abnormal situation
                gooseModule->setAlarmState();
            }

            //Process

            /*stAPDUStart apduStart = frame->getApduStart();
            stNoASDU noAsdu = frame->getAsduNumber();
            stASDUseqStart asduSeqStart = frame->getAsduSeqStart();
            stIndASDUseqStart firstAsdu = frame->getFirstAsdu();
            stSvID asduId = frame->getSvID();
            stSmpCnt asduSmpCnt = frame->getSmpCnt();
            stConfRev asduConfRev = frame->getConfRev();
            stSmpSynch asduSmpSyn = frame->getSmpSynch();
            stDataSetStart asduDataSetStart = frame -> getDataSetStart();*/

            //printf("The APPID of the Header must be 0x4000 or %d\n",frame->getHdrAppID());
            //printf("The APDU+HEADER size is %d\n",frame->getTotalLenght());
            //printf("The Reserved1 and Reserved2 bytes are %d and %d\n",frame->getReserved1(), frame->getReserved2());
            //printf("The first part of the APDU is: TAG = %x, SizeTag = %x, APDU size = %x\n",apduStart.savPdu, apduStart.savPduSize, apduStart.savPduValue);
            //printf("The number of ASDU in APDU: TAG = %x, Size = %x, number of ASDU = %x\n",noAsdu.noASDU, noAsdu.noASDUSize, noAsdu.noASDUValue);
            //printf("Start of ASDU is: TAG = %d, Size = %x, SEQUENCE size = %x\n",asduSeqStart.seqSASDU, asduSeqStart.seqSASDUSize, asduSeqStart.seqSASDUValue);
            //printf("Start of First ASDU is: TAG = %x, FIRST ASDU size = %x\n",firstAsdu.seqASDU, firstAsdu.seqASDUSize);
            //printf("MSVCB ID is: TAG = %x, SvID size = %x, SvID value = %s\n",asduId.svID, asduId.svIDSize, asduId.svIDValue);
            //printf("Number of unsynchronized samples is Tag = %x, Size = %x, Value = %x\n", asduSmpCnt.smpCnt, asduSmpCnt.smpCntSize, asduSmpCnt.smpCntValue);
            //printf("asduConfRev Tag = %x, Size = %x, Value = %x\n", asduConfRev.confRev, asduConfRev.confRevSize, asduConfRev.confRevValue);
            //printf("asduSmpSyn Tag = %x, Size = %x, Value = %x\n", asduSmpSyn.smpSynch, asduSmpSyn.smpSynchSize, asduSmpSyn.smpSynchValue);
            //printf("asduDataSetStart Tag = %x, Size = %x\n", asduDataSetStart.dataSet, asduDataSetStart.dataSetSize);
            //printf("dataSetValues: Current = %d\n Quality = %d\n",dataSetValues.aValue,dataSetValues.quality.secondQByte.DetailQua);
        }//*/
    }
    delete msg;
}

void EtherAppSv::finish()
{
    cancelAndDelete(timerMsg);
    timerMsg = NULL;
}




