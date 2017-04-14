/* testconf.h */

using namespace std;

class Conf {
};

class TczConf: public Conf {
public:
	string 	state;
	string 	action;
	string 	flag;
};

class IcmzConf: public Conf {
public:
	string 	state;
	string 	action;
	int 	type;
	int 	code;
};

class DnzConf: public Conf {
public:
	string 	q-addr;
	string 	response;
};

class TimeConf: public Conf {
public:
	string 	state;
	string 	action;
	int 	delay;
};

class ContentConf: public Conf {
public:
	string 	resource;
	string 	http-status;
	string 	headers;
	string 	body;
};

class TestCase {
public:
	string 	test_id;
	string 	interface;
	int 	lis-port;
	string	category;
	list<Configuration>	parameters;
};