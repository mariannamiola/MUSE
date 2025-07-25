#ifndef LOAD_CSV_H
#define LOAD_CSV_H

#include <string>
#include <vector>

void read_csv                   (const std::string filename, char delimiter, std::vector<std::vector<std::string>> &matrix);
void read_csv_with_header       (const std::string filename,
                                   int nrows_header,
                                   std::vector<std::vector<std::string>> &matrix_header,
                                   std::vector<std::vector<std::string>> &matrix_data,
                                   char delimiter = ';');

std::string string_printable    (const std::string &word);
size_t nchars_printable(const std::string &word);

std::string search_column_csv   (const std::string filename, int n_column, char delimiter = ';');

std::vector<std::string> extracting_kcolumn (const std::vector<std::vector<std::string>> &matrix, const int &k);
std::vector<std::vector<std::string>> extracting_kmatrix (const std::vector<std::vector<std::string>> &matrix, const int &k);
std::vector<std::string> extracting_krow (const std::vector<std::vector<std::string>> &matrix, const int &k);

void save_data                  (const std::string filename, std::vector<std::string> &values);

void count_whitespace           (const std::string &str);
bool searching_special_chars    (const std::string &string, char c);
int pos_special_chars           (const std::string &string, char *c);




void read_header_csv (const std::string filename, int nrows_header, std::vector<std::vector<std::string>> &matrix_header, char delimiter);
bool equal_cell (std::string cell0, std::string cell1);
bool equal_column (std::vector<std::string> col0,std::vector<std::string> col1);
std::vector<bool> matrix_compare (const std::vector<std::vector<std::string>> &matrix0, const std::vector<std::vector<std::string>> &matrix1);
std::vector<std::pair<int, bool>> matrix_compare1 (const std::vector<std::vector<std::string>> &matrix0, const std::vector<std::vector<std::string>> &matrix1);

#ifndef STATIC_MUSELIB
#include "load_csv.cpp"
#endif

#endif // LOAD_CSV_H
