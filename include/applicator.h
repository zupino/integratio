#ifndef INC_APPLICATOR
#define INC_APPLICATOR

#include <unordered_map>
#include "testconf.h"
class Applicator {
    
public:
    unordered_map <string, string> resources;
 
    // Methods
    Applicator();
    ~Applicator();
    string getResponse( string requestPayload );
    
    
};

class HTTPApplicator: public Applicator {
    bool addResource(ContentConf c); 
};

class DNSApplicator: public Applicator {};
class DHCPApplicator: public Applicator {};
class NTPApplicator: public Applicator {};

#endif  /* INC_APPLICATOR */
