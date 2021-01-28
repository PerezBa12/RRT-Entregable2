//Iñigo Pérez Bada - Máster Teleco - RRT
///
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <random>
#include "MyNetMsg_m.h"
#include <chrono>
using namespace omnetpp;

class Fuente: public cSimpleModule {
private:
    double lambda;
    double tampaquete;
    int muestras;
    int numSeq;
    long numSent;
    long flujo;
    cLongHistogram packetSizeStats;
    cOutVector packetSizeVector;
    cLongHistogram lambdaStats;
    cOutVector lambdaVector;
protected:
    virtual void initialize() override;
    virtual double setDepartures(double lambda);
    virtual double setLengths(double mu);
    virtual MyNetMsg* getPacket();
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

Define_Module(Fuente);

void Fuente::initialize() {
    numSent = 0;
    flujo = 0;
    WATCH(numSent);
    WATCH(flujo);

    packetSizeStats.setName("packetSizeStats");
    packetSizeVector.setName("packetSize");
    lambdaStats.setName("lambdaStats");
    lambdaVector.setName("lambdaVector");

    lambda = (double) par("lambda");
    tampaquete = (double) par("tampaquete");
    MyNetMsg *pkt = getPacket();
    pkt->setBitLength(setLengths(tampaquete));
    scheduleAt(simTime() + setDepartures(lambda), pkt);
}

double Fuente::setDepartures(double lambda) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::uniform_real_distribution<double> uniformRandom(0.0, 1.0);
    double randomNumber = uniformRandom(generator);
    double number = (-1 / lambda) * log(randomNumber);
    return number;
}

double Fuente::setLengths(double tampaquete) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::uniform_real_distribution<double> uniformRandom(0.0, 1.0);
    double randomNumber = uniformRandom(generator);
    double number = (-tampaquete) * log(randomNumber);
    return number;
}

MyNetMsg* Fuente::getPacket() {
    std::string packetName = "packet::" + std::to_string(getId()) + "::"
            + std::to_string(numSeq);
    char *charPacketName = new char[packetName.length() + 1];
    strcpy(charPacketName, packetName.c_str());
    MyNetMsg *pkt = new MyNetMsg(charPacketName);
    pkt->setInicial(true);
    pkt->setKind(1);
    pkt->setNumSeq(numSeq);
    pkt->setOrigen(getId());
    pkt->setRuta(0, getId());
    pkt->setRutaTimes(0, simTime());
    numSeq++;
    return pkt;
}

void Fuente::handleMessage(cMessage *msg) {
    MyNetMsg *pkt = check_and_cast<MyNetMsg*>(msg);
    send(pkt, "outPort");
    double lambdapkt = setDepartures(lambda);
    double tampaquetepkt= setLengths(tampaquete);
    EV << "lambda: " + std::to_string(lambdapkt)+" Tam del paquete: " + std::to_string(tampaquetepkt);
    MyNetMsg *myPkt = getPacket();
    myPkt->setBitLength(tampaquetepkt);
    scheduleAt(simTime() + lambdapkt, myPkt);
    numSent++;
    flujo = numSent / simTime().dbl();
    packetSizeVector.record(tampaquetepkt);
    packetSizeStats.collect(tampaquetepkt);
    lambdaVector.record(simTime().dbl());
    lambdaStats.collect(simTime().dbl());
}
void Fuente::finish() {
    // This function is called by OMNeT++ at the end of the simulation.
    EV << "Sent:     " << numSent << endl;
    EV << "Received: " << flujo << endl;
    EV << "Hop count, min:    " << packetSizeStats.getMin() << endl;
    EV << "Hop count, max:    " << packetSizeStats.getMax() << endl;
    EV << "Hop count, mean:   " << packetSizeStats.getMean() << endl;
    EV << "Hop count, stddev: " << packetSizeStats.getStddev() << endl;

    EV << "Hop count, min:    " << lambdaStats.getMin() << endl;
    EV << "Hop count, max:    " << lambdaStats.getMax() << endl;
    EV << "Hop count, mean:   " << lambdaStats.getMean() << endl;
    EV << "Hop count, stddev: " << lambdaStats.getStddev() << endl;
    recordScalar("#sent", numSent);
    recordScalar("#received", flujo);

    packetSizeStats.recordAs("packet size");
}
