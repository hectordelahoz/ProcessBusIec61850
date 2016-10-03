/*
 * EtherAppSvSubs.h
 *
 *  Created on: 20/04/2015
 *      Author: Hector
 */

#ifndef ETHERAPPSVSUBS_H_
#define ETHERAPPSVSUBS_H_

#include "INETDefs.h"

#include "MACAddress.h"
#include "NodeStatus.h"
#include "ILifecycle.h"
#include "EtherAppGoose.h"
#include "EtherSv_m.h"

/**
 * Simple traffic generator for the SV (IEC61850-9-2) model.
 */
class INET_API EtherAppSv : public cSimpleModule, public ILifecycle
{
  protected:
    enum Kinds {START=100, NEXT, SAMPLE};

    // send parameters
    long seqNum;
    cPar *sendInterval;
    double sampleInterval;
    double samplingRate;
    int ppc;
    int vid;
    bool alarmState;
    double currentSampleValue[4];
    double voltageSampleValue[4];

    MACAddress destMACAddress;
    NodeStatus *nodeStatus;
    EtherAppGoose * gooseModule;

    // self messages
    cMessage *timerMsg;
    cMessage *timerSmp;
    simtime_t startTime;
    simtime_t stopTime;
    long eteAvg;

    // receive statistics
    long packetsSent;
    long packetsReceived;
    static simsignal_t sentPkSignal;
    static simsignal_t rcvdPkSignal;
    static simsignal_t eteDelaySignal;

  public:
    EtherAppSv();
    virtual ~EtherAppSv();

    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback);

  protected:
    virtual void initialize(int stage);
    virtual int numInitStages() const { return 4; }
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    virtual bool isNodeUp();
    virtual bool isGenerator();
    virtual void scheduleNextPacket(bool start);
    virtual void scheduleNextSample(bool start);
    virtual void cancelNextPacket();
    virtual void cancelNextSample();

    virtual MACAddress resolveDestMACAddress();

    virtual void createSvPacket(EtherSvData *smvpacket);
    virtual void sendPacket();
    virtual void receivePacket(cPacket *msg);
    virtual void getSinValue();
};

#endif /* ETHERAPPSVSUBS_H_ */
