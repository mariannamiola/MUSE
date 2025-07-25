#include "load_csv.h"

#include <iostream>
#include <fstream>
#include <sstream>

void read_csv (const std::string filename, char delimiter, std::vector<std::vector<std::string>> &matrix)
{
    std::ifstream file;
    file.open(filename, std::fstream::in);
    if (!file.is_open())
    {
        std::cerr << "ERROR while reading in file " << filename << std::endl;
        exit(1);
    }

    std::vector<std::string> row; //to store reading words
    std::string line, word;

    while(getline(file, line))
    {
        row.clear();

        std::stringstream str(line); //creating file related to line

        while(getline(str, word, delimiter))
        {
            if(word.find("\n\r") != std::string::npos)
                word.erase(word.find("\n\r")); //"\n\r" (partire da combinazione)

            if(word.find("\r") != std::string::npos)
                word.erase(word.find("\r")); //"\r" ritorno a capo

            if(word.find("\n") != std::string::npos)
                word.erase(word.find("\n")); //"\n" carattere di nuova riga

            if(word.find("?") != std::string::npos)
                word.erase(word.find("?")); //"?"

            row.push_back(word);
        }
        matrix.push_back(row);
    }

    std::cout << "Number of variables (csv columns): " << matrix[0].size() << std::endl;
    std::cout << "Number of samples (csv rows) for each variable: " << matrix.size() << std::endl;

    std::cout << "\033[0;32mReading CSV file ... COMPLETED. \033[0m" << std::endl;
    std::cout << std::endl;


    file.close();
}


void read_csv_with_header (const std::string filename, int nrows_header,
                           std::vector<std::vector<std::string>> &matrix_header, std::vector<std::vector<std::string>> &matrix_data, char delimiter)
{
    std::ifstream file;
    file.open(filename, std::fstream::in);
    if (!file.is_open())
    {
        std::cerr << "\033[0;31mInput ERROR while reading in csv file: " << filename << "\033[0m" << std::endl;
        exit(1);
    }

    std::vector<std::string> row; //to store reading words
    std::string line, word;

    int count = 0;
    while(getline(file, line))
    {
        count++;
        row.clear();

        std::stringstream str(line); //creating file related to line

        while(getline(str, word, delimiter))
        {

            if(word.find("\n\r") != std::string::npos)
                word.erase(word.find("\n\r")); //"\n\r" (partire da combinazione)

            if(word.find("\r") != std::string::npos)
                word.erase(word.find("\r")); //"\r" ritorno a capo

            if(word.find("\n") != std::string::npos)
                word.erase(word.find("\n")); //"\n" carattere di nuova riga

            if(word.find("?") != std::string::npos)
                word.erase(word.find("?")); //"?"

//            char sc = '"';
//            if(word.find(sc) != std::string::npos)
//            {
//                word.erase(word.find(sc));
//                row.push_back(word);
//            }

            row.push_back(word);
        }

        if(count <= nrows_header)
            matrix_header.push_back(row);
        else
            matrix_data.push_back(row);
    }

    std::cout << "Number of variables (csv columns): " << matrix_data[0].size() << std::endl;
    std::cout << "Number of samples (csv rows) for each variable: " << matrix_data.size() << std::endl;

    std::cout << "\033[0;32mReading CSV file ... COMPLETED. \033[0m" << std::endl;
    std::cout << std::endl;

    file.close();
}

std::string string_printable (const std::string &word)
{
    std::string string_print;
    for(size_t i=0; i<word.size(); i++)
    {
        bool char_is_printable = isprint(word.at(i));
        if(char_is_printable == true)
            string_print.push_back(word.at(i));
    }
    return string_print;
}

size_t nchars_printable (const std::string &word)
{
    size_t n = 0;
    for(size_t i=0; i<word.size(); i++)
    {
        bool char_is_printable = isprint(word.at(i));
        if(char_is_printable == true)
            n++;
    }
    return n;
}

std::string search_column_csv (const std::string filename, int n_column, char delimiter)
{
    std::string name_var; //nome variabile della colonna da ricercare (n_column)

    if(n_column == 0)
        name_var = " "; //stringa vuota

    else
    {
        std::ifstream file;
        file.open(filename, std::fstream::in);
        if (!file.is_open())
        {
            std::cerr << "ERROR while reading in file " << filename << std::endl;
            exit(1);
        }

        std::vector<std::string> row; //to store reading words
        std::string line, word;

        getline(file, line); //prima riga del file
        std::stringstream str(line); //creating file related to line

        int n_col = 0;
        while(getline(str, word, delimiter))
        {
            n_col++;
            if(n_col == n_column)
            {
                name_var = word;
                break;
            }
        }
        file.close();
    }

    // Check on printable chars
    std::string str_printable = name_var;
    if(nchars_printable(str_printable) < str_printable.length())
        name_var = string_printable(str_printable);

    return name_var;
}

