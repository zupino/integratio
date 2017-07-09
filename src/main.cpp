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
#include <string>

// Listener
#include "listener.h"

// Test case configuration
#include "common.h"

// Applicator for Content test cases
#include "applicator.h"

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
            
		    if( conf->getCategory() == "content" ) {

				HTTPApplicator * http = new HTTPApplicator();

				// TODO Cast different *Conf depending on JSON content
				ContentConf* cc = dynamic_cast<ContentConf*>(conf);

				// tcz[j-1].addTimeConf( *tc );
				http->addResource( *cc );
				tcz[j-1].httz = http;
				connector.responders.push_back( &tcz[j-1] );
		    } else if ( conf->getCategory() == "time" ) {
		    	TimeConf* tc = dynamic_cast<TimeConf*>(conf);

		    	tcz[j-1].timeTests.push_back( *tc );
		    }

		    // Add the configured Responder to the Listener
		    connector.responders.push_back( &tcz[j-1] );
        }
	}

	connector.start();
}
