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

    // This might be a list of Boost::vector instead
    list<Responder*> responders;

// Methods
    Listener();
    ~Listener();
    virtual bool pktCallback(PDU &pdu);
    bool configure(std::string jsonConfig);
    void start();
    bool forwardPacket(PDU* pkt);

};

class IPListener: public Listener {
public:
    IPListener();
    ~IPListener();

    // bool pktCallback(PDU &pdu);
    // void start();
};

#endif  /* INC_LISTENER */
