#include "stats.h"

double quartile (const std::vector<double> &values, const double &percentile)
{
    double q;

    std::vector<double> v = values;
    sort(v.begin(), v.end());

    double k = v.size()*percentile;

    if(floor(k) == ceil(k)) //se k è intero
        q = (v.at(k-1)+v.at(k))/2;
    else
        q = v.at(ceil(k)-1);

    return q;
}
