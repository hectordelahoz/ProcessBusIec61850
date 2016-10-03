/*
 * EtherIECClassifier.cc
 *
 *  Created on: 28/05/2015
 *      Author: Hector
 */

#include "Ether1qClassifier.h"

#include "EtherFrame.h"


Define_Module(Ether1qClassifier);

void Ether1qClassifier::handleMessage(cMessage *msg)
{   if (dynamic_cast<Ethernet1QTag *>(msg) != NULL)
    {   unsigned int priority = 0;
        //Extracting the priority of the msg from the IEEE802.1q header
        stPriTag priTag = ((Ethernet1QTag *)msg)->getTci();
        priority = (unsigned int)priTag.ppc;
        //Sending to the priority queue.
        if(priority > 0 && priority <= 7)
        {   priority = 7-priority;
            send(msg, "Out", priority);
        }
        else
            send(msg, "defaultOut");
    }
    else
    {   //Sending to the default queue
        send(msg, "defaultOut");
    }
}




