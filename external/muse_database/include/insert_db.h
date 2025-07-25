#ifndef INSERT_DB
#define INSERT_DB

#include <sqlite3.h>

#include <string>
#include <vector>

int insert_in_db (sqlite3 *db, const std::vector<std::string> &values);

int insert_in_db (sqlite3 *db, const std::vector<std::vector<std::string> > &values_list);


#ifndef STATIC_LIB
#include "insert_db.cpp"
#endif

#endif
