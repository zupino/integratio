/*
*   This class correspond to the Connector
*
*
*
*/
#ifndef INC_LISTENER
#define INC_LISTENER

// LibTin
#include <tins/tins.h>
// List
#include <list>
// Working Queue
#include "wqueue.h"
#include "responder.h"

using namespace Tins;

class Listener {

public:
    Sniffer* sniffer;
    SnifferConfiguration snifferConfig;
    IPv4Address localAddress;
    IPv4Address remoteAddress;

    // Incoming packet Queue. Listener send incoming packet
    // in this queue, on the other side the Responder is
    // waiting on incoming messages
    
    wqueue<PDU*> *pktQueue = new wqueue<PDU*>();

    // This might be a list of Boost::vector instead
    list<Responder*> responders;

// Methods
    Listener();
    ~Listener();
    bool pktCallback(PDU &pdu);
    bool configure(std::string jsonConfig);
    void start();

};

class IPListener: public Listener {
public:
    IPListener();
    ~IPListener();
};

#endif  /* INC_LISTENER */