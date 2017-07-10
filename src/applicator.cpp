#include "applicator.h"
#include "common.h"

Applicator::Applicator() {}
Applicator::~Applicator() {}
HTTPApplicator::HTTPApplicator() {}
HTTPApplicator::~HTTPApplicator() {}

std::string Applicator::getResponse(string requestPayload) {

	std::string ret;

    std::unordered_map<std::string,std::string>::iterator got = resources.find(requestPayload);
    if ( got == resources.end() ) {
        std::cout << "[Applicator] Resource not found.";
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


