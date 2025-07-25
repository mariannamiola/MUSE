#include "create_db.h"
#include "insert_db.h"

#include <tclap/CmdLine.h>

#include <iostream>

int main (int argc, char **argv)
{
    try {

        TCLAP::CmdLine cmd("MUSE Database", ' ', "0.9");

        TCLAP::ValueArg<std::string> dbArg("d","db","DB Location and name",true,"db","string");

        cmd.add( dbArg );

        // Parse the argv array.
        cmd.parse( argc, argv );

        std::string database_path = dbArg.getValue();
        sqlite3 *db = nullptr;

        open_or_create_db(database_path.c_str(), &db);

        std::vector<std::vector<std::string> > values_list;
        std::vector<std::string> values (sim_table_cols.size()-1);

        for (uint s=0; s < 20; s++) // For each Simulation
        for (uint c=0; c < 10; c++) // For each Cell
        {
            values.at(0) = std::to_string(s);   // # Simulation
            values.at(1) = std::to_string(c);   // # Cell
            values.at(2) = "variable_name";     // Variable name
            values.at(3) = std::to_string(1.34);    // Variable Value

            values_list.push_back(values);
        }

        insert_in_db(db, values_list);

        sqlite3_close(db);

    } catch (std::exception e) {

    std::cerr << e.what() << std::endl;

    }


    return 0;
}
