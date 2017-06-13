/*
*   This class correspond to the TCZ, UDZ and ICMZ
*
*
*/

#ifndef INC_RESPONDER
#define INC_RESPONDER

// Total number of TestConf per responder
#define MAX_TEST_CONF 10

// Shared ptr for state machine class attribute
#include <boost/shared_ptr.hpp>

// LibTin
#include <tins/tins.h>
#include "wqueue.h"
#include "testconf.h"

using namespace Tins;

class Responder {

public:
        // --> Configuration conf[];
        // --> ContentEngine engines[];
        // --> typedef msm::back::state_machine<T> stateMachine;
        // my state machine, hidden
        uint16_t localPort;
        uint16_t remotePort;
        // This should be one side of the incoming Queue.
        // The other side is filled in by the Listener
        wqueue<PDU*> *incomingQueue = new wqueue<PDU*>();
        
        Responder();
        ~Responder();

        // Each child need to implement this,
        // used to have the responder attached to the incoming packet queue
        virtual void startListen() = 0;
        
        // Each child need to implement this,
        // used by the listener to correctly forward packets
        virtual int getId() = 0;

        bool send();
        virtual void processPacket(PDU& pdu) = 0;

        // Each child must implement this, 
        // used by the Listener to get the packet queue
        virtual wqueue<Tins::PDU*>* getQueue(); 
};

class TCPResponder: public Responder {
public: 
        boost::shared_ptr<void> stateMachine;
        int streamId = -1;
        std::vector<TimeConf> timeTests;
        std::vector<TczConf> tczTests;

        TCPResponder();
        ~TCPResponder();
        void startListen();
        bool send(IP pdu, string funcName = "");
        void processPacket(PDU* pdu);

        int getId();

        void addTimeConf(TimeConf t);
        void addTczConf(TczConf t);

};

class UDPResponder: public Responder {};

class ICMPResponder: public Responder {};

#endif  /* INC_RESPONDER */



