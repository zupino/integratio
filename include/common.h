#ifndef INC_COMMON
#define INC_COMMON

// Boost Logging
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

// LibTin
#include <tins/tins.h>

using namespace Tins;

std::string flags(TCP* t);

#endif /* INC_COMMON */