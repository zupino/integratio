#include "applicator.h"
#include "common.h"

// HTTP Parser headers (MIT license)
#include "httpparser/request.h"
#include "httpparser/httprequestparser.h"


Applicator::Applicator() {}
Applicator::~Applicator() {}
HTTPApplicator::HTTPApplicator() {}
HTTPApplicator::~HTTPApplicator() {}

std::string HTTPApplicator::getResponse(std::string requestPayload) {
	// This is the implementation specific for HTTP,
	// payload is assumed to be an HTTP request

	std::string ret;

	httpparser::Request request;
	httpparser::HttpRequestParser parser;

	httpparser::HttpRequestParser::ParseResult res = parser.parse( request,
			requestPayload.c_str(),
			requestPayload.c_str() + strlen(requestPayload.c_str()) );

	if( res == httpparser::HttpRequestParser::ParsingCompleted ) {
		BOOST_LOG_TRIVIAL(debug)	<< "[Applicator] HTTP Parsed correctly. Requested URI: " << request.uri;
	} else {
		BOOST_LOG_TRIVIAL(debug)	<< "[Applicator] Error parsing the HTTP Request";
		return "";
	}

    std::unordered_map<std::string,std::string>::iterator got = resources.find(request.uri);
    if ( got == resources.end() ) {
    	BOOST_LOG_TRIVIAL(debug)	<< "[Applicator] Resource '" << request.uri << "' not found.";
        ret = "";
    }
    else {
    	ret = got->second;
    }

    return ret;

}

/*
 * This is the generic implementation for all Applicator
 * Each specific one should re-implement
 * TODO Mark as virtual method (if not already)
 * */
std::string Applicator::getResponse(string requestPayload) {

	std::string ret;

    std::unordered_map<std::string,std::string>::iterator got = resources.find(requestPayload);
    if ( got == resources.end() ) {
    	BOOST_LOG_TRIVIAL(debug)	<< "[Applicator] Resource not found.";
        ret = "";
    }
    else {
    	ret = got->second;
    }

    return ret;

}


bool HTTPApplicator::addResource(ContentConf c) {
    int bodySize = c.body.length();
    c.headers = trim(c.headers, "\r\n");
    c.headers += "\r\nContent-length: ";
    c.headers += to_string(bodySize);
    c.headers += "\r\n\r\n";
    
    string complete = c.http_status;
    complete += c.headers;
    complete += c.body;
    
    resources[c.resource] = complete;

    return true;
}


