/*
 * PrioritySchedulerEx.cc
 *
 *  Created on: 28/05/2015
 *      Author: Hector
 */

#include "PrioritySchedulerEx.h"

Define_Module(PrioritySchedulerEx);

bool PrioritySchedulerEx::schedulePacket()
{
    for (std::vector<IPassiveQueue*>::iterator it = inputQueues.begin(); it != inputQueues.end(); ++it)
    {
        IPassiveQueue *inputQueue = *it;
        if (!inputQueue->isEmpty())
        {
            inputQueue->requestPacket();
            return true;
        }
    }
    return false;
}


void PrioritySchedulerEx::sendOut(cMessage *msg)
{   send(msg, "out");
}
