// Boost Logging
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

// State Machine
// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>

#include "responder.h"
#include "wqueue.h"
#include "common.h"

// Used for delay (C++11)
#include <chrono>
#include <thread>
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

namespace logging = boost::log;
namespace mpl = boost::mpl;
namespace msm = boost::msm;

/*
	START STATE MACHINE
*/

namespace {

    // events
    struct syn {
        const PDU &p;
        syn(const PDU& pkt) : p(pkt) {}
    };
    struct ack {
        const PDU &p;
        ack(const PDU& pkt) : p(pkt) {}
    };
    struct psh_ack {
        const PDU &p;
        psh_ack(const PDU& pkt) : p(pkt) {}
    };
    struct rst {
        const PDU &p;
        rst(const PDU& pkt) : p(pkt) {}
    };
    struct fin_ack {
        const PDU &p;
        fin_ack(const PDU& pkt) : p(pkt) {}
    };

    // front-end
    struct integratio_ : public msm::front::state_machine_def<integratio_>
    {
    	// Reference to outer Responder
    	TCPResponder *responder;

        // Last received packet
        IP  ip_r;
        TCP tcp_r;

        // Packet to be sent
        IP  ip_s;
        TCP tcp_s;

        int streamId = -1;

        void setResponder( TCPResponder *tresp ) {
        	responder = tresp;
        }

        // Entry and exit actions for the whole state machine 
        template <class Event, class FSM>
        void on_entry(Event const&, FSM&)
        {
            BOOST_LOG_TRIVIAL(debug) << "[INTEGRATIO] Initialize state machine.";

        }

        template <class Event, class FSM>
        void on_exit(Event const&, FSM&)
        {
            BOOST_LOG_TRIVIAL(debug) << "[INTEGRATIO] Exiting state machine.";
        }

        // FSM States
        struct LISTEN : public msm::front::state<> {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& i) {
                BOOST_LOG_TRIVIAL(debug) << "\t[LISTEN] entering.";

	            // Initialize internal packet
	            i.ip_s = IP();
	            i.tcp_s = TCP();

