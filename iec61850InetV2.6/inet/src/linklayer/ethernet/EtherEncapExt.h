/*
 * EtherEncapExt.h
 *
 *  Created on: 14/04/2015
 *      Author: Hector
 */

#ifndef ETHERENCAPEXT_H_
#define ETHERENCAPEXT_H_


#include "INETDefs.h"

#include "Ethernet.h"

// Forward declarations:
class EtherFrame;


/**
 * Performs Goose and SV encapsulation/decapsulation into Ether802.1q. More info in the NED file.
 */
class INET_API EtherEncapExt : public cSimpleModule
{
  protected:
    int seqNum;

    // statistics
    long totalFromHigherLayer;  // total number of packets received from higher layer
    long totalFromMAC;          // total number of frames received from MAC
    long totalPauseSent;        // total number of PAUSE frames sent
    static simsignal_t encapPkSignal;
    static simsignal_t decapPkSignal;
    static simsignal_t pauseSentSignal;
    bool useSNAP;               // true: generate EtherFrameWithSNAP, false: generate EthernetIIFrame

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    virtual void processPacketFromHigherLayer(cPacket *msg);
    virtual void processFrameFromMAC(EtherFrame *msg);
    virtual void handleSendPause(cMessage *msg);

    virtual void updateDisplayString();
};


#endif /* ETHERENCAPEXT_H_ */
