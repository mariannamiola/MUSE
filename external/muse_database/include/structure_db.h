#ifndef STRUCTURE_DB
#define STRUCTURE_DB

#if defined __cplusplus

#include <string>
#include <vector>

typedef std::pair<std::string, std::string> table_header;

const std::string sim_table_name = "SIMULATIONS";

const table_header ra[5] = {
                                table_header ("RECORD_ID", "INTEGER"),
                                table_header ("SIM_ID", "INTEGER"),
                                table_header ("CELL_ID", "INTEGER"),
                                table_header ("VAR_NAME", "STRING"),
                                table_header ("VAR_VALUE", "DOUBLE")
                            };

const std::vector<table_header> sim_table_cols (ra, ra+5);
const std::string sim_table_primary_key = sim_table_cols.at(0).first + " " + sim_table_cols.at(0).second + " PRIMARY KEY AUTOINCREMENT NOT NULL";

#endif
#endif