	            // Arbitrary set the sequence number for the packet next to be sent.
	            i.tcp_s.seq(31331);
	            BOOST_LOG_TRIVIAL(debug) << "\t\t\tstreamID: " << i.streamId;
	            i.streamId = -1;
	            BOOST_LOG_TRIVIAL(debug) << "\t\t\tstreamID: " << i.streamId;

            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& i) {
                BOOST_LOG_TRIVIAL(debug) 	<< "\t[LISTEN] exiting. Settings internal packets.";
                BOOST_LOG_TRIVIAL(debug) << "\t\t\tstreamID: " << i.streamId;
                
                // set TCP Layer
                i.tcp_s.sport( i.tcp_r.dport() );
                i.tcp_s.dport( i.tcp_r.sport() );
                i.tcp_s.ack_seq( i.tcp_r.seq() + 1 );

                // Set IP Layer
                i.ip_s.src_addr( i.ip_r.dst_addr() );
                i.ip_s.dst_addr( i.ip_r.src_addr() );

                //BOOST_LOG_TRIVIAL(debug) 	<< "\t[LISTEN] After setting IP fields. DST: "
                //							<< i.ip_s.dst_addr() << ", SRC: " << i.ip_s.src_addr();

            }
        };
        struct SYN_RCVD : public msm::front::state<> {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& i) {
                BOOST_LOG_TRIVIAL(debug) << "\t[SYN_RCVD] entering.";
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                BOOST_LOG_TRIVIAL(debug) << "\t[SYN_RCVD] exiting.";
            }
        };
        struct ESTABLISHED : public msm::front::state<> {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& i) {
                BOOST_LOG_TRIVIAL(debug) << "\t[ESTABLISHED] entering.";
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                BOOST_LOG_TRIVIAL(debug) << "\t[ESTABLISHED] leaving.";
            }
        };
        struct FIN_WAIT_1 : public msm::front::state<> {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {
                BOOST_LOG_TRIVIAL(debug) << "\t[FIN_WAIT_1] entering.";
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                BOOST_LOG_TRIVIAL(debug) << "\t[FIN_WAIT_1] leaving.";
            }
        };
        struct TIME_WAIT : public msm::front::state<> {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {
                BOOST_LOG_TRIVIAL(debug) << "\t[TIME_WAIT] entering.";
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                BOOST_LOG_TRIVIAL(debug) << "\t[TIME_WAIT] leaving.";
            }
        };
        struct CLOSING : public msm::front::state<> {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {
                BOOST_LOG_TRIVIAL(debug) << "\t[CLOSING] entering.";
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                BOOST_LOG_TRIVIAL(debug) << "\t[CLOSING] leaving.";
            }
        };
        struct CLOSED : public msm::front::state<> {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {
                BOOST_LOG_TRIVIAL(debug) << "\t[CLOSED] entering.";
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                BOOST_LOG_TRIVIAL(debug) << "\t[CLOSED] leaving.";
            }
        };
        struct SYN_SENT : public msm::front::state<> {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {
                BOOST_LOG_TRIVIAL(debug) << "\t[SYN_SENT] entering.";
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                BOOST_LOG_TRIVIAL(debug) << "\t[SYN_SENT] leaving.";
            }
        };
        struct CLOSE_WAIT : public msm::front::state<> {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {
                BOOST_LOG_TRIVIAL(debug) << "\t[CLOSE_WAIT] entering.";
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& i) {
                BOOST_LOG_TRIVIAL(debug) << "\t[CLOSE_WAIT] leaving.";
                i.tcp_s = TCP();
                i.ip_s = IP();
                i.ip_r = IP();
                i.tcp_r = TCP();
            }
        };
        struct LAST_ACK : public msm::front::state<> {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {
                BOOST_LOG_TRIVIAL(debug) << "\t[LAST_ACK] entering.";
            }
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {
                BOOST_LOG_TRIVIAL(debug) << "\t[LAST_ACK] leaving.";
            }
        };

        // Define initial state
        typedef LISTEN initial_state;

        // transition actions
        void sendSynAck(syn const& evt) {
            BOOST_LOG_TRIVIAL(debug) << "integratio::sendSynAck";

            // State machine properties are visible from here
            tcp_s.flags(TCP::SYN | TCP::ACK);
            
            BOOST_LOG_TRIVIAL(debug)	<< "[TCPResponder][SEND] sendSynAck().";
            responder->send( ip_s / tcp_s, __FUNCTION__ );
        }
        void sendData(psh_ack const&)    {
            BOOST_LOG_TRIVIAL(debug) << "[ESTABLISHED] sendData";

            std::string body = "Zeta spacca i culi!!";
            int size = body.size();
            std::string payload =   "HTTP/1.1 200 OK\r\nDate: Sat, 27 Aug 2016 18:51:19 GMT\r\nContent-length: " \
                                    + std::to_string(size) + "\r\nServer: Apache/2.4.10 (Unix)\r\n\r\n" + body;

            // Prepare and send the packet
            RawPDU* raw = new RawPDU(payload);
            tcp_s.flags(TCP::ACK);
            // tcp_s /= *raw;
            
            BOOST_LOG_TRIVIAL(debug)	<< "[TCPResponder][SEND] sendData().";
            responder->send( ip_s / tcp_s / *(raw), __FUNCTION__ );

            // Remove the payload
            // delete tcp_s.release_inner_pdu();
            // raw = new RawPDU("");

        }

        void sendFinAck(fin_ack const&)    {
            BOOST_LOG_TRIVIAL(debug) << "[CLOSE_WAIT] sendData";
            // Set flags and increase ACK
            tcp_s.flags(TCP::FIN | TCP::ACK);
            tcp_s.ack_seq( tcp_r.seq() + 1 );
            
            responder->send( ip_s / tcp_s, __FUNCTION__ );
        }

        // Guard can be defined as bool function
        //
        // we do not need them here


        // this typedef is simply to make more readable the transition table
        typedef integratio_ i;

        // Transition table for Integratio
        // TODO From CLOSE_WAIT we go back to LISTEN instead of CLOSED

        /*
            row takes 5 arguments: start state, event, target state, action and guard.
            a_row (“a” for action) allows defining only the action and omit the guard condition.
            g_row (“g” for guard) allows omitting the action behavior and defining only the guard.
            _row allows omitting action and guard.
        */
        struct transition_table : mpl::vector<
            //    Start         Event       Next            Action            Guard
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < LISTEN,       syn         , SYN_RCVD ,    &i::sendSynAck                         >,
           _row < SYN_RCVD,     ack         , ESTABLISHED                                          >,
          a_row < ESTABLISHED,  psh_ack     , ESTABLISHED , &i::sendData                           >,
          a_row < ESTABLISHED,  fin_ack     , CLOSE_WAIT  , &i::sendFinAck                         >,
           _row < CLOSE_WAIT,   ack         , LISTEN                                               >
            //  +---------+-------------+---------+---------------------+----------------------+
        > {};
        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& e, FSM&,int state)
        {
            BOOST_LOG_TRIVIAL(debug) 	<< "no transition from state "
            							<< state << " on event " << typeid(e).name();
        }
    };

    // Pick a back-end
    typedef msm::back::state_machine<integratio_> integratio;

    // Testing utilities.
    //
    static char const* const state_names[] = { "LISTEN", "SYN_RCVD", "ESTABLISHED", "FIN_WAIT_1", "TIME_WAIT", "CLOSING", "CLOSED", "SYN_SENT", "CLOSE_WAIT", "LAST_ACK" };
    string pstate(boost::shared_ptr<void> i)
    {
        
        return state_names[boost::static_pointer_cast<integratio>(i)->current_state()[0]];
    }

}

/*
	END STATE MACHINE
*/



Responder::~Responder() {

}

Responder::Responder() {
	
}

TCPResponder::TCPResponder() : stateMachine( new integratio ) {
	BOOST_LOG_TRIVIAL(debug) << "[TCPResponder] Entering constructor";
	// incomingQueue = q;
	boost::static_pointer_cast<integratio>(stateMachine)->setResponder( this );
	boost::static_pointer_cast<integratio>(stateMachine)->start();
}

TCPResponder::~TCPResponder() {
	
}


void TCPResponder::startListen() {
	// This is the method the Thread attach to
    BOOST_LOG_TRIVIAL(debug) << "[TCPResponder][startListen()] ENtering startListener()";	
	bool terminateFlag = true;
	TCP* tcp_r = NULL;

	while(terminateFlag) {

		PDU* pkt = incomingQueue->remove();

		/* DEBUG */
		PDU* p = pkt->inner_pdu();
		while ( p ) {
		
			BOOST_LOG_TRIVIAL(debug)    << pkt->find_pdu<IP>()->src_addr() << ':' << pkt->find_pdu<TCP>()->sport() << " >> " 
		                        		<< pkt->find_pdu<IP>()->dst_addr() << ':' << pkt->find_pdu<TCP>()->dport() << " ("
		                        		<< flags( tcp_r ) << ")";
			p = p->inner_pdu();
		}
		if( pkt != NULL ) {
			tcp_r = pkt->find_pdu<TCP>();
		
			if ( flags( tcp_r ) == "ZFA") {
				return;
			} else {
				processPacket( pkt );
			}
		}
	}
	
}

int TCPResponder::getId() {
    return boost::static_pointer_cast<integratio>(stateMachine)->streamId;
}

wqueue<PDU*> * Responder::getQueue() {
    return this->incomingQueue;
}

