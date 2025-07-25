#include "flag.h"
#include "check.h"
#include "num_check.h"

#include "muselib/colors.h"

// Management of flags

MUSE::Flag setFlag(std::string name, char code, bool is_active)
{
    MUSE::Flag flag;
    flag.nameFlag = name;
    flag.charFlag = code;
    flag.activeFlag = is_active;
    flag.check = false;

    return flag;
}

// Definition table of flags (default)
void flagsTable(std::vector<MUSE::Flag> &table)
{
    MUSE::Flag f;

    f = setFlag ("COMPOSITIONAL", 'C');
    table.push_back(f);

    f = setFlag("PROBABILITY", 'P');
    table.push_back(f);

    f = setFlag("BOUNDED", 'B');
    table.push_back(f);

    f = setFlag ("REAL", 'R');
    table.push_back(f);

    f = setFlag ("INTEGER", 'N');
    table.push_back(f);

    f = setFlag ("LOGARITHMIC", 'L');
    table.push_back(f);

    f = setFlag ("ERROR", 'E');
    table.push_back(f);

    f = setFlag ("STRING", 'V');
    table.push_back(f);

    f = setFlag("DEPTH", 'D'); //STESSA COSA DI REAL (POSITIVE/NEGATIVE)
    table.push_back(f);

    f = setFlag("ABSOLUTE_HEIGHT", 'A');
    table.push_back(f);

    f = setFlag("TIME", 'T');
    table.push_back(f);

    f = setFlag("DATE", 'Y');
    table.push_back(f);

    f = setFlag("CATEGORIC", 'K');
    table.push_back(f);

    f = setFlag("SOFT", 'S');
    table.push_back(f);

    f = setFlag("SOFT_A", 'a');
    table.push_back(f);

    f = setFlag("SOFT_B", 'b');
    table.push_back(f);

    f = setFlag("SOFT_C", 'c');
    table.push_back(f);

    f = setFlag("HARD", 'H');
    table.push_back(f);

    f = setFlag("GEODESIC", 'G');
    table.push_back(f);

    f = setFlag ("POSITIVE", '+');
    table.push_back(f);

    f = setFlag ("NEGATIVE", '-');
    table.push_back(f);
}


// void flagActivation (std::vector<MUSE::Flag> &table, const std::string &str_flag)
// {
//     for(size_t i= 0; i< str_flag.size(); i++)
//     {
//         if(str_flag[i] != 'R')
//         {
//             for(size_t j=0; j<table.size(); j++)
//                 if(str_flag[i] == table.at(j).charFlag)
//                     table.at(j).activeFlag = true; //aggiorno il flag a true, quando lo trovo nella stringa delle flag
//         }
//         else //se è uguale a R, controllare se c'è un +
//         {
//             if(str_flag.find('+') == std::string::npos) //se non c'è il + (ho solo R)
//             {
//                 for(size_t j=0; j<table.size(); j++)
//                     if(str_flag[i] == table.at(j).charFlag)
//                         table.at(j).activeFlag = true; //aggiorno il flag a true, quando lo trovo nella stringa delle flag
//             }
//             else // se trova il + dopo R
//             {
//                 size_t pos = str_flag.find('+'); //posizione dove c'è il +
//                 std::string code = "REAL_POSITIVE";

//                 for(size_t j=0; j<table.size(); j++)
//                 {
//                     if(code.compare(table.at(j).nameFlag) == 0)
//                         table.at(j).activeFlag = true; //aggiorno il flag a true, quando lo trovo nella stringa delle flag
//                 }
//                 i = pos;
//             }
//         }
//     }
// }

void flagActivation (std::vector<MUSE::Flag> &table, const std::string &str_flag)
{
    for(size_t i= 0; i< str_flag.size(); i++)
    {
        if((str_flag[i] == '+' && str_flag[i-1] == 'D') || (str_flag[i] == '-' && str_flag[i-1] == 'D'))
        {
            i++;
            std::cout << FYEL("### +/--WARNING: checks are not provided in combination with D flag.") << std::endl;
            //non attivo il + o il - come flag da verificare! non hanno lo stesso significato di R+/R-!!!
        }
        else
        {
            for(size_t j=0; j<table.size(); j++)
            {
                if(str_flag[i] == table.at(j).charFlag)
                    table.at(j).activeFlag = true; //aggiorno il flag a true, quando lo trovo nella stringa delle flag
            }
        }
    }
}



