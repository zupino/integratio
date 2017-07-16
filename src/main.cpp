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

		    // Allocating new Applicator, just in case.
		    // We need one for each Content Configuration file
		    HTTPApplicator * http = new HTTPApplicator();

		    // Test case Configuration
	        std::ifstream i( fileName );
		    std::string strConf((	std::istreambuf_iterator<char>(i)),
	        						std::istreambuf_iterator<char>() 	);

		    vector<Conf*> configs = jsonConf( strConf );

		    for (Conf* conf: configs) {
            
				if( conf->getCategory() == "content" ) {

					// TODO Cast different *Conf depending on JSON content
					ContentConf* cc = dynamic_cast<ContentConf*>(conf);

					http->addResource( *cc );

					if ( tcz[j-1].httz != http ) {
						tcz[j-1].httz = http;
					}

				} else if ( conf->getCategory() == "time" ) {
					TimeConf* tc = dynamic_cast<TimeConf*>(conf);
					tcz[j-1].timeTests.push_back( *tc );

				}
		    }

		    // Add the configured Responder to the Listener
		    connector.responders.push_back( &tcz[j-1] );
        }
	}

	connector.start();
}
