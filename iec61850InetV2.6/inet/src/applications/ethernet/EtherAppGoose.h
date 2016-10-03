/*
 * EtherAppGoose.h
 *
 *  Created on: 17/04/2015
 *      Author: Hector
 */

#ifndef ETHERAPPGOOSE_H_
#define ETHERAPPGOOSE_H_
#include "INETDefs.h"

#include "MACAddress.h"
#include "NodeStatus.h"
#include "ILifecycle.h"
#include "EtherGoose_m.h"


/**
 * Simple traffic generator for the GOOSE (IEC61850-8-1) model.
 */
class INET_API EtherAppGoose : public cSimpleModule, public ILifecycle
{
  protected:
    enum Kinds {START=100, NEXT};

    // send parameters
    long seqNum;
    uint32_t stChng;
    double myAppID;

    double pcAppID[3];
    double interAppID[2];
    double comAppID[3];
    //double pcAppID;
    //double interAppID;
    //double comAppID;

    bool alarmState;
    double  sendInterval;
    double  sInterval;
    double ppc;
    double vid;
    //Ethernet Param.
    MACAddress destMACAddress;
    NodeStatus *nodeStatus;

    //IecParam
    char nodeType[6];

    // self messages
    cMessage *timerMsg;
    simtime_t startTime;
    simtime_t stopTime;

    // receive statistics
    long packetsSent;
    long packetsReceived;
    static simsignal_t sentPkSignal;
    static simsignal_t rcvdPkSignal;

  public:
    EtherAppGoose();
    virtual ~EtherAppGoose();
    virtual void setAlarmState();
    virtual bool getAlarmState();
    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback);

  protected:
    virtual void initialize(int stage);
    virtual int numInitStages() const { return 4; }
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    virtual bool isNodeUp();
    virtual bool isGenerator();
    virtual void scheduleNextPacket(bool start);
    virtual void cancelNextPacket();

    virtual MACAddress resolveDestMACAddress();

    virtual void createGoosePacket(EtherGooseData *goosePacket);
    virtual void sendPacket();
    virtual void receivePacket(cPacket *msg);
};

#endif /* ETHERAPPGOOSE_H_ */
