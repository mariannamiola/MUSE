// For numerical check (double type)

#include "num_check.h"

bool is_integer (const double &value)
{
    int int_value = value;
    double tmp = value-int_value;

    if(tmp > 0)
        return false;
    return true;
}

bool is_real (const double &value)
{
    if(is_integer(value) == false)
        return true;
    return false;
}

bool is_real_positive (const double &value)
{
    if(is_real(value) == true)
        if(value >= 0)
            return true;
    return false;
}

bool is_positive (const double &value)
{
    if(value >= 0)
        return true;
    return false;
}

bool is_bounded (const double &value, const double &inf, const double &sup)
{
    if(value < inf || value > sup)
        return false;
    return true;
}


















//bool getVarCheck (char code, double &value)
//{
//    bool check;

//    if(code == 'R' || code == 'N' || code == 'S' || code == 'B'|| code == 'Y' || code == 'T' || code == 'L' || code == 'R' || code == 'K' || code == 'D' || code == 'A' || code == 'E')
//        check = true;

//    else if(code == 'C' || code == '+')
//        check = is_positive(value);

////    if(code == 'P') //probability: quindi bounded
////        check = checkProbability(values);

////    if(code == 'B') //probability: quindi bounded
////        check = checkBounded(values);

//    return check;
//}



// REAL CONVERSION
// negative_value = true -> trasforming negative values in zeros
// negative_value = false -> considering negative values
// nd_value = true -> trasforming not detected values in zeros
// nd_value = false -> neglecting not detected values
//std::vector<double> rStr_conversion (const std::vector<std::string> &str, std::string negative_value, bool nd_value) //se volessi considerare R+, metto true a negative value
//{
//    std::vector<double> values;

//    for(size_t i=0; i<str.size(); i++)
//    {
//        double val = 0.0;
//        std::string val_tmp = str.at(i);

//        //verificare se v è un numero o una stringa
//        if(val_tmp.compare("nd") != 0)
//        {
//            val = std::stod(val_tmp);
//            if(is_negative(val_tmp) == false) //se è un numero positivo
//                values.push_back(val);
//            else
//            {
//                //come devo considerare i numeri negativi??
//                if(negative_value.compare("YES") == 0)
//                    values.push_back(val);

//                else if(negative_value.compare("ZERO") == 0)
//                    values.push_back(0.0); //considera i valori negativi pari a 0 (per oraaa!!)

//                //altrimenti ancora li trascura
//            }
//        }
//        else if(val_tmp.compare("nd") == 0)
//        {
//            if(nd_value == true)
//                 values.push_back(0.0); //se vero, considera i valori "not detected" pari a 0; altrimenti li esclude
//        }
//        else
//            std::cerr << "ERROR: Unknown format" << std::endl;
//    }
//    std::cout << "R: Conversion from string to double... COMPLETED. " << std::endl;
//    return values;
//}
