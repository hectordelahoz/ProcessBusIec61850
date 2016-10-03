/*
 * PrioritySchedulerEx.h
 *
 *  Created on: 28/05/2015
 *      Author: Hector
 */

#ifndef PRIORITYSCHEDULEREX_H_
#define PRIORITYSCHEDULEREX_H_

#include "INETDefs.h"
#include "SchedulerBase.h"
#include "Ieee802Ctrl_m.h"
#include "IPassiveQueue.h"

/**
 * Schedule packets in strict priority order.
 *
 * Packets arrived at the 0th gate has the higher priority.
 */
class INET_API PrioritySchedulerEx : public SchedulerBase
{
  protected:
    virtual bool schedulePacket();
    virtual void sendOut(cMessage *msg);
};



#endif /* PRIORITYSCHEDULEREX_H_ */
