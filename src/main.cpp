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
	TCPResponder tcz[argc-1];

// Responders are allocating depending on command line arguments passed.
// Each argv[x] is a filename of a json configuration file, that will 
// be associated to a Responder

if( argc>0 ) {

    for( int j=1; j<argc; j++){
		    std::string fileName( argv[j] );

		    // Test case Configuration
	        std::ifstream i( fileName );
		    std::string strConf((	std::istreambuf_iterator<char>(i)),
	        						std::istreambuf_iterator<char>() 	);

		    Conf* conf = jsonConf( strConf );
            // TODO Here simply assuming all tests are TIME test.
		    TimeConf* tc = dynamic_cast<TimeConf*>(conf);

            tcz[j-1].addTimeConf( *tc );
        
            connector.responders.push_back( &tcz[j-1] );
        }
	}

	connector.start();
}
