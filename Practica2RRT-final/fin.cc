//Iñigo Pérez Bada - Máster Teleco - RRT
//
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "MyNetMsg_m.h"

using namespace omnetpp;

class Fin: public cSimpleModule {
protected:
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Fin);

void Fin::handleMessage(cMessage *msg) {
    MyNetMsg *pkt = (MyNetMsg*) msg;
    cGate *arrivalGate = pkt->getArrivalGate();
    int arrivalGateIndex = arrivalGate->getIndex();
    if (uniform(0, 1) < 0.05) {
        EV << "\"Losing\" message.\n";
        bubble("message lost");  // making animation more informative...
        delete msg;
    } else {
        EV
                  << "Packet arrived from gate "
                          + std::to_string(arrivalGateIndex) + "\n";

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
                EV << "Packet it's okay!";
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
                                          + std::to_string(getId());
                        break;
                    }
                }
            }
        }
    }
}
