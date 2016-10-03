/*
 * EtherEncapExt.cc
 *
 *  Created on: 14/04/2015
 *      Author: Hector
 */

#include <stdio.h>

#include "EtherEncapExt.h"

#include "EtherFrame.h"
#include "IInterfaceTable.h"
#include "Ieee802Ctrl_m.h"


Define_Module(EtherEncapExt);

simsignal_t EtherEncapExt::encapPkSignal = registerSignal("encapPk");
simsignal_t EtherEncapExt::decapPkSignal = registerSignal("decapPk");
simsignal_t EtherEncapExt::pauseSentSignal = registerSignal("pauseSent");

void EtherEncapExt::initialize()
{
    seqNum = 0;
    WATCH(seqNum);

    totalFromHigherLayer = totalFromMAC = totalPauseSent = 0;
    useSNAP = par("useSNAP").boolValue();

    WATCH(totalFromHigherLayer);
    WATCH(totalFromMAC);
    WATCH(totalPauseSent);
}

void EtherEncapExt::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("lowerLayerIn"))
    {
        processFrameFromMAC(check_and_cast<EtherFrame *>(msg));
    }
    else
    {
        // from higher layer
        switch (msg->getKind())
        {
            case IEEE802CTRL_DATA:
            case 0: // default message kind (0) is also accepted
              processPacketFromHigherLayer(PK(msg));
              break;

            case IEEE802CTRL_SENDPAUSE:
              // higher layer want MAC to send PAUSE frame
              handleSendPause(msg);
              break;

            default:
              throw cRuntimeError("Received message `%s' with unknown message kind %d", msg->getName(), msg->getKind());
        }
    }

    if (ev.isGUI())
        updateDisplayString();
}

void EtherEncapExt::updateDisplayString()
{
    char buf[80];
    sprintf(buf, "passed up: %ld\nsent: %ld", totalFromMAC, totalFromHigherLayer);
    getDisplayString().setTagArg("t", 0, buf);
}

void EtherEncapExt::processPacketFromHigherLayer(cPacket *msg)
{
    if (msg->getByteLength() > MAX_ETHERNET_DATA_BYTES)
        error("packet from higher layer (%d bytes) exceeds maximum Ethernet payload length (%d)", (int)msg->getByteLength(), MAX_ETHERNET_DATA_BYTES);

    totalFromHigherLayer++;
    emit(encapPkSignal, msg);

    // Creates MAC header information and encapsulates received higher layer data
    // with this information and transmits resultant frame to lower layer

    // create Ethernet frame, fill it in from Ieee802Ctrl and encapsulate msg in it
    EV << "Encapsulating higher layer packet `" << msg->getName() <<"' for MAC\n";

    Ieee802Ctrl *etherctrl = check_and_cast<Ieee802Ctrl*>(msg->removeControlInfo());
    EtherFrame *frame = NULL;
    if (etherctrl->getEtherType() == ETHERTYPE_802_1_Q)
    {   Ethernet1QTag *ethDot1qFrame = new Ethernet1QTag(msg->getName());

        ethDot1qFrame->setSrc(etherctrl->getSrc());  // if blank, will be filled in by MAC
        ethDot1qFrame->setDest(etherctrl->getDest());
        ethDot1qFrame->setEtherType(etherctrl->getEtherType());
        stPriTag priTag;
        priTag.ppc  = etherctrl->getPpc();
        priTag.de   = etherctrl->getDe();
        priTag.vid  = etherctrl->getVid();
        ethDot1qFrame->setTci(priTag);
        ethDot1qFrame->setApptype(etherctrl->getApptype());
        //used by MAC layer module
        ethDot1qFrame->setByteLength(ETHER_MAC_FRAME_BYTES + ETHER_802_1_Q_HEADER_LENGTH);
        frame = ethDot1qFrame;
        EV << "EtherType " << etherctrl->getEtherType() << "\n";
    }
    else
    {
        if (useSNAP)
        {
            EtherFrameWithSNAP *snapFrame = new EtherFrameWithSNAP(msg->getName());

            snapFrame->setSrc(etherctrl->getSrc());  // if blank, will be filled in by MAC
            snapFrame->setDest(etherctrl->getDest());
            snapFrame->setOrgCode(0);
            snapFrame->setLocalcode(etherctrl->getEtherType());
            snapFrame->setByteLength(ETHER_MAC_FRAME_BYTES + ETHER_LLC_HEADER_LENGTH + ETHER_SNAP_HEADER_LENGTH);
            frame = snapFrame;
        }
        else
        {
            EthernetIIFrame *eth2Frame = new EthernetIIFrame(msg->getName());

            eth2Frame->setSrc(etherctrl->getSrc());  // if blank, will be filled in by MAC
            eth2Frame->setDest(etherctrl->getDest());
            eth2Frame->setEtherType(etherctrl->getEtherType());
            eth2Frame->setByteLength(ETHER_MAC_FRAME_BYTES);
            frame = eth2Frame;
        }
    }
    delete etherctrl;

    frame->encapsulate(msg);
    if (frame->getByteLength() < MIN_ETHERNET_FRAME_BYTES)
        frame->setByteLength(MIN_ETHERNET_FRAME_BYTES);  // "padding"

    send(frame, "lowerLayerOut");
}

