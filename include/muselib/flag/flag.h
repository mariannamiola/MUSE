#ifndef FLAG_H
#define FLAG_H

#include <string>
#include <vector>

namespace MUSE
{
    struct Flag;
}

struct MUSE::Flag
{
    std::string nameFlag;
    char charFlag;
    bool activeFlag;
    bool check;         //memorizza se per quel flag il check è stato superato
};


// Management of flags
MUSE::Flag setFlag      (std::string name, char code, bool is_active = false);
void flagsTable         (std::vector<MUSE::Flag> &table); // Definition of flags table (default)
void restoreTable       (std::vector<MUSE::Flag> &table); // Restore default activeFlag in table
//void flagActivation     (std::vector<MUSE::Flag> &table, const std::string &str_flag);
void flagActivation     (std::vector<MUSE::Flag> &table, const std::string &str_flag);
int count_activeFlag    (std::vector<MUSE::Flag> &table);
int count_passedCheck   (std::vector<MUSE::Flag> &table);


// Check on vector
bool checkString        (const std::vector<std::string> &values);
bool checkReal          (const std::vector<std::string> &values);

bool checkPositive      (const std::vector<std::string> &values);
bool checkNegative      (const std::vector<std::string> &values);

bool checkCompositional (const std::vector<std::string> &values, const double &scale_factor, const double &inf = 0.0, const double &sup = 1.0);
//DA MODIFICARE: + coefficiente di scala in funzione dell'unità di misura


bool checkBounded       (const std::vector<std::string> &values, const double &inf, const double &sup);
bool checkProbability   (const std::vector<std::string> &values, const double &inf = 0.0, const double &sup = 1.0);
bool checkDepth         (const std::vector<std::string> &values);
bool checkAbsHeight     (const std::vector<std::string> &values);
bool checkSoft          (const std::vector<std::string> &values);

void getPreliminaryCheck (char code, std::vector<std::string> &values, bool &check, const double &scale_factor, const double &inf, const double &sup);


#ifndef STATIC_MUSELIB
#include "flag.cpp"
#endif

#endif // FLAG_H
