#ifndef CREATE_DB
#define CREATE_DB

#include <sqlite3.h>

#include <string>

int open_or_create_db (const std::string db_location, sqlite3 **db);

#ifndef STATIC_LIB
#include "create_db.cpp"
#endif

#endif

