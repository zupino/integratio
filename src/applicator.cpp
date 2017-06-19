#include "applicator.h"
#include "common.h"

string Applicator::getResponse(string requestPayload) {
    return resources[requestPayload];
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

}
