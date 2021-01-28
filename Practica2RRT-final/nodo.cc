//Iñigo Pérez Bada - Máster Teleco - RRT
//
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "MyNetMsg_m.h"

using namespace omnetpp;

class Nodo: public cSimpleModule {
private:
    cChannel *channel[2]; // one channel for each output
    cQueue *queue[2];  // one queue for each channel
    double probability;  // from 0 to 1
    simtime_t timeout;  // timeout
    cMessage *timeoutEvent;  // holds pointer to the timeout self-message
    MyNetMsg *lastpkt;
    int lastGate;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void sendNewPkt(MyNetMsg *pkt);
    virtual void sendNextPkt(int gateIndex);
    virtual void sendPkt(MyNetMsg *pkt, int gateIndex);
};

Define_Module(Nodo);

void Nodo::initialize() {
    WATCH(lastGate);
    channel[0] = gate("outPort", 0)->getTransmissionChannel();
    channel[1] = gate("outPort", 1)->getTransmissionChannel();

    queue[0] = new cQueue("queueZero");
    queue[1] = new cQueue("queueOne");

    srand(time(NULL));

    probability = (double) par("probability");
}

void Nodo::handleMessage(cMessage *msg) {
    timeout = 3.0;
    timeoutEvent = new cMessage("timeoutEvent");

    if (strcmp(msg->getName(), "timeoutEvent") == 0) {
        // If we receive the timeout event, that means the packet hasn't
        // arrived in time and we have to re-send it.
        EV << "Timeout expired, resending message and restarting timer\n";
        //cGate *lastarrivalGate = lastpkt->getArrivalGate();
        //int lastarrivalGateIndex = lastarrivalGate->getIndex();
        if (uniform(0, 1) < 0.33) {
            sendNextPkt(0);
        }else{
            sendNextPkt(1);
        }
        scheduleAt(simTime() + timeout, timeoutEvent);
    } else {
        EV << "Timer cancelled.\n";
        cancelEvent(timeoutEvent);
        MyNetMsg *pkt = check_and_cast<MyNetMsg*>(msg);
        //if(lastpkt!=nullptr){
          //  lastpkt->_detachEncapMsg();
        //}
        lastpkt = check_and_cast<MyNetMsg*>(pkt->dup());;
        cGate *arrivalGate = pkt->getArrivalGate();
        int arrivalGateIndex = arrivalGate->getIndex();
        EV
                  << "Packet arrived from gate "
                          + std::to_string(arrivalGateIndex) + "\n";

        if (pkt->getInicial()) {
            // Packet from source
            EV << "Forward packet from source\n";
            pkt->setInicial(false);
            sendNewPkt(pkt);
            return;
        }
        if (pkt->getKind() == 1) { // 1: If we receive Packet
            if (pkt->hasBitError()) {
                EV << "Packet arrived with error, send NAK\n";
                MyNetMsg *nak = new MyNetMsg("NAK");
                nak->setKind(3);
                send(nak, "outPort", arrivalGateIndex);
            } else {
                EV << "Packet arrived without error, send ACK\n";
                MyNetMsg *ack = new MyNetMsg("ACK");
                ack->setKind(2);
                send(ack, "outPort", arrivalGateIndex);
                for (int i = 0; i < 4; i++) {
                    EV
                              << "La posicion " + std::to_string(i)
                                      + " en la ruta es: "
                                      + std::to_string(pkt->getRuta(i));
                    if (pkt->getRuta(i) == 0) {
                        pkt->setRuta(i, getId());
                        pkt->setRutaTimes(i, simTime());
                        EV
                                  << "Guardando en la posicion "
                                          + std::to_string(i) + " de la ruta : "
                                          + std::to_string(pkt->getId());
                        break;
                    }
                }
                sendNewPkt(pkt);
            }
        } else if (pkt->getKind() == 2) { // 2: If we receive ACK
            EV << "ACK from next node\n";
            if (queue[arrivalGateIndex]->isEmpty())
                EV
                          << "WARNING: there are not packets in queue, but ACK arrived\n";
            else {
                // pop() removes queue's first packet
                queue[arrivalGateIndex]->pop();
                sendNextPkt(arrivalGateIndex);
                scheduleAt(simTime() + timeout, timeoutEvent);
            }
        } else { // 3: If we receive NAK
            EV << "NAK from next node\n";
            sendNextPkt(arrivalGateIndex);
            scheduleAt(simTime() + timeout, timeoutEvent);
        }
    }
}

void Nodo::sendNewPkt(MyNetMsg *pkt) {
    int gateIndex;
    double randomNumber = ((double) rand() / (RAND_MAX));
    if (randomNumber < probability)
        gateIndex = 0;
    else
        gateIndex = 1;
    if (queue[gateIndex]->isEmpty()) {
        EV << "Queue is empty, send packet and wait\n";
        // Insert in queue (it may have to be sent again)
        queue[gateIndex]->insert(pkt);
        sendPkt(pkt, gateIndex);
    } else {
        EV << "Queue is not empty, add to back and wait\n";
        queue[gateIndex]->insert(pkt);
    }
}

void Nodo::sendNextPkt(int gateIndex) {
    if (queue[gateIndex]->isEmpty())
        EV << "No more packets in queue\n";
    else {
        // front() gets the packet without removing it from queue
        MyNetMsg *pkt = check_and_cast<MyNetMsg*>(queue[gateIndex]->front());
        sendPkt(pkt, gateIndex);
    }
}

void Nodo::sendPkt(MyNetMsg *pkt, int gateIndex) {
    if (channel[gateIndex]->isBusy()) {
        EV
                  << "WARNING: channel is busy, check that everything is working fine\n";
    } else {
        // OMNeT++ can't send a packet while it is queued, must send a copy
        MyNetMsg *newPkt = check_and_cast<MyNetMsg*>(pkt->dup());
        send(newPkt, "outPort", gateIndex);
    }
}
