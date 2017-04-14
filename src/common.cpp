#include "common.h"

std::string flags(TCP* t){
    std::string f = "";

    if( t == NULL ) {
        BOOST_LOG_TRIVIAL(debug) << "[ERROR] Null reference to TCP layer.";
        return f;
    }

    if ( t->get_flag( TCP::FIN ) ) {
        f += 'F';
    }
    if ( t->get_flag( TCP::SYN ) ) {
        f += 'S';
    }
    if ( t->get_flag( TCP::RST ) ) {
        f += 'R';
    }
    if ( t->get_flag( TCP::PSH ) ) {
        f += 'P';
    }
    if ( t->get_flag( TCP::ACK ) ) {
        f += 'A';
    }
    if ( t->get_flag( TCP::URG ) ) {
        f += 'U';
    }
    if ( t->get_flag( TCP::ECE ) ) {
        f += 'E';
    }
    if ( t->get_flag( TCP::CWR ) ) {
        f += 'C';
    }

    return f;
}


