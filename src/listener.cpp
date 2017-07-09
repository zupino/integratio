#include "listener.h"
#include "common.h"
#include "responder.h"

// LibTin
#include <tins/tins.h>

// Boost Logging
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

// Thread
#include <thread>

bool syn_flag = true;

Listener::Listener() {

}

Listener::~Listener() {

}

IPListener::IPListener() {

	// Set local address
	localAddress = IPv4Address("192.168.178.89");
	remoteAddress = IPv4Address("192.168.178.37");

	// Set the sniffer
	snifferConfig.set_filter(  	"ip and tcp and (tcp port 80) \
								and ((ip src 192.168.178.37) or \
								(ip src 192.168.178.89)) and \
								((ip dst " + localAddress.to_string() + 
								") or (ip dst " + remoteAddress.to_string() + "))");
	snifferConfig.set_immediate_mode(true);
	
	sniffer = new Sniffer("lo", snifferConfig);
	

}

IPListener::~IPListener() {

}

bool Listener::pktCallback(PDU &pdu) {
    BOOST_LOG_TRIVIAL(debug) << "[Listener][pktCallback()] Entering pktCallback()";
    TCP* tcp_r;
    IP* ip_r;

    if( &pdu ){
    	// Update local packet (last received)
    	// tcp_r = pdu.find_pdu<TCP>();
    	// ip_r = pdu.find_pdu<IP>();

//
//	Locking a listener on the TCP src port of the first
//	SYN pkt received.     	
//
/*
		if( syn_flag && (tcp_r->flags() == TCP::SYN) ) {
			syn_flag = false;
			BOOST_LOG_TRIVIAL(debug) 	<< "\t\t--> SYN incoming, setting flag: " << syn_flag;

			// Set the sniffer
			sniffer->set_filter(  	"ip and tcp and (tcp port 80) \
								and ((ip src 192.168.178.37) or \
								(ip src 192.168.178.89)) and \
								((ip dst " + localAddress.to_string() + 
								") or (ip dst " + remoteAddress.to_string() + ")) and \
								(tcp dst port" + to_string( tcp_r->dport() ) + ")");


		}
*/
		forwardPacket( pdu.clone() );
		
    }

    return true;
}

bool Listener::configure( std::string jsonConfig ) {
	// Here the processing of JSON config file should be done
	// in particular, the external part of the JSON will be processed here

}

void Listener::start() {
	// Start listen for all Responders

	list<Responder*>::iterator iter;
	BOOST_LOG_TRIVIAL(debug) << "[Listener] about to loop on Responders";
	for (iter = responders.begin() ; iter != responders.end(); iter++){
		BOOST_LOG_TRIVIAL(debug) << "[Listener] Creating Thread for Responder";
		std::thread t( &Responder::startListen, (*iter) );
		t.detach();
	}

	sniffer->sniff_loop( make_sniffer_handler(this, &Listener::pktCallback) );
}

bool Listener::forwardPacket(PDU* pkt) {
    BOOST_LOG_TRIVIAL(debug) << "[Listner][forwardPacket()] Entering forwardPacket()";

    TCP* tcp_r;
    int stream = 0;
    list<Responder*>::iterator it;

    if( pkt != NULL ) {
        tcp_r = pkt->find_pdu<TCP>();
        stream = tcp_r->sport();
        bool sent = false;

        for(it = responders.begin(); it != responders.end(); it++) {
            if ((*it)->getId() == stream) {
                (*it)->getQueue()->add(pkt);
                BOOST_LOG_TRIVIAL(debug)    << "[Listener][forwardPacket()] pkt forward to existing stream";
                return true;
            }
        }

        for(it = responders.begin(); it != responders.end(); it++) {
                if ((*it)->getId() == -1) {
                    (*it)->getQueue()->add(pkt);
                    BOOST_LOG_TRIVIAL(debug)    << "[Listener][forwardPacket()] pkt forward to new stream";
                    return true;
                }
            }

        BOOST_LOG_TRIVIAL(debug)    <<  "[Listener][forwardPacket()] Packet not matching with any strem," <<
                                        " and all Responders are busy";
        return false;
    }
        
    else {
        BOOST_LOG_TRIVIAL(debug)    << "[Listener][forwardPacket()] The pkt does not have TCP layer.";
        return false;
    }

}

















