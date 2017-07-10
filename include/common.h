//#ifndef INC_COMMON
//#define INC_COMMON

// Boost Logging
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

// Testconf for the Test Configuration
#include "listener.h"

// LibTin
#include <tins/tins.h>

using namespace Tins;

std::string flags(TCP* t);

// TODO This might need to return a more generic
//      Listener*, for the moment we have only
//      IP use cases
vector<Conf*> jsonConf(std::string);

// Just a trim() function for the string
std::string trim(const std::string& str,
                 const std::string& whitespace = " \t");


//#endif /* INC_COMMON */
