/* testconf.h */
#ifndef INC_COMMON
#define INC_COMMON

#include <list>

using namespace std;

class Conf {
public:
	virtual std::string getCategory() = 0;
	virtual std::string getSubCategory() = 0;
	Conf() {};
	~Conf() {};
};

class TczConf: public Conf {
public:
	string 	state;
	string 	action;
	string 	flag;

	std::string getCategory() {
		return "packet";
	}

	std::string getSubCategory() {
		return "tcp";
	}
};

class IcmzConf: public Conf {
public:
	string 	state;
	string 	action;
	int 	type;
	int 	code;

	std::string getCategory() {
		return "packet";
	}

	std::string getSubCategory() {
			return "icmp";
	}
};

class DnzConf: public Conf {
public:
	string 	q_addr;
	string 	response;

	std::string getCategory() {
		return "content";
	}

	std::string getSubCategory() {
		return "dns";
	}
};

class TimeConf: public Conf {
public:
	string 	state;
	string 	action;
	int 	delay;

	std::string getCategory() {
		return "time";
	}

	std::string getSubCategory() {
		// TODO 	Do we need a subCategory for time?
		//			How can we have a time delay for content?
		return "";
	}
};

class ContentConf: public Conf {
public:
	string 	resource;
	string 	http_status;
	string 	headers;
	string 	body;

	std::string getCategory() {
		return "content";
	}

	std::string getSubCategory() {
		return "http";
	}
};

class TestCase {
public:
	string 	test_id;
	string 	interface;
	int 	lis_port;
	string	category;
	list<Conf>	parameters;
};

#endif /* INC_COMMON */
