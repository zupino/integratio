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

	// This is done here only for development
	connector.responders.push_back( &tcz );

	connector.start();
	// std::thread tc( &Listener::start, connector );
	// tc.join();
}

