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

Listener::Listener() {

}

Listener::~Listener() {

}

IPListener::IPListener() {

	// Set local address
	localAddress = IPv4Address("192.168.178.89");
	remoteAddress = IPv4Address("192.168.178.37");

	// Set the sniffer
	snifferConfig.set_filter( "ip and tcp and (tcp port 80) and (ip src 192.168.178.37 or ip src 192.168.178.89) and (ip dst " + localAddress.to_string() + " or ip dst " + remoteAddress.to_string() + ")");
	snifferConfig.set_immediate_mode(true);
	
	sniffer = new Sniffer("eth0", snifferConfig);
	

}

IPListener::~IPListener() {

}

bool Listener::pktCallback(PDU &pdu) {

    TCP* tcp_r;
    IP* ip_r;

    if( &pdu ){
    	// Update local packet (last received)
    	tcp_r = pdu.find_pdu<TCP>();
    	ip_r = pdu.find_pdu<IP>();


		BOOST_LOG_TRIVIAL(debug)    << "[CheckERROR]" << ip_r->src_addr() << ':' << tcp_r->sport() << " >> " 
	                        		<< ip_r->dst_addr() << ':' << tcp_r->dport() << " ("
	                        		<< flags( tcp_r ) << ") ";
	                        		// << payload << std::endl;


		/* DEBUG */
		PDU* p = pdu.inner_pdu();
		while( p ) {
			BOOST_LOG_TRIVIAL(debug)	<< "[CheckERROR] inner_pdu inserted in the Queue: " << p;
			BOOST_LOG_TRIVIAL(debug)    << ip_r->src_addr() << ':' << tcp_r->sport() << " >> " 
	                        			<< ip_r->dst_addr() << ':' << tcp_r->dport() << " ("
	                        			<< flags( tcp_r ) << ") " << p->pdu_type();
			p = p->inner_pdu();
		}
		/* DEBUG */


		pktQueue->add( pdu.clone() );
		
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





