#include "check.h"
#include <cmath>
#include <cstdlib>

// Check on string

bool is_number(const std::string &str)
{
    // std::string::const_iterator it = str.begin();
    // while (it != str.end() && std::isdigit(*it))
    //     ++it;
    // return !str.empty() && it == str.end();

    char* end = nullptr;
    double val = strtod(str.c_str(), &end);
    return end != str.c_str() && *end == '\0' && val != HUGE_VAL;
}


bool is_double_pos(const std::string &str)
{
    for (size_t i = 0; i < str.length(); i++)
        if (std::isdigit(str[i]) == false && str[i] != '.') //se non è un numero e neanche un punto
            return false;

    return true;
}

bool is_double_neg (const std::string &str)
{
    for (size_t i = 0; i < str.length(); i++)
        if (std::isdigit(str[i]) == false && str[i] != '.' && str[0] != '-') //se non è un numero e neanche un punto
            return false;

    return true;
}

bool is_integer_pos(const std::string &str)
{
    for (size_t i = 0; i < str.length(); i++)
        if (std::isdigit(str[i]) == false)
            return false;

    return true;
}

bool is_integer_neg(const std::string &str)
{
    for (size_t i = 0; i < str.length(); i++)
        if (std::isdigit(str[i]) == false && str[0] != '-')
            return false;

    return true;
}

bool is_negative(const std::string &str)
{
    if (str[0] != '-') //se non c'è il meno come primo carattere della stringa: è positivo, quindi falso
            return false;
    else return true;
}

bool is_positive(const std::string &str)
{
    if (is_negative(str) == false) //se non c'è il meno come primo carattere della stringa: è positivo, quindi falso
            return true;
    else return false;
}

int count_ndvalues (const std::vector<std::string> &values)
{
    int n = 0; //contatore not detected
    for(size_t i=0; i<values.size(); i++)
        if(values.at(i).compare("nd") == 0)
            n++;
    return n;
}

int count_navalues (const std::vector<std::string> &values)
{
    int n = 0; //contatore NA
    for(size_t i=0; i<values.size(); i++)
        if(values.at(i).compare("NA") == 0)
            n++;
    return n;
}

int count_allowedsymbol (const std::vector<std::string> &values)
{
    int n = 0; //contatore symbols
    for(size_t i=0; i<values.size(); i++)
        if(values.at(i).compare("*") == 0)
            n++;
    return n;
}

int count_posvalues (const std::vector<std::string> &values)
{
    int n = 0;
    for(size_t i=0; i<values.size(); i++)
        if(is_number(values.at(i)))
            //if(values.at(i).compare("nd") != 0)
            if(is_positive(values.at(i)))
                n++;
    return n;
}


int count_negvalues (const std::vector<std::string> &values)
{
    int n = 0;
    for(size_t i=0; i<values.size(); i++)
        if(is_number(values.at(i)))
        //if(values.at(i).compare("nd") != 0)
            if(is_negative(values.at(i)))
                n++;
    return n;
}

int count_empty (const std::vector<std::string> &values)
{
    int n = 0;
    for(size_t i=0; i<values.size(); i++)
        if(values.at(i).empty())
            n++;
    return n;
}


bool getCheck (char code, std::string &value)
{
    bool check;

    if(code == 'S' || code == 'Y' || code == 'T' || code == 'L' || code == 'K' || code == 'E')
        check = true;

    else if(code == 'C' || code == 'P' || code == '+')
        check = is_positive(value);

    else if(code == '-')
        check = !is_positive(value);

    else if(code == 'V')
        check = !is_number(value);

    else if(code == 'R' || code == 'D' || code == 'A' || code == 'B' || code == 'N')
        check = is_number(value);
    else
    {
        std::cout << "ERROR: No check is implemented for this flag." << std::endl;
        exit(1);
    }

    return check;
}


//void DataSummary (const MUSE::Data &data)
//{
//    size_t n = data.text_values.size();

//    std::cout << std::endl;
//    std::cout << "Name: " << data.getName() << "; Flag: " << data.getFlag() << "; Description: " << data.getDescription() << std::endl;
//    std::cout << std::endl;

//    std::cout << "Data summary ..." << std::endl;
//    std::cout << "Number of samples: " << n << std::endl;

//    if(data.getFlag() == "A" || data.getFlag() == "R") //ammetto numeri negativi come VALIDI!
//    {
//        std::cout << "Number of not detected (nd) values: " << count_ndvalues(data.text_values) << std::endl;
//        std::cout << "Number of allowed symbols: " << count_allowedsymbol(data.text_values) << std::endl;
//        std::cout << "Number of positive values: " << count_posvalues(data.text_values) << std::endl;
//        std::cout << "Number of negative values: " << count_negvalues(data.text_values) << std::endl;


//        if(count_empty(data.text_values) > 0)
//        {
//            std::cout << "Number of empty cells: " << count_empty(data.text_values) << std::endl;
//            std::cout << "Number of valid values: " << n - count_ndvalues(data.text_values) - count_empty(data.text_values) << std::endl;
//        }
//        else
//            std::cout << "Number of valid values: " << n - count_ndvalues(data.text_values) - count_allowedsymbol(data.text_values) << std::endl;

//        std::cout << "###################################" << std::endl;
//    }
//    else
//    {
//        std::cout << "Number of negative values: " << count_negvalues(data.text_values) << std::endl;
//        std::cout << "Number of not detected (nd) values: " << count_ndvalues(data.text_values) << std::endl;
//        std::cout << "Number of allowed symbols: " << count_allowedsymbol(data.text_values) << std::endl;

//        if(count_empty(data.text_values) > 0)
//        {
//            std::cout << "Number of empty cells: " << count_empty(data.text_values) << std::endl;
//            std::cout << "Number of valid values: " << n - count_ndvalues(data.text_values) - count_negvalues(data.text_values) - count_empty(data.text_values) << std::endl;
//        }
//        else
//            std::cout << "Number of valid values: " << n - count_ndvalues(data.text_values) - count_negvalues(data.text_values) - count_allowedsymbol(data.text_values) << std::endl;

//        std::cout << "###################################" << std::endl;
//    }

//}














//// Check if numerical conversion is possible related to flag
//std::string num_conversion (const std::string &code)
//{
//    std::string do_conversion = " ";

//    if(code == "C" || code == "R+" || code == "R")
//        do_conversion = "DOUBLE";
//    else if(code == "N")
//        do_conversion = "INT";

//    return do_conversion;
//}



//std::vector<double> getConversion (const std::vector<std::string> &variable, const std::string &flag, std::string negative_value, bool nd_value)
//{
//    std::vector<double> conv_variable;
//    if(flag == "C")
//        conv_variable = cStr_conversion(variable, negative_value, nd_value);

//    else if(flag == "R")
//        conv_variable = rStr_conversion(variable, negative_value, nd_value);

//    else if(flag == "R+")
//    {
//        negative_value = true;
//        conv_variable = rStr_conversion(variable, negative_value, nd_value);
//    }

//    else if(flag == "N")
//        conv_variable = nStr_conversion(variable, negative_value, nd_value);

//    return conv_variable;
//}





