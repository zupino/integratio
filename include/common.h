//#ifndef INC_COMMON
//#define INC_COMMON

// Boost Logging
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

// Testconf for the Test Configuration
#include "testconf.h"

// LibTin
#include <tins/tins.h>

using namespace Tins;

std::string flags(TCP* t);

Conf* jsonConf(std::string);

//#endif /* INC_COMMON */