// Boost Logging
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

// To read conf file
#include <fstream>
#include <streambuf>

// Threads!
#include <thread>

// IOstream
#include <iostream>

// Listener
#include "listener.h"

// Test case configuration
#include "common.h"

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
	TCPResponder tcz1;
    TCPResponder tcz2;

if( argc>0 ) {

		std::string fileName( argv[1] );

		// Test case Configuration
	    std::ifstream i( fileName );
		std::string strConf((	std::istreambuf_iterator<char>(i)),
	    						std::istreambuf_iterator<char>() 	);


	    
		Conf* conf = jsonConf( strConf );
		TimeConf* tc = dynamic_cast<TimeConf*>(conf);
		tcz1.addTimeConf( *tc );
        tcz2.addTimeConf( *tc );
	}

	// This is done here only for development
	connector.responders.push_back( &tcz1 );
    connector.responders.push_back( &tcz2 );

	connector.start();
}