void restoreTable (std::vector<MUSE::Flag> &table)
{
    for(size_t i=0; i<table.size(); i++)
    {
        table.at(i).activeFlag = false;
        table.at(i).check = false;
    }
}


int count_activeFlag  (std::vector<MUSE::Flag> &table) //conta il numero di flag attivi
{
    int n_active_flag = 0;

    for(size_t i=0; i<table.size(); i++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
        if(table.at(i).activeFlag == true)
            n_active_flag ++;

    return n_active_flag;
}

int count_passedCheck  (std::vector<MUSE::Flag> &table) //conta il check passati
{
    int n_passed_Check = 0;

    for(size_t i=0; i<table.size(); i++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
        if(table.at(i).check == true)
            n_passed_Check ++;

    return n_passed_Check;
}







// Check on vector

bool checkString (const std::vector<std::string> &values)
{
    std::vector<bool> tmp;

    for(size_t i=0; i< values.size(); i++)
    {
        if(values.at(i).empty())
        {
            std::cerr << "\033[0;33mV-WARNING: Empty row n. "<< i+1 << "\033[0m" << std::endl;
            tmp.push_back(true);
        }

        // Check if string is not a number
        else if (!is_number(values.at(i)))
        {
            bool check_string = true;
            tmp.push_back(check_string);
        }
        else if(values.at(i).compare("nd") == 0) //nd = not detected
            tmp.push_back(true);
        else if(values.at(i).compare("NA") == 0) //NA
            tmp.push_back(true);
        else if(values.at(i).compare("*") == 0) //* = accepted symbol
            tmp.push_back(true);
        else
        {
            tmp.push_back(true);
            //std::cerr << "\033[0;31mV-ERROR on row n. " << i+1 << "; unknown string format: " << values.at(i) << "\033[0m" <<std::endl;
        }
    }

    size_t count_true = 0;
    for(size_t i=0; i< tmp.size(); i++)
    {
        if(tmp.at(i) == true)
            count_true ++;
    }

    if(count_true == values.size())
        return true;
    else
        return false;
}


bool checkReal (const std::vector<std::string> &values)
{
    std::vector<bool> tmp;

    for(size_t i=0; i< values.size(); i++)
    {
        if(values.at(i).empty())
        {
            std::cerr << "\033[0;33mR-WARNING: Empty row n. "<< i+1 << "\033[0m" << std::endl;
            tmp.push_back(true);
        }

        // Check if string is numerical
        else if (is_double_neg(values.at(i)) || is_double_pos(values.at(i)) || is_integer_neg(values.at(i)) || is_integer_pos(values.at(i)))
            tmp.push_back(true);
        else if(values.at(i).compare("nd") == 0) //nd = not detected
            tmp.push_back(true);
        else if(values.at(i).compare("NA") == 0) //NA
            tmp.push_back(true);
        else if(values.at(i).compare("*") == 0) //* = accepted symbol
            tmp.push_back(true);
        else
        {
            tmp.push_back(false);
            std::cerr << "\033[0;31mR-ERROR on row n. " << i+1 << "; unknown string format: " << values.at(i) << "\033[0m" <<std::endl;
        }
    }

    size_t count_true = 0;
    for(size_t i=0; i< tmp.size(); i++)
    {
        if(tmp.at(i) == true)
            count_true ++;
    }

    if(count_true == values.size())
        return true;
    else
        return false;
}


bool checkPositive (const std::vector<std::string> &values)
{
    std::vector<bool> tmp;

    for(size_t i=0; i< values.size(); i++)
    {
        if(values.at(i).empty())
        {
            std::cerr << "\033[0;33m+-WARNING: Empty row n. "<< i+1 << "\033[0m" << std::endl;
            tmp.push_back(true);
        }
        // Check if string is numerical
        else if (is_number(values.at(i)))
        {
            double value = std::stod(values.at(i)); //converto la stringa in double
            bool check_pos = is_positive(value);
            tmp.push_back(check_pos);

            if(check_pos == false)
                std::cerr << "\033[0;31m+-ERROR on row n. " << i+1 << ": Negative value!\033[0m" << std::endl;
        }
        else if(values.at(i).compare("nd") == 0) //nd = not detected
            tmp.push_back(true);
        else if(values.at(i).compare("NA") == 0) //NA
            tmp.push_back(true);
        else if(values.at(i).compare("*") == 0) //* = accepted symbol
            tmp.push_back(true);
        else
        {
            tmp.push_back(false);
            std::cerr << "\033[0;31m+-ERROR on row n. " << i+1 << "; unknown string format: " << values.at(i) << "\033[0m" <<std::endl;
        }
    }

    size_t count_true = 0;
    for(size_t i=0; i< tmp.size(); i++)
    {
        if(tmp.at(i) == true)
            count_true ++;
    }

    if(count_true == values.size())
        return true;
    else
        return false;
}


bool checkNegative (const std::vector<std::string> &values)
{
    std::vector<bool> tmp;

    for(size_t i=0; i< values.size(); i++)
    {
        if(values.at(i).empty())
        {
            std::cerr << "\033[0;33m--WARNING: Empty row n. "<< i+1 << "\033[0m" << std::endl;
            tmp.push_back(true);
        }
        // Check if string is numerical
        else if (is_number(values.at(i)))
        {
            double value = std::stod(values.at(i)); //converto la stringa in double
            bool check_pos = !is_positive(value);
            tmp.push_back(check_pos);

            if(check_pos == false)
                std::cerr << "\033[0;31m--ERROR on row n. " << i+1 << ": Positive value!\033[0m" << std::endl;
        }
        else if(values.at(i).compare("nd") == 0) //nd = not detected
            tmp.push_back(true);
        else if(values.at(i).compare("NA") == 0) //NA
            tmp.push_back(true);
        else if(values.at(i).compare("*") == 0) //* = accepted symbol
            tmp.push_back(true);
        else
        {
            tmp.push_back(false);
            std::cerr << "\033[0;31m--ERROR on row n. " << i+1 << "; unknown string format: " << values.at(i) << "\033[0m" <<std::endl;
        }
    }

    size_t count_true = 0;
    for(size_t i=0; i< tmp.size(); i++)
    {
        if(tmp.at(i) == true)
            count_true ++;
    }

    if(count_true == values.size())
        return true;
    else
        return false;
}


bool checkCompositional (const std::vector<std::string> &values, const double &scale_factor, const double &inf, const double &sup) //fattore di scala: per tenere in conto della variabilità degli estremi (inf/sup) in base all'unità di misura
{
    double inf_sc = inf*scale_factor;
    double sup_sc = sup*scale_factor;

    std::vector<bool> tmp;
    for(size_t i=0; i< values.size(); i++)
    {
        if(values.at(i).empty())
        {
            std::cerr << "C-WARNING: Empty row n. "<< i+1 << std::endl;
            tmp.push_back(true);
        }

        // Check if string is numerical (double/integer/negative)
        else if (is_double_neg(values.at(i)) || is_double_pos(values.at(i)) || is_integer_neg(values.at(i)) || is_integer_pos(values.at(i)))
        //if(values.at(i).compare("nd") != 0)
        {
            double value = std::stod(values.at(i)); //converto la stringa in double
            bool check_bounded = is_bounded(value, inf_sc, sup_sc);

            if(check_bounded == false)
            {
                if(value < inf_sc)
                {
                    //Il numero di riga stampato nell'errore è relativo alla matrice dei dati (ovvero senza le righe di header)
                    std::cerr << "\033[0;33mC-WARNING on row n. " << i+1 << ": Data below DL: " << values.at(i) << "\033[0m"<< std::endl;
                    check_bounded = true;
                }
                else //rimane falso
                    std::cerr << "\033[0;31mC-ERROR on row n. " << i+1 << ": out from limit: [" << inf_sc << ";" << sup_sc <<"]\033[0m" << std::endl;
            }

            tmp.push_back(check_bounded);
        }
        else if(values.at(i).compare("nd") == 0) //nd = not detected
            tmp.push_back(true);
        else if(values.at(i).compare("NA") == 0) //NA
            tmp.push_back(true);
        else
        {
            tmp.push_back(false);
            std::cerr << "\033[0;31mC-ERROR on row n. " << i+1 << "; unknown string format: " << values.at(i) << "]\033[0m" << std::endl;
        }
    }

    size_t count_true = 0;
    for(size_t i=0; i< tmp.size(); i++)
    {
        if(tmp.at(i) == true)
            count_true ++;
    }

    if(count_true == values.size())
        return true;
    else
        return false;
}


bool checkBounded (const std::vector<std::string> &values, const double &inf, const double &sup) //da linea di comando inf/sup
{
    std::cout << "### Inf limit is set on: " << inf << std::endl;
    std::cout << "### Sup limit is set on: " << sup << std::endl;

    std::vector<bool> tmp;
    for(size_t i=0; i< values.size(); i++)
    {
        if(values.at(i).empty())
        {
            std::cerr << "B-WARNING: Empty row n. "<< i+1 << std::endl;
            tmp.push_back(true);
        }

        // Check if string is numerical (double/integer/negative)
        else if (is_double_neg(values.at(i)) || is_double_pos(values.at(i)) || is_integer_neg(values.at(i)) || is_integer_pos(values.at(i)))
        //if(values.at(i).compare("nd") != 0)
        {
            double value = std::stod(values.at(i)); //converto la stringa in double
            bool check_bounded = is_bounded(value, inf, sup);

            if(check_bounded == false)
               std::cerr << "\033[0;31mB-ERROR on row n. " << i+1 << ": out from limit: [" << inf << ";" << sup <<"]\033[0m" << std::endl;

            tmp.push_back(check_bounded);
        }
        else if(values.at(i).compare("nd") == 0) //nd = not detected
            tmp.push_back(true);
        else if(values.at(i).compare("NA") == 0) //NA
            tmp.push_back(true);
        else
        {
            tmp.push_back(false);
            std::cerr << "\033[0;31mB-ERROR on row n. " << i+1 << "; unknown string format: " << values.at(i) << "]\033[0m" << std::endl;
        }
    }

    size_t count_true = 0;
    for(size_t i=0; i< tmp.size(); i++)
    {
        if(tmp.at(i) == true)
            count_true ++;
    }

    if(count_true == values.size())
        return true;
    else
        return false;
}


bool checkProbability (const std::vector<std::string> &values, const double &inf, const double &sup)
{
    return checkBounded(values, inf, sup);
}


bool checkDepth (const std::vector<std::string> &values)
{
    std::cout << FYEL("### D-WARNING: The variable D is checked as REAL ('R' flag).") << std::endl;
    return checkReal(values);
}

bool checkAbsHeight (const std::vector<std::string> &values)
{
    std::cout << FYEL("### A-WARNING: The variable A is checked as REAL ('R' flag).") << std::endl;
    return checkReal(values);
}

bool checkSoft (const std::vector<std::string> &values)
{
    std::cout << "##### SOFT DATA ... " << std::endl;
    std::cout << "##### TO COMPLETE ... " << std::endl;

    return false;
}



void getPreliminaryCheck (char code, std::vector<std::string> &values, bool &check, const double &scale_factor, const double &inf, const double &sup)
{
    if(code == 'N' || code == 'Y' || code == 'T' || code == 'L' || code == 'K' || code == 'E')
       check = true;

    else if(code == 'V')
       check = checkString(values);

    else if(code == 'C')
       check = checkCompositional(values, scale_factor);

    else if(code == 'R')
        check = checkReal(values);

    else if(code == '+')
        check = checkPositive(values);

    else if(code == '-')
        check = checkNegative(values);

    else if(code == 'P') //probability: quindi bounded
        check = checkProbability(values);

    else if(code == 'B') //bounded
        check = checkBounded(values, inf, sup);

    else if(code == 'D') //depth (profondità rispetto lo 0)
        check = checkDepth(values); //questo check richiama il check sui valori reali

    else if(code == 'A') //absolute height (altezza assoluta sul livello del mare)
        check = checkAbsHeight(values); //questo check richiama il check sui valori reali

    else if(code == 'S')
        check = checkSoft(values);
    else
    {
        std::cout << "ERROR: No check is implemented for this flag." << std::endl;
        exit(1);
    }
//     else if(code == '+')
//     {
//         if(prev_code == 'R')
//             check = checkRealPositive(values);
//         else if (prev_code == 'D')
//         {
//             std::cout << "###+-WARNING: Variable must increase with depth D increasing." << std::endl;
//             check = true;
//         }
//     }

//     else if(code == '-')
//     {
//         if (prev_code == 'D')
//         {
//             std::cout << "###--WARNING: Variable must decrease with depth D increasing." << std::endl;
//             check = true;
//         }
//     }
}
