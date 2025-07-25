#ifndef CHECK_H
#define CHECK_H

#include <vector>
#include <string>

#include "muselib/data_structures/data.h"

// Checks on string
bool is_number(const std::string &str);

bool is_double_pos  (const std::string &str);
bool is_double_neg  (const std::string &str);
bool is_integer_pos (const std::string &str);
bool is_integer_neg (const std::string &str);
bool is_negative    (const std::string &str);
bool is_positive    (const std::string &str);

bool getCheck       (char code, std::string &value);

int count_ndvalues  (const std::vector<std::string> &values);
int count_navalues  (const std::vector<std::string> &values);
int count_posvalues (const std::vector<std::string> &values);
int count_negvalues (const std::vector<std::string> &values);
int count_allowedsymbol (const std::vector<std::string> &values);
int count_empty     (const std::vector<std::string> &values);

//void DataSummary    (const MUSE::Data &data);

// Check if numerical conversion is possible related to flag
//std::string num_conversion(const std::string &code);
//std::vector<double> getConversion (const std::vector<std::string> &variable, const std::string &flag, std::string negative_value, bool nd_value);


#ifndef STATIC_MUSELIB
#include "check.cpp"
#endif

#endif // CHECK_H