void TCPResponder::processPacket(PDU* pdu) {
	BOOST_LOG_TRIVIAL(debug) 	<< "[TCPResponder] Entering processPacket().";
	TCP* tcp_r;
    IP* ip_r;

    TCP& tcp_s = boost::static_pointer_cast<integratio>(stateMachine)->tcp_s;
    IP& ip_s = boost::static_pointer_cast<integratio>(stateMachine)->ip_s;
    int& streamId = boost::static_pointer_cast<integratio>(stateMachine)->streamId;

    // Update local packet (last received)
    if( pdu != NULL ) {
        tcp_r = pdu->find_pdu<TCP>();
        ip_r = pdu->find_pdu<IP>();

        // Skipping this packet if no IP nor TCP
        if ( !tcp_r || !ip_r ) {
        	return;
        }
    }

    if ( ip_s.src_addr().to_string() == "0.0.0.0" ) {
    	ip_s.src_addr( ip_r->dst_addr() );
    	ip_s.dst_addr( ip_r->src_addr() );
    }

    // Get the payload, if any
    
    std::string payload;
    RawPDU* raw = tcp_r->find_pdu<RawPDU>();
    if( raw ){
        const RawPDU::payload_type& pl = raw->payload();
        payload.assign(pl.begin(), pl.end());
    }

    // if client ACK bigger than local SEQ, I update local SEQ
    if( tcp_r->ack_seq() > tcp_s.seq() ){
        if( streamId == -1)
            tcp_s.seq( tcp_r->ack_seq() );
        else {
            if( tcp_r->sport() == streamId ) {
                tcp_s.seq( tcp_r->ack_seq() );
            }
        }
    }

    // Update stateMachine last received packet
    boost::static_pointer_cast<integratio>(stateMachine)->tcp_r = *tcp_r;
    boost::static_pointer_cast<integratio>(stateMachine)->ip_r = *ip_r;

	BOOST_LOG_TRIVIAL(debug)    << "[TCPResponder][processPacket] " << ip_r->src_addr()
								<< ':' << tcp_r->sport() << " >> " 
                        		<< ip_r->dst_addr() << ':' << tcp_r->dport() << " ("
                        		<< flags( tcp_r ) << ") "
                        		<< payload << std::endl;

    // If incoming packet has a payload, I update my local ACK
    if ( payload.size() > 0 && tcp_r->sport() == streamId ) {
        tcp_s.ack_seq( tcp_s.ack_seq() + payload.size() );
    }

    // Finally, if the incoming packet is a SYN, I update the filter
    // to listen only to this stream
    // TODO This still need to be implemented, first need to bring the Sniffer
    //      as member of integratio_.
    //      Also, it will change if I want to spawn separate thread ofr each TCP stream
    if( tcp_r->flags() == TCP::SYN ) {
        if( streamId == -1 ) {
            streamId = tcp_r->sport();
            //i->sniff->SetFilter("tcp and port 80 and (src 192.168.178.37 or src 192.168.178.89 or src 127.0.0.1) and tcp src port " \
            //                    + std::to_string( i->streamId ));
        }

        boost::static_pointer_cast<integratio>(stateMachine)->process_event( syn(*pdu) );
        BOOST_LOG_TRIVIAL(debug) << "[EVENT] SYN";
        return;
    }

    if( tcp_r->flags() == (TCP::PSH | TCP::ACK) ) {
        boost::static_pointer_cast<integratio>(stateMachine)->process_event( psh_ack(*pdu) );
        BOOST_LOG_TRIVIAL(debug) << "[EVENT] PSH-ACK";
        return;
    }

    if( tcp_r->flags() == TCP::ACK ) {
        boost::static_pointer_cast<integratio>(stateMachine)->process_event( ack(*pdu) );
        BOOST_LOG_TRIVIAL(debug) << "[EVENT] ACK";
        return;
    }

    if( tcp_r->flags() == (TCP::FIN | TCP::ACK) ) {
        boost::static_pointer_cast<integratio>(stateMachine)->process_event( fin_ack(*pdu) );
        BOOST_LOG_TRIVIAL(debug) << "[EVENT] FIN-ACK";
        return;
    }

    return;

}

bool TCPResponder::send(IP pdu, string currAction) {

    // Check current state
    //
    //  0 = "LISTEN"
    //  1 = "SYN_RCVD"
    //  2 = "ESTABLISHED"
    //  3 = "FIN_WAIT_1"
    //  4 = "TIME_WAIT"
    //  5 = "CLOSING"
    //  6 = "CLOSED"
    //  7 = "SYN_SENT"
    //  8 = "CLOSE_WAIT"
    //  9 = "LAST_ACK"
    //
    string currState = pstate( stateMachine );

	// used to send Packet
    PacketSender sender;

    BOOST_LOG_TRIVIAL(debug) << "[SEND] Call of custom TCPResponder::send() method";
    BOOST_LOG_TRIVIAL(debug)    << pdu.rfind_pdu<IP>().src_addr() << ':' << pdu.rfind_pdu<TCP>().sport() << " >> " 
                                << pdu.rfind_pdu<IP>().dst_addr() << ':' << pdu.rfind_pdu<TCP>().dport() << " ("
                                << flags( pdu.find_pdu<TCP>() ) << ")" << std::endl; 
    BOOST_LOG_TRIVIAL(debug) << "[SEND] TCPResponder::send() was called by: " << currAction
                                << ", state: " << currState;
    
    // delay use case
    for(TimeConf t : timeTests) {
        if( currAction == t.action && currState == t.state ) {
            sleep_for( seconds( t.delay ) );
        }
    }

    sender.send(pdu);
}


void TCPResponder::addTimeConf(TimeConf t) {
    BOOST_LOG_TRIVIAL(debug)    << "About to push_back timeTests.";
    timeTests.push_back(t);
}

void TCPResponder::addTczConf(TczConf t){
    BOOST_LOG_TRIVIAL(debug)    << "About to push_back tczTests.";
    tczTests.push_back(t);
}
