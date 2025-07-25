#include "create_db.h"
#include "structure_db.h"

#include <fstream>
#include <iostream>

int open_or_create_db (const std::string db_location, sqlite3 **db)
{
//    std::ifstream f(db_location.c_str());
//    if (f.good())
//    {
//        std::cout << "WARNING: Database " << db_location << " already exists." << std::endl;
//        sqlite3_open(db_location.c_str(), db);
//        return 1;
//    }

    int rc = sqlite3_open(db_location.c_str(), db);

    if (rc != SQLITE_OK)
    {
        std::cerr << "ERROR Opening db : " << rc << std::endl;
        return rc;
    }

    std::string sql_create = "CREATE TABLE IF NOT EXISTS " + sim_table_name + " (";
    sql_create += sim_table_primary_key + ", ";

    for (uint i=1; i < sim_table_cols.size(); i++)
    {
        sql_create += sim_table_cols.at(i).first + " " + sim_table_cols.at(i).second;

        if (i < sim_table_cols.size()-1)
            sql_create += ", ";
        else
            sql_create += ");";
    }

    std::cout << sql_create  << std::endl;

    char *errMsg = 0;
    rc = sqlite3_exec(*db, sql_create.c_str(), NULL, NULL, &errMsg);

    if (rc != SQLITE_OK)
        std::cerr << "ERROR Creating db : " << rc << std::endl;

    return rc;
}
