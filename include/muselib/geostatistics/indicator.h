#ifndef INDICATOR_H
#define INDICATOR_H

#include <vector>
#include <algorithm>

std::vector<int> categories_extraction                  (const std::vector<double> &data);
std::vector<std::string> categories_extraction          (const std::vector<std::string> &data);

std::vector<std::vector<int>> indicator_transformation  (const std::vector<double> &data, const std::vector<int> &categories);


#ifndef STATIC_MUSELIB
#include "indicator.cpp"
#endif

#endif // INDICATOR_H