void read_header_csv (const std::string filename, int nrows_header, std::vector<std::vector<std::string>> &matrix_header, char delimiter)
{
    std::ifstream file;
    file.open(filename, std::fstream::in);
    if (!file.is_open())
    {
        std::cerr << "\033[0;31mInput ERROR while reading in csv file: " << filename << "\033[0m" << std::endl;
        exit(1);
    }
    std::vector<std::string> row; //to store reading words
    std::string line, word;

    int count = 0;
    while(getline(file, line))
    {
        count++;
        row.clear();

        std::stringstream str(line); //creating file related to line

        while(getline(str, word, delimiter))
        {
            if(word.find("\n\r") != std::string::npos)
                word.erase(word.find("\n\r")); //"\n\r" (partire da combinazione)

            if(word.find("\r") != std::string::npos)
                word.erase(word.find("\r")); //"\r" ritorno a capo

            if(word.find("\n") != std::string::npos)
                word.erase(word.find("\n")); //"\n" carattere di nuova riga

            if(word.find("?") != std::string::npos)
                word.erase(word.find("?")); //"?"

            row.push_back(word);
        }

        if(count <= nrows_header)
            matrix_header.push_back(row);
        else
            break;
    }

    std::cout << "\033[0;32mReading header CSV file ... COMPLETED. \033[0m" << std::endl;
    std::cout << std::endl;

    file.close();
}

bool equal_cell (std::string cell0, std::string cell1)
{
    if(cell0.compare(cell1) == 0)
        return true;
    else
        return false;
}

bool equal_column (std::vector<std::string> col0,std::vector<std::string> col1)
{
    std::vector<bool> results;

    bool is_equal = false;

    if(col0.size() != col1.size())
        std::cerr << "\033[0;31mERROR on header rows number!\033[0m" << std::endl;
    else
    {
        for(size_t i=0; i<col0.size(); i++)
            results.push_back(equal_cell(col0.at(i), col1.at(i)));

        int count_false = 0;
        for(size_t i=0; i<results.size(); i++)
        {
            if(results.at(i) == false)
                count_false++;
        }
        if(count_false == 0)
        {
            is_equal = true;
            std::cout << "\033[0;32mAll elements of headers are equal!\033[0m" << std::endl;
        }
        else
        {
            is_equal = false;
            std::cout << "\033[0;31mElements of headers are different!!\033[0m" << std::endl;
        }
    }
    return is_equal;
}


std::vector<bool> matrix_compare (const std::vector<std::vector<std::string>> &matrix0, const std::vector<std::vector<std::string>> &matrix1)
{
    size_t n_col0, n_col1;
    n_col0 = matrix0.at(0).size();
    n_col1 = matrix1.at(0).size();

    std::vector<bool> results;
    for(size_t i=0; i<n_col0; i++)
    {
        std::vector<std::string> header0 = extracting_kcolumn(matrix0, i);
        std::vector<std::string> header1 = extracting_kcolumn(matrix1, i);

        bool result = equal_column(header0, header1);
        results.push_back(result);
    }
    return results;
}



std::vector<std::pair<int, bool>> matrix_compare1 (const std::vector<std::vector<std::string>> &matrix0, const std::vector<std::vector<std::string>> &matrix1)
{
    size_t n_col0, n_col1;
    n_col0 = matrix0.at(0).size();
    n_col1 = matrix1.at(0).size();

    std::vector<std::pair<int, bool>> results;
    for(size_t i=0; i<n_col0; i++)
    {
        std::vector<std::string> header0 = extracting_kcolumn(matrix0, i);
        std::vector<std::string> header1 = extracting_kcolumn(matrix1, i);


        std::pair<int, bool> result;
        result.first = i;
        result.second = equal_column(header0, header1);
        results.push_back(result);
    }
    return results;
}





std::vector<std::vector<std::string>> extracting_kmatrix (const std::vector<std::vector<std::string>> &matrix, const int &k)
{
    size_t n_rows = matrix.size();
    std::vector<std::vector<std::string>> values (n_rows, std::vector<std::string> (1));

    for(unsigned int i=0; i< n_rows; i++)
        values.at(0).push_back(matrix[i][k]);

    return values;
}





std::vector<std::string> extracting_kcolumn (const std::vector<std::vector<std::string>> &matrix, const int &k)
{
    std::vector<std::string> values;
    unsigned int n_rows = matrix.size();
    for(unsigned int i=0; i< n_rows; i++)
    {
        if(matrix[i][k].empty())
        {
            //std::cout << "VUOTA" << std::endl;
            values.push_back("");
        }
        else
            values.push_back(matrix[i][k]);
    }
    return values;
}

std::vector<std::string> extracting_krow (const std::vector<std::vector<std::string>> &matrix, const int &k)
{
    std::vector<std::string> values;
    unsigned int n_cols = matrix.at(0).size();

    for(unsigned int j=0; j< n_cols; j++)
    {
        values.push_back(matrix[k][j]);
    }
    return values;
}


void save_data (const std::string filename, std::vector<std::string> &values)
{
    std::ofstream file;
    file.open(filename, std::fstream::out);
    if(!file.is_open())
    {
        std::cerr << "Error while reading in file " << filename << std::endl;
        exit(1);
    }
    else
    {
        for(unsigned int i=0; i<values.size(); i++)
            file << values.at(i) << std::endl;
        file.close();
    }
}


void count_whitespace (const std::string &str)
{
    size_t count = 0;
    for (size_t i = 0; i < str.length(); i++)
    {
        int c = str[i];
        if (isspace(c))
            count++;
    }
    if(count == str.length())
        std::cout << "All characted are whitespaces" << std::endl;
}


bool searching_special_chars (const std::string &string, char c)
{
    bool is_found = false;
    if(string.find(c) != std::string::npos)
        is_found = true;
    return is_found;
}


int pos_special_chars (const std::string &string, char c)
{
    int pos = 0;
    if(string.find(c) != std::string::npos)
        pos = string.find(c);
    return pos;
}





