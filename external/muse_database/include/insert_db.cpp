#include "insert_db.h"
#include "structure_db.h"

#include <iostream>

// Create a callback function
int callback(void *NotUsed, int argc, char **argv, char **azColName){

    // int argc: holds the number of results
    // (array) azColName: holds each column returned
    // (array) argv: holds each value

    for(int i = 0; i < argc; i++) {

        // Show column name, value, and newline
        std::cout << azColName[i] << ": " << argv[i] << std::endl;

    }

    // Insert a newline
    std::cout << std::endl;

    // Return successful
    return 0;
}

int insert_in_db (sqlite3 *db, const std::vector<std::string> &values)
{
    if (db == nullptr) return 1;

    sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0);

    std::string sql_insert = "INSERT INTO " + sim_table_name + " (";

    for (uint i=1; i < sim_table_cols.size(); i++)
    {
        sql_insert += sim_table_cols.at(i).first;

        if (i <  sim_table_cols.size()-1)
            sql_insert += ", ";
        else
            sql_insert += ")";
    }

    sql_insert += " VALUES (";

    for (uint i=0; i < values.size(); i++)
    {
        if (sim_table_cols.at(i+1).second.compare("STRING") == 0)
            sql_insert += "'";

        sql_insert += values.at(i);

        if (sim_table_cols.at(i+1).second.compare("STRING") == 0)
            sql_insert += "'";

        if (i < values.size()-1)
            sql_insert += ", ";
        else
            sql_insert += ");";
    }

    //std::cout << sql_insert << std::endl;

    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql_insert.c_str(), callback, 0, &errMsg);

    if (rc != SQLITE_OK)
        std::cerr << "ERROR: " << rc << /*" : " << errMsg <<*/ std::endl;

    sqlite3_exec(db, "END TRANSACTION", 0, 0, 0);

    return rc;
}

int insert_in_db (sqlite3 *db, const std::vector<std::vector<std::string> > &values_list)
{
    if (db == nullptr) return 1;

    int rc = SQLITE_OK;

    sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0);

    for (const std::vector<std::string> &values : values_list)
    {

        std::string sql_insert = "INSERT INTO " + sim_table_name + " (";

        for (uint i=1; i < sim_table_cols.size(); i++)
        {
            sql_insert += sim_table_cols.at(i).first;

            if (i <  sim_table_cols.size()-1)
                sql_insert += ", ";
            else
                sql_insert += ")";
        }

        sql_insert += " VALUES (";

        for (uint i=0; i < values.size(); i++)
        {
            if (sim_table_cols.at(i+1).second.compare("STRING") == 0)
                sql_insert += "'";

            sql_insert += values.at(i);

            if (sim_table_cols.at(i+1).second.compare("STRING") == 0)
                sql_insert += "'";

            if (i < values.size()-1)
                sql_insert += ", ";
            else
                sql_insert += ");";
        }

        //std::cout << sql_insert << std::endl;

        char *errMsg = 0;
        rc = sqlite3_exec(db, sql_insert.c_str(), callback, 0, &errMsg);

        if (rc != SQLITE_OK)
            std::cerr << "ERROR: " << rc << /*" : " << errMsg <<*/ std::endl;
    }

    sqlite3_exec(db, "END TRANSACTION", 0, 0, 0);

    return rc;
}
