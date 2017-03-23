/*
*   This class correspond to the TCZ, UDZ and ICMZ
*
*
*/

#ifndef INC_RESPONDER
#define INC_RESPONDER

// Shared ptr for state machine class attribute
#include <boost/shared_ptr.hpp>

// LibTin
#include <tins/tins.h>
#include "wqueue.h"

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
        wqueue<PDU*> *incomingQueue;
        
        Responder();
        ~Responder();

        // Each child need to implement this,
        // used to have the responder attached to the incoming packet queue
        virtual void startListen() = 0;

        bool send();
        void processPacket(PDU& pdu);       
};

class TCPResponder: public Responder {
public: 
        boost::shared_ptr<void> stateMachine;
        int streamId = -1;

        TCPResponder(wqueue<PDU*> *q);
        ~TCPResponder();
        void startListen();
        bool send(IP pdu);
        void processPacket(PDU* pdu);
};

class UDPResponder: public Responder {};

class ICMPResponder: public Responder {};

#endif  /* INC_RESPONDER */



