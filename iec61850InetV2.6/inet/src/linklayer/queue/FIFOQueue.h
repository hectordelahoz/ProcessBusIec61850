//
// Copyright (C) 2012 Opensim Ltd.
// Author: Tamas Borbely
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//


#ifndef __INET_FIFOQUEUE_H
#define __INET_FIFOQUEUE_H

#include "INETDefs.h"
#include "PassiveQueueBase.h"
#include "IQueueAccess.h"

/**
 * Passive FIFO Queue with unlimited buffer space.
 */
class INET_API FIFOQueue : public PassiveQueueBase, public IQueueAccess
{
  protected:
    // state
    cQueue queue;
    cGate *outGate;
    int byteLength;

    // statistics
    static simsignal_t queueLengthSignal;

  public:
    FIFOQueue() : outGate(NULL), byteLength(0) {}

  protected:
    virtual void initialize();

    virtual cMessage *enqueue(cMessage *msg);

    virtual cMessage *dequeue();

    virtual void sendOut(cMessage *msg);

    virtual bool isEmpty();

    virtual int getLength() const { return queue.getLength(); }

    virtual int getByteLength() const { return byteLength; }
};

#endif
