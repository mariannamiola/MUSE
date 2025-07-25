#ifndef NUM_CHECK_H
#define NUM_CHECK_H

#include <vector>
#include <string>

// Checks on double
bool is_integer         (const double &value);
bool is_real            (const double &value);
bool is_real_positive   (const double &value);
bool is_positive        (const double &value);
bool is_bounded         (const double &value, const double &inf, const double &sup);
bool getCheck           (char code, std::string &value);
//bool getVarCheck        (char code, double &value);


#ifndef STATIC_MUSELIB
#include "num_check.cpp"
#endif

#endif // NUM_CHECK_H
