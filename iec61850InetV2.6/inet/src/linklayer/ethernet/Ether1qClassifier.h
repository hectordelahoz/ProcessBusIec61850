/*
 * EtherIECClassifier.h
 *
 *  Created on: 28/05/2015
 *      Author: Hector
 */

#ifndef ETHERIECCLASSIFIER_H_
#define ETHERIECCLASSIFIER_H_

#include "INETDefs.h"

/**
 * Ethernet Frame classifier.
 *
 * Ethernet frames are classified as:
 * - 802.1q frames
 * - others
 */
class INET_API Ether1qClassifier : public cSimpleModule
{
  public:
    /**
     * Sends the incoming packet to either pauseOut or defaultOut gate.
     */
    virtual void handleMessage(cMessage *msg);
};



#endif /* ETHERIECCLASSIFIER_H_ */
