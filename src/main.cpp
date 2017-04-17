// Boost Logging
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

// Threads!
#include <thread>

// IOstream
#include <iostream>

// Listener
#include "listener.h"

namespace logging = boost::log;
namespace mpl = boost::mpl;

// Initialize the logging filter. 
// trace, debug, info, warning, error, fatal
void initLog()
{
    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::debug
    );
}

int main( int argc, char *argv[] ) {
	initLog();

	IPListener connector;
	TCPResponder tcz( connector.pktQueue );

	// This will be done by a Configurator
	TimeConf t1;
	t1.action = "sendSynAck";
	t1.state = "LISTEN";
	t1.delay = 3;

	TimeConf t2;
	t2.action = "sendFinAck";
	t2.state = "ESTABLISHED";
	t2.delay = 3;

	tcz.addTimeConf(t1);
	tcz.addTimeConf(t2);

	// This is done here only for development
	connector.responders.push_back( &tcz );

	connector.start();
	// std::thread tc( &Listener::start, connector );
	// tc.join();
}

