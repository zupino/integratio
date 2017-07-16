#include "common.h"
#include "testconf.h"
#include "json.hpp"

using namespace std;

using json = nlohmann::json;

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


/* Tool for reading Test Conf from JSON */

vector<Conf*> jsonConf(std::string jsonConfig) {

	std::vector<Conf*> confs = {};

    // parse explicitly
    json jc = json::parse( jsonConfig );
    // TODO General configuration for IPListener should 
    //      also be read from conf file. Currently harcoded
    //      in IPListner
    IPListener * listener = new IPListener();
    
    for (auto& jconfigs : jc["configs"]){     
         if( jconfigs["category"] == "time" ) {
             TimeConf* t = new TimeConf();

             for( auto& element : jconfigs["parameters"] ) {
                 t->state = element["state"].get<std::string>();
                 t->action = element["action"].get<std::string>();
                 t->delay = element["delay"].get<int>();
             }

             confs.push_back( t );
             
         } else if( jconfigs["category"] == "content") {

             for( auto& element : jconfigs["parameters"]) {
            	 ContentConf* c = new ContentConf();

                 c->resource = element["resource"].get<std::string>();
                 c->http_status = element["http-status"].get<std::string>();
                 c->headers = element["headers"].get<std::string>();
                 c->body = element["body"].get<std::string>();

                 confs.push_back( c );
             }
             
         }  else if( jconfigs["category"] == "packet" ) {
             // packet case
             break;

         } else {
             // default
             break;
         }
    }

    return confs;
}

std::string trim(const std::string& str,
                 const std::string& whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}