void EtherEncapExt::processFrameFromMAC(EtherFrame *frame)
{
    // decapsulate and attach control info
    cPacket *higherlayermsg = frame->decapsulate();

    // add Ieee802Ctrl to packet
    Ieee802Ctrl *etherctrl = new Ieee802Ctrl();
    etherctrl->setSrc(frame->getSrc());
    etherctrl->setDest(frame->getDest());
    if (dynamic_cast<EthernetIIFrame *>(frame) != NULL)
        etherctrl->setEtherType(((EthernetIIFrame *)frame)->getEtherType());
    else if (dynamic_cast<EtherFrameWithSNAP *>(frame) != NULL)
        etherctrl->setEtherType(((EtherFrameWithSNAP *)frame)->getLocalcode());
    else if (dynamic_cast<Ethernet1QTag *>(frame) != NULL)
        {   etherctrl->setEtherType(((Ethernet1QTag *)frame)->getEtherType());
            stPriTag priTag = ((Ethernet1QTag *)frame)->getTci();
            etherctrl->setPpc(priTag.ppc);
            etherctrl->setDe(priTag.de);
            etherctrl->setVid(priTag.vid);
            etherctrl->setApptype(((Ethernet1QTag *)frame)->getApptype());
        }
    higherlayermsg->setControlInfo(etherctrl);

    EV << "Decapsulating frame `" << frame->getName() <<"', passing up contained "
          "packet `" << higherlayermsg->getName() << "' to higher layer\n";

    totalFromMAC++;
    emit(decapPkSignal, higherlayermsg);

    // pass up to higher layers.
    if(etherctrl->getApptype() == ETHERTYPE_GOOSE)
        send(higherlayermsg, "upperLayerOut");
    else if(etherctrl->getApptype() == ETHERTYPE_SV)
        send(higherlayermsg, "upperLayerOutSv");
    delete frame;
}

void EtherEncapExt::handleSendPause(cMessage *msg)
{
    Ieee802Ctrl *etherctrl = dynamic_cast<Ieee802Ctrl*>(msg->removeControlInfo());
    if (!etherctrl)
        error("PAUSE command `%s' from higher layer received without Ieee802Ctrl", msg->getName());
    int pauseUnits = etherctrl->getPauseUnits();
    delete etherctrl;

    EV << "Creating and sending PAUSE frame, with duration=" << pauseUnits << " units\n";

    // create Ethernet frame
    char framename[40];
    sprintf(framename, "pause-%d-%d", getId(), seqNum++);
    EtherPauseFrame *frame = new EtherPauseFrame(framename);
    frame->setPauseTime(pauseUnits);
    MACAddress dest = etherctrl->getDest();
    if (dest.isUnspecified())
        dest = MACAddress::MULTICAST_PAUSE_ADDRESS;
    frame->setDest(dest);
    frame->setByteLength(ETHER_PAUSE_COMMAND_PADDED_BYTES);

    send(frame, "lowerLayerOut");
    delete msg;

    emit(pauseSentSignal, pauseUnits);
    totalPauseSent++;
}


