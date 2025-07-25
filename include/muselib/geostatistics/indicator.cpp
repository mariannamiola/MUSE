#include "indicator.h"

// TO DO: da estendere con le stringhe!!

std::vector<int> categories_extraction (const std::vector<double> &data)
{
    std::vector<double> data_tmp = data;
    sort(data_tmp.begin(), data_tmp.end());

    std::vector<int> categories;
    categories.push_back(data_tmp.at(0));
    for(size_t i=1; i<data_tmp.size(); i++)
    {
        if(data_tmp.at(i) != data_tmp.at(i-1))
            categories.push_back(data_tmp.at(i));
    }

    //sort(categories.begin(), categories.end());
    categories.erase(unique(categories.begin(), categories.end()), categories.end());

    data_tmp.clear();

    return categories;
}

std::vector<std::string> categories_extraction (const std::vector<std::string> &data)
{
    std::vector<std::string> categories;
    categories.push_back(data.at(0));

    for(size_t i=1; i<data.size(); i++)
    {
        if(data.at(i).compare(data.at(i-1)) != 0)
            categories.push_back(data.at(i));
    }

    sort(categories.begin(), categories.end());
    categories.erase(unique(categories.begin(), categories.end()), categories.end());

    return categories;
}

std::vector<std::vector<int>> indicator_transformation (const std::vector<double> &data, const std::vector<int> &categories)
{
    std::vector<std::vector<int>> indicator_matrix (categories.size());
    for(size_t i=0; i<categories.size(); i++)
    {
        for(size_t j=0; j<data.size(); j++)
        {
            if(data.at(j) == categories.at(i))
                indicator_matrix.at(i).push_back(1);
            else
                indicator_matrix.at(i).push_back(0);
        }
    }

    return indicator_matrix;

//    for(size_t row=0; row<data.size(); row++)
//    {
//        for(size_t col=0; col<categories.size(); col++)
//        {
//            if(data.at(row) == categories.at(col))
//                indicator_matrix.at(row).push_back(1);
//            else
//                indicator_matrix.at(row).push_back(0);
//        }
//    }
}



