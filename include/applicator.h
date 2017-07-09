#ifndef INC_APPLICATOR
#define INC_APPLICATOR

// IOstream
#include <iostream>
#include <unordered_map>
#include "testconf.h"

class Applicator {
    
public:
    std::unordered_map <std::string, std::string> resources;
 
    // Methods
    Applicator();
    ~Applicator();
    std::string getResponse( std::string requestPayload );
    
    
};

class HTTPApplicator: public Applicator {
public:
    HTTPApplicator();
    ~HTTPApplicator();
    bool addResource(ContentConf c); 
};

class DNSApplicator: public Applicator {};
class DHCPApplicator: public Applicator {};
class NTPApplicator: public Applicator {};

#endif  /* INC_APPLICATOR */
