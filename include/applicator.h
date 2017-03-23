#ifndef INC_APPLICATOR
#define INC_APPLICATOR

class Applicator {
    
    // Methods
    Applicator();
    ~Applicator();
    std::string getResponse( requestPayload );
    
    
};

class HTTPApplicator: public Applicator {};
class DNSApplicator: public Applicator {};
class DHCPApplicator: public Applicator {};
class NTPApplicator: public Applicator {};

#endif  /* INC_APPLICATOR */