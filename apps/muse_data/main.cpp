#include <iostream>
#include <cmath>
#include <string.h>
#include <filesystem>

#include <tclap/CmdLine.h>
#include <matplot/matplot.h>
#include <geostatslib/statistics/stats.h>

//#include "muselib/reference_system/coordinate_systems.h"

#include "muselib/input/load_csv.h"

#include "muselib/utils.h"
#include "muselib/metadata/metadata.h"
#include "muselib/metadata/data_meta.h"
#include "muselib/metadata/data_summary_meta.h"
#include "muselib/metadata/data_summary.h"
#include "muselib/metadata/data_statistics.h"

#include "muselib/data_structures/data.h"
#include "muselib/data_structures/project.h"

#include "muselib/flag/flag.h"
#include "muselib/flag/check.h"
#include "muselib/flag/num_check.h"

#include "muselib/plot/plots.h"

#include "muselib/metadata/metadata.h"

// For color code
// https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal


//for filesystem
#ifdef __APPLE__
    //#include <filesystem>
    using namespace std::__fs;
#else
    //#include <experimental/filesystem>
    using namespace std;
#endif

using namespace MUSE;
using namespace TCLAP;

int main(int argc, char** argv)
{
    std::string app_name = "data"; //app name

    try {
    CmdLine cmd("MUSE - Modelling Uncertainty as a Support for Environment. muse-data application", ' ', "version 0.0");

    std::cout << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "================== STARTING MUSE-DATA ======================" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << std::endl;

    // ---------------------------------------------------------------------------------------------------------
    // MAIN FUNCTIONALITIES:

    // Option 0. New project creation
    /**
     * @brief Initialize a new project directory structure for storing (source point) data
     * @param new_project If set, creates `in/data` and `out/data` subdirectories under the path specified by `-pdir`
     * @note Boolean flag, default: false. Requires `--pdir` to be set and `--input` to optionally copy data file(s)
     * @example muse_data -N -p /path/to/project/dir --input user/path/filename.csv
     */
    SwitchArg projectCreation           ("N", "new_project", "Initialize a new project directory structure for storing (source point) data", cmd, false); //booleano
    
    /**
     * @brief Specify the directory path where the project is created
     * @param pdir Absolute path to the project root directory (mandatory)
     * @note Required when using --new_project flag
     * @example -p /path/to/project/dir
     */
    ValueArg<std::string> projectFolder ("p", "pdir", "Project directory", true, "/path/to/project/dir", "string", cmd);

    /**
     * @brief Copy input file(s) in the input data project directory (path/project/in/data) to replace manual data copy
     * @param input One or more file paths to copy into `data/in/` of the project root.
     * Multiple files can be specified by repeating the flag.
     * @note Optional. Recommended with `-N` to populate the input directory without manual file copying.
     * @example --input user/path1/file1.csv --input user/path2/file2.csv
     */
    MultiArg<std::string> setInput ("i", "input", "Copy input file(s) in the project directory (in/data/)", false, "string", cmd );


    // Option 0a. Project creation - optional: setting project EPSG
    /**
     * @brief Set project EPSG coordinate system
     * @param setEPSG EPSG authority code (default: Unknown)
     */
    ValueArg<std::string> setEPSG       ("", "setEPSG", "Set project EPSG", false, "Unknown", "authority", cmd);

    // Option 1. Project creation - optional: setting coordinate columns
    /**
     * @brief Enable manual setting of coordinate column numbers
     * @param setIDXYZ Flag to set coordinate column numbers
     * @note When using -S/--setIDXYZ, these column flags work together:
     *       - --setID: ID column number
     *       - --setX: X coordinate column number
     *       - --setY: Y coordinate column number
     *       - --setZ: Z coordinate column number (optional)
     * @example -S --setID 0 --setX 1 --setY 2 --setZ 3
     */
    SwitchArg setIDXYZ                  ("S", "setIDXYZ", "Set n. column coordinate", cmd, false); //booleano
    
    /**
     * @brief Set ID column number
     * @param setID Column number for ID field
     */
    ValueArg<int> Colid                 ("", "setID", "Set ID", false, 0, "int" , cmd);
    
    /**
     * @brief Set X coordinate column number
     * @param setX Column number for X coordinate
     */
    ValueArg<int> Colx                  ("", "setX", "Set coordinate x", false, 0, "int" , cmd);
    
    /**
     * @brief Set Y coordinate column number
     * @param setY Column number for Y coordinate
     */
    ValueArg<int> Coly                  ("", "setY", "Set coordinate y", false, 0, "int" , cmd);
    
    /**
     * @brief Set Z coordinate column number
     * @param setZ Column number for Z coordinate
     */
    ValueArg<int> Colz                  ("", "setZ", "Set coordinate z", false, 0, "int" , cmd);

    // Option 2b. Converter function - optional: setting delimiter
    std::vector<std::string> allowedDel = {"DEFAULT", "COMMA"}; //default = ;
    ValuesConstraint<std::string> allowedValsD(allowedDel);
    
    /**
     * @brief Set CSV delimiter type
     * @param setDel Type of CSV delimiter (DEFAULT or COMMA)
     */
    ValueArg<std::string> Delimiter     ("", "setDel", "Set type of csv delimiter", false, "DEFAULT", &allowedValsD, cmd);
    allowedDel.clear();

    // Option 2a. Converter function - SINGLE DATASET
    /**
     * @brief Convert CSV format data into MUSE format
     * @param converter Flag to enable data conversion
     * @note When using -C/--converter, these flags are commonly used:
     *       - --pdir: Project directory (REQUIRED)
     *       - --setDel: CSV delimiter type (optional)
     *       - --inf, --sup: Value limits (optional)
     *       - -S/--setIDXYZ: Manual column mapping (optional)
     * @example -C --pdir /project --setDel COMMA --inf 0.0 --sup 100.0
     */
    SwitchArg converterFunction         ("C", "converter", "Converter data (csv format) into MUSE format", cmd, false); //booleano
    //UnlabeledValueArg<int> n_rowsHeader ("n_rows_header", "Number of header rows", false, 6, "int", cmd);

    // Option 2a. Option for variables check
    //ValueArg<double> scaleFactor        ("", "scale", "Set scale factor unity (only for compositional variables)", false, 1.0, "double", cmd);
    
    /**
     * @brief Set inferior limit for variable values
     * @param inf Inferior limit value
     */
    ValueArg<double> infLimit           ("", "inf", "Set inf limit", false, 0, "inf", cmd);
    
    /**
     * @brief Set superior limit for variable values
     * @param sup Superior limit value
     */
    ValueArg<double> supLimit           ("", "sup", "Set sup limit", false, 1, "sup", cmd);

    // Option 2a. Converter function - optional: setting flag row
//    SwitchArg setFlagRow                ("F", "set_flag", "Set flag row", cmd, false); //booleano
//    ValueArg<int> FlagRow               ("r", "row", "Set row ", false, 0, "row", cmd);

    // Option 3. Reading MUSE format
    /**
     * @brief Enable reading of MUSE format files
     * @param read Flag to enable MUSE format reading
     * @note When using -R/--read, these flags work together:
     *       - --pdir: Project directory (REQUIRED)
     *       - --var: Variable name (default: ALL_INPUT)
     *       - --nrealiz: Number of realizations (optional)
     *       - --hist: Enable histogram plotting (optional)
     * @example -R --pdir /project --var temperature --hist --nbin 20
     */
    SwitchArg readFunction              ("R", "read", "Reading MUSE format", cmd, false); //booleano
    
    /**
     * @brief Specify variable name to read
     * @param var Variable name (default: ALL_INPUT)
     */
    ValueArg<std::string> Variable      ("v", "var", "Variable", false, "ALL_INPUT", "name", cmd);
    
    /**
     * @brief Set number of realization to process
     * @param nrealiz Number of realization (default: 0)
     */
    ValueArg<int> setNrealization       ("n", "nrealiz", "Set number of realization", false, 0, "int", cmd);


    // Option 3a. Options for histograms
    /**
     * @brief Enable histogram computation and plotting
     * @param hist Flag to compute histogram plots
     * @note When using --hist, these flags work together:
     *       - --nval: Minimum number of values for plotting (default: 20)
     *       - --nbin: Number of histogram bins (default: 1)
     *       Requires sufficient data points (>= nval threshold)
     * @example --hist --nval 50 --nbin 25
     */
    SwitchArg getHistogram              ("", "hist", "Compute plot - hitogram", cmd, false); //booleano
    
    /**
     * @brief Set minimum number of values for histogram plotting
     * @param nval Minimum number of values (default: 20)
     */
    ValueArg<int> setNMaxValues         ("", "nval", "Set min number of values, sufficient for histogram plot", false, 20, "int", cmd);
    
    /**
     * @brief Set number of bins for histogram plot
     * @param nbin Number of bins for histogram (default: 1)
     */
    ValueArg<size_t> setNbins           ("", "nbin", "Set number of bins for histogram plot", false, 1, "size_t", cmd);


    // ---------------------------------------------------------------------------------------------------------
    // ADDITIONAL FUNCTIONALITIES:

    /**
     * @brief Enable CSV format for output files
     * @param csv Flag to save files in CSV format
     */
    SwitchArg csvConversion             ("", "csv", "Saving file as csv", cmd, false); //booleano

//    // Option 2a. Converter function - MULTI DATASET
//    SwitchArg mergeDataset              ("M", "multi", "Merge dataset into CSV format", cmd, false); //booleano


    // ---------------------------------------------------------------------------------------------------------
    // PARSING:

    // Parse the argv array.
    cmd.parse(argc, argv);


    // ---------------------------------------------------------------------------------------------------------
    // SETTINGS:

    // 0) Project settings
    MUSE::Project Project;
    Project.setFolder(projectFolder.getValue()); //cartella di progetto
    Project.setName(Project.folder.substr(Project.folder.find_last_of("/")+1, Project.folder.length()));

    // 0) Commands
    std::string command;
    filesystem::path abspath = argv[3];

    for(int i=1; i< argc; i++)
    {
        std::string string = argv[i];
        if(string.find(abspath) != std::string::npos)
        {
            //std::cout << "Path: " << argv[i] << std::endl;

            filesystem::path path = argv[i];
            filesystem::path relpath = filesystem::relative(path, abspath);
            //std::cout << "Relative path: " << relpath << std::endl;

            if(relpath.string().length() > 1)
                command += "./" + relpath.string();
            else
                command += relpath;
            command += " ";
        }
        else
        {
            command += argv[i];
            command += " ";
        }
    }
    std::cout << "=== Command line: " << command << std::endl;
    std::cout << "=== Number of command arguments: " << argc << std::endl;
    std::cout << "=== Absolute path: " << abspath << std::endl;
    std::cout << std::endl;

    // 0) Set folder (in/out)
    std::string in_folder = Project.folder + "/in/" + app_name;
    std::string out_folder = Project.folder + "/out/" + app_name;


    std::string ext = ".dat";
    if(csvConversion.isSet() == true)
        ext = ".csv";


    // ---------------------------------------------------------------------------------------------------------
    // STARTS:

    // Option 0. Project creation and settings
    if(projectCreation.isSet())
    {
        if(!filesystem::exists(in_folder))
            filesystem::create_directory(in_folder); //l'utente inserirà all'interno di questa cartella i dati di input

        if(!filesystem::exists(out_folder))
            filesystem::create_directory(out_folder);

        if(setInput.isSet())
        {
            std::cout << "=== Copy input file in project_dir/in/data/ ..." << std::endl;
            if(setInput.getValue().empty())
                std::cerr << "ERROR: list of files is empty." << std::endl;

            for (const std::string file : setInput.getValue())
            {
                std::string filename = get_filename(file);
                try {
                    filesystem::copy(file, in_folder + "/" + filename,
                             filesystem::copy_options::overwrite_existing);

                    std::cout << "Copied file: " << file << " in the data project directory ... " << in_folder + "/" + filename << std::endl;
                } catch (filesystem::filesystem_error& e) {
                    std::cerr << "ERROR: " << e.what() << std::endl;
                }
            }
        }
        else
        {
            std::cout << "WARNING. No input file is set and copied in project_dir/in/data/" << std::endl;
            std::cout << "Use --input (multiple) flag to currently import data ..." << std::endl;
            std::cout << "... or manually copy input files in project_dir/in/data/" << std::endl;
        }


        // if(setEPSG.isSet())
        // {
        //     Project.authority =  setEPSG.getValue();
        //     datameta.setProject(Project);
        // }

        //datameta.write(out_folder + "/"+ Project.name + ".json");
        //datameta.write(project_json + ".json");

        //std::cout << "\033[0;32mCreation of Project JSON into " << out_folder << " ... COMPLETED.\033[0m" << std::endl;

        //i dati devono contenere informazioni sull'EPSG oppure può essere assegnato manualmente..
        //a questo livello (creazione progetto) vado a predisporre il campo json per l'epsg
        //che poi verrà riempito o modificato manualmente dall'utente in base ai dati
    }


    //L'utente può copiare i dati nella cartella "in" e procedere con la conversione

    // Option 1. Setting coordinate function
    if(setIDXYZ.isSet())
    {
        MUSE::DataMeta datameta;

        char csv_delimiter;
        if(Delimiter.getValue().compare("DEFAULT") == 0) //DEFAULT (;) oppure COMMA
        {
            csv_delimiter = ';';

            std::cout << "WARNING: The default delimiter is ';'. Set --setDel for changing the type." << std::endl;
        }
        if(Delimiter.getValue().compare("COMMA") == 0) //DEFAULT (;) oppure COMMA
            csv_delimiter = ',';


        // Check on input files (.csv)
        if(filesystem::is_empty(in_folder))
        {
            std::cerr << "\033[0;31mInput ERROR: Insert file.csv into: "<< in_folder << "\033[0m" << std::endl;
            exit(1);
        }
        else
        {
            std::vector<std::string> list_csv = get_files(in_folder, ".csv");

            MUSE::InfoData info;

            for(size_t i=0; i< list_csv.size(); i++)
            {
                if(Colid.getValue() > 0)
                    info.id_name = search_column_csv(list_csv.at(i), Colid.getValue(), csv_delimiter);

                if(Colx.getValue() > 0)
                    info.x_name = search_column_csv(list_csv.at(i), Colx.getValue(), csv_delimiter);

                if(Coly.getValue() > 0)
                    info.y_name = search_column_csv(list_csv.at(i), Coly.getValue(), csv_delimiter);

                if(Colz.getValue() > 0)
                    info.z_name = search_column_csv(list_csv.at(i), Colz.getValue(), csv_delimiter);

                //std::string filename = list_csv.at(i).substr(list_csv.at(i).find_last_of("/")+1, list_csv.at(i).length());

                MUSE::DataMeta::CSVFile csv;
                csv.setFilename(get_basename(get_filename(list_csv.at(i))));
                csv.setDelimiter(Delimiter.getValue());
                datameta.setCSVFile(csv);

                datameta.setInfoData(info);

                if(list_csv.size() > 1)
                {
                    std::string out_frame = out_folder + "/" + csv.filename;
                    if(!filesystem::exists(out_frame))
                        filesystem::create_directory(out_frame);

                    datameta.write(out_frame + "/" + csv.filename + ".json");
                    std::cout << "\033[0;32mUpdating JSON file: "<< out_frame + "/" + csv.filename << ".json \033[0m" << std::endl;
                }
                else
                {
                    datameta.write(out_folder + "/" + csv.filename + ".json");
                    std::cout << "\033[0;32mUpdating JSON file: "<< out_folder + "/" + csv.filename << ".json \033[0m" << std::endl;
                }
            }
        }
    }



    // Option 2. Converter function
    if(converterFunction.isSet())
    {
        std::vector<std::string> list_csv = get_files(in_folder, ".csv");

        char csv_delimiter;
        if(Delimiter.getValue().compare("DEFAULT") == 0) //DEFAULT (;) oppure COMMA
        {
            csv_delimiter = ';';
            std::cout << "WARNING: The default delimiter is ';'. Set --setDel for changing the type." << std::endl;
        }
        if(Delimiter.getValue().compare("COMMA") == 0) //DEFAULT (;) oppure COMMA
            csv_delimiter = ',';


        for(size_t i=0; i< list_csv.size(); i++) //per multidataset, fare prima un merge dei file .csv
        {
            // Input
            std::string filename = list_csv.at(i);
            int n_rows_header = 6;

            if(list_csv.size() > 1)
            {
                out_folder.clear();
                out_folder =  Project.folder + "/out/" + app_name + "/" + get_basename(get_filename(filename));
                std::cout << "### MULTIFRAME DATA ANALYSIS ..." << std::endl;
            }

            MUSE::DataMeta datameta;
            datameta.read(out_folder + "/" + get_basename(get_filename(filename)) + ".json");


            // Reading csv and storing into matrix_header/data
            std::vector<std::vector<std::string>> matrix_header;
            std::vector<std::vector<std::string>> matrix_data;
            read_csv_with_header(filename, n_rows_header, matrix_header, matrix_data, csv_delimiter); //opzione sul delimitatore da linea di comando; stessa cosa anche sul separatore decimale!

            size_t n_var = matrix_header[0].size();
            size_t n_files = 0; //numero file creati in formato MUSE

            // Active flag table
            std::vector<Flag> table;
            flagsTable(table);

            int flag_row = 3; //riga della matrice (contando da 1) dove si trova il flag
//            if(setFlagRow.isSet())
//                flag_row = FlagRow.getValue();

            // Preliminary check on values and creation of MUSE Format (json and data file)

            std::cout << "Conversion into MUSE format ..." << std::endl;
            std::cout << "###############################" << std::endl;

            for(size_t i=0; i<n_var; i++)
            {
                std::vector<std::string> header_variable = extracting_kcolumn(matrix_header, i);

                std::string str_flag = header_variable[flag_row-1]; //posizione flag: l'utente la conosce dato che ha costruito il csv!!
                std::string str_unity = header_variable[1]; //posizione unità di misura

                //Check su unità di misura
                double scale_factor = 1.0; // da stabilire in base all'unità di misura (usato per ora solo per le composizionali)
                if(str_unity.compare("%") == 0)
                    scale_factor = scale_factor * 100;
                if(str_unity.compare("ppm") == 0)
                    scale_factor = scale_factor * 1e6; //10^6
                if(str_unity.compare("ppb") == 0)
                    scale_factor = scale_factor * 1e9; //10^9

                // Activation flags in table
                flagActivation(table, str_flag);
                // char prev_plus = '\0';
                // if(str_flag.find('+'))
                //     prev_plus = str_flag[str_flag.find('+')-1];

                // if(str_flag.find('-'))
                //     prev_plus = str_flag[str_flag.find('-')-1];
                // std::cout << prev_plus << std::endl;
                // ////////////////// ATTENZIONE: CONSIDERARE LA CASISTICA IN CUI NELLA STRINGA POSSONO ESSERCI PIÙ + O +E-


                // Data vector to check (related to active flags)
                int n_activeFlag = count_activeFlag(table); //conta il numero di flag attivi
                std::vector<std::string> data_variable = extracting_kcolumn(matrix_data, i);

                for(size_t j=0; j<table.size(); j++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
                {
                    if(table.at(j).activeFlag == true)
                        getPreliminaryCheck(table.at(j).charFlag, data_variable, table.at(j).check, scale_factor, infLimit.getValue(), supLimit.getValue()); //richiama la funzione relativa al check da fare sui valori
                }

                int n_passedCheck = count_passedCheck(table);
                if(n_activeFlag == n_passedCheck) //se i check sono superati per tutti i flag attivi, allora costruisco il formato; altrimenti errore!
                {
                    n_files++;

                    // Check on printable chars
                    for(size_t k=0; k<header_variable.size(); k++)
                    {
                        std::string str_printable = header_variable.at(k);
                        if(nchars_printable(str_printable) < str_printable.length())
                            header_variable.at(k) = string_printable(str_printable);
                    }


                    MUSE::Data data;
                    data.setData(header_variable);
                    std::vector<MUSE::Data> multidata;
                    multidata.push_back(data);

                    MUSE::Metadata meta_input;
                    meta_input.setMultiData(multidata);
                    meta_input.setProject(Project);

                    std::vector<std::string> excommands;
                    excommands.push_back(command);
                    meta_input.setCommands(excommands);

                    std::vector<std::string> deps;
                    filesystem::path realpath = filesystem::relative(filename, abspath);
                    deps.push_back(realpath);
                    meta_input.setDependencies(deps);

                    filesystem::create_directory(out_folder + "/metadata");
                    meta_input.write(out_folder + "/metadata/" + header_variable.at(0) + ".json");
                    //data.write(out_folder + "/metadata/" + header_variable.at(0) + ".json");

                    filesystem::create_directory(out_folder + "/data");
                    save_data(out_folder + "/data/" + header_variable.at(0) + ".dat", data_variable);

                    std::cout << "\033[0;32mAll checks are passed. Creation of MUSE format for variable: " << header_variable.at(0) << "\033[0m" << std::endl;
                    std::cout << std::endl;
                }
                else
                {
                    std::cerr << "\033[0;31mERROR with the creation of MUSE format for variable: " << header_variable.at(0) << "\033[0m" << std::endl;
                    std::cout << std::endl;
                }
                restoreTable(table);
            }

            std::cout << "###############################" << std::endl;
            std::cout << "Conversion into MUSE format ... COMPLETED." << std::endl;
            std::cout << std::endl;

            // Summary about conversion
            if(n_files == n_var)
                std::cout << "\033[0;32mAll variables are converted into MUSE format.\033[0m" << std::endl; //CONVERTED OR ACCEPETED???
            else
                std::cout << "\033[0;32m" << n_files << " on " << n_var << " variables are converted into MUSE format.\033[0m" << std::endl;
            std::cout << std::endl;


            MUSE::DataMeta::DataFormat format;
            format.n_variables = n_var;
            format.n_accepted_variables = n_files;
            datameta.setDataFormat(format);

            std::vector<std::string> excommands;
            excommands.push_back(command);
            datameta.setCommands(excommands);

            datameta.write(out_folder + "/" + get_basename(get_filename(filename)) + ".json");
            std::cout << "\033[0;32mUpdating JSON file: "<< out_folder + "/" + get_basename(get_filename(filename)) << ".json\033[0m" << std::endl;

            matrix_data.clear();
            matrix_header.clear();
        }
    }



    // Option 2. Reading MUSE format
    if(readFunction.isSet() && Variable.getValue().compare("ALL_INPUT") != 0)
    {
        std::string path = get_path(Variable.getValue());
        std::string json_basename = get_basename(get_filename(Variable.getValue()));

        // Reading json file
        MUSE::Metadata data;
        data.read(Variable.getValue());

        PlotStruct dataplot;
        std::vector<MUSE::Data> vec_data = data.getMultiData();

        if(setNrealization.isSet())
        {
            MUSE::Data d = vec_data.at(setNrealization.getValue());
            d.setType(d.flag);

            std::vector<std::string> textValues;
            readTextValues(get_basename(Variable.getValue()) + "_" + d.getDescription() + ext, textValues);
            d.setTextValues(textValues);

            DataSummaryMeta datasum_meta;
            datasum_meta.setData(d);

            DataSummary sum;
            sum.setSummary(d);
            datasum_meta.setSummary(sum);

            if(d.type == NUMBER)
            {
                dataplot.x.clear();

                size_t n_sample = d.text_values.size();

                // Active flag table
                std::vector<Flag> table;
                flagsTable(table);

                // String-double conversion
                for(size_t i =0 ; i<n_sample; i++)
                {
                    //vedo che flag è e faccio il check per entrambi i vettori x e y
                    double val = 0.0;
                    std::string val_tmp = d.text_values.at(i);

                    if(!val_tmp.empty() && val_tmp.compare("nd") !=0) //se la stringa non è vuota ed è diversa da nd
                    {
                        if(val_tmp.compare("*")!=0)
                        {
                            if(val_tmp.compare("NA")!=0)
                            {
                                flagActivation(table, d.getFlag());
                                int n_activeFlag = count_activeFlag(table); //conta il numero di flag attivi
                                for(size_t i=0; i<table.size(); i++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
                                {
                                    if(table.at(i).activeFlag == true)
                                        table.at(i).check = getCheck(table.at(i).charFlag, val_tmp);
                                }
                                int n_passedCheck = count_passedCheck(table);
                                restoreTable(table);

                                if(n_activeFlag == n_passedCheck)
                                {
                                    val = std::stod(val_tmp);
                                    dataplot.x.push_back(val);
                                }
                            }
                        }
                    }
                }

                int n_conv_samples = dataplot.x.size(); //numero campioni convertiti da stringa a double
                if(n_conv_samples == 0)
                    std::cerr << "\033[0;31mERROR: All values are invalid! No statistical data summary.\033[0m" << std::endl;
                else
                {
                    std::cout << "Statistical data summary ..." << std::endl;
                    summary(dataplot.x);

                    Statistics stats;
                    stats.setStatistics(dataplot.x);
                    datasum_meta.setStatistics(stats);

                    if(getHistogram.isSet())
                    {
                        if(n_conv_samples >= setNMaxValues.getValue()) //numero di punti sufficiente per creare un grafico
                        {
                            hist_plot(dataplot, Project.name, d.getName() + " [" + d.getUnit() + "]", "Frequency", setNbins.getValue(), setNbins.isSet());
                            matplot::save(get_basename(Variable.getValue()), "jpeg");
                            matplot::cla();
                            std::cout << std::endl;
                        }
                        else
                            std::cerr << "\033[0;31mERROR: Not created histogram! Number of samples is < " << setNMaxValues.getValue() << ".\033[0m" << std::endl;
                    }
                }
            }

            else
                std::cout << "\033[0;33mWARNING: Histogram plot is irrelevant for data type.\033[0m" << std::endl;

            datasum_meta.write(get_basename(Variable.getValue()) + "_summ.json");
        }
        else
        {
            for(MUSE::Data &d:vec_data)
            {
                d.setType(d.flag);

                std::vector<std::string> textValues;
                readTextValues(get_basename(Variable.getValue()) + "_" + d.getDescription() + ext, textValues);
                d.setTextValues(textValues);

                DataSummaryMeta datasum_meta;
                datasum_meta.setData(d);

                DataSummary sum;
                sum.setSummary(d);
                datasum_meta.setSummary(sum);

                if(d.type == NUMBER)
                {
                    dataplot.x.clear();

                    size_t n_sample = d.text_values.size();

                    // Active flag table
                    std::vector<Flag> table;
                    flagsTable(table);

                    // String-double conversion
                    for(size_t i =0 ; i<n_sample; i++)
                    {
                        //vedo che flag è e faccio il check per entrambi i vettori x e y
                        double val = 0.0;
                        std::string val_tmp = d.text_values.at(i);

                        if(!val_tmp.empty() && val_tmp.compare("nd") !=0) //se la stringa non è vuota ed è diversa da nd
                        {
                            if(val_tmp.compare("*")!=0)
                            {
                                if(val_tmp.compare("NA")!=0)
                                {
                                    flagActivation(table, d.getFlag());
                                    int n_activeFlag = count_activeFlag(table); //conta il numero di flag attivi
                                    for(size_t i=0; i<table.size(); i++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
                                    {
                                        if(table.at(i).activeFlag == true)
                                            table.at(i).check = getCheck(table.at(i).charFlag, val_tmp);
                                    }
                                    int n_passedCheck = count_passedCheck(table);
                                    restoreTable(table);

                                    if(n_activeFlag == n_passedCheck)
                                    {
                                        val = std::stod(val_tmp);
                                        dataplot.x.push_back(val);
                                    }
                                }
                            }
                        }
                    }

                    int n_conv_samples = dataplot.x.size(); //numero campioni convertiti da stringa a double
                    if(n_conv_samples == 0)
                        std::cerr << "\033[0;31mERROR: All values are invalid! No statistical data summary.\033[0m" << std::endl;
                    else
                    {
                        std::cout << "Statistical data summary ..." << std::endl;
                        summary(dataplot.x);

                        Statistics stats;
                        stats.setStatistics(dataplot.x);
                        datasum_meta.setStatistics(stats);

                        if(getHistogram.isSet())
                        {
                            if(n_conv_samples >= setNMaxValues.getValue()) //numero di punti sufficiente per creare un grafico
                            {
                                hist_plot(dataplot, Project.name, d.getName() + " [" + d.getUnit() + "]", "Frequency", setNbins.getValue(), setNbins.isSet());
                                matplot::save(get_basename(Variable.getValue()), "jpeg");
                                matplot::cla();
                                std::cout << std::endl;
                            }
                            else
                                std::cerr << "\033[0;31mERROR: Not created histogram! Number of samples is < " << setNMaxValues.getValue() << ".\033[0m" << std::endl;
                        }
                    }
                }

                else
                    std::cout << "\033[0;33mWARNING: Histogram plot is irrelevant for data type.\033[0m" << std::endl;

                datasum_meta.write(get_basename(Variable.getValue()) + "_summ.json");
            }
        }

        std::cout << std::endl;
        std::cout << "\033[0;32mReading MUSE format and data analysis... COMPLETED.\033[0m" << std::endl;

    }



    // Option 2. Reading MUSE format
    if(readFunction.isSet() && Variable.getValue().compare("ALL_INPUT") == 0)
    {
        std::string abs_datadir = out_folder;
        std::vector<std::string> list_dir = get_directories(abs_datadir);
        if(list_dir.empty())
            list_dir.push_back(abs_datadir);

        int count_frame = 0;
        for(const std::string &l:list_dir)
        {
            count_frame++;

            filesystem::path dir = l;
            filesystem::path rel_datadir = filesystem::relative(dir, abs_datadir);

            std::string sum_folder = out_folder;
            if(rel_datadir.string().compare(".") != 0)
            {
                sum_folder += "/"+ rel_datadir.string();

                std::cout << std::endl;
                std::cout << "###########################" << std::endl;
                std::cout << "### NUMBER OF TIME FRAMES: " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME N° " << count_frame << " ON " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME NAME: " << rel_datadir.string() << std::endl;
                std::cout << std::endl;
            }
            sum_folder += "/summary";

            if(!filesystem::exists(sum_folder))
                filesystem::create_directory(sum_folder);

            std::vector<std::string> list_proj_json = get_files(l, ".json");
            if(list_proj_json.size() > 1)
            {
                std::cerr << "ERROR. Only a file JSON is expected!" << std::endl;
                exit(1);
            }

            // Definition of coordinate variables
            MUSE::DataMeta datameta;
            datameta.read(list_proj_json.at(0));


            // Creation of data structure
            std::vector<Data> dataset; //struttura dati
            std::vector<std::string> list_json = get_files(l + "/metadata", ".json"); //gli devo passare la json_dir

            size_t n_var = list_json.size();
            for(size_t i=0; i< n_var; i++)
            {
                // Funzioni per estrarre la lista dei file contenuti in una cartella: utile per lavorare su tutti i file
                // Apertura multipla: gli devo dare la cartella dei json

                // Defining paths (json and data file)
                std::string json_path = list_json.at(i);

                std::string json_filename;
                json_filename = json_path.substr(json_path.find_last_of("/")+1, json_path.length());

                std::string basename = get_basename(json_filename);

                std::string data_filename = basename +".dat";
                std::string data_path = l +"/data/"+ data_filename;

                // Reading json file
                MUSE::Metadata data;
                data.read(json_path);

                std::vector<MUSE::Data> vec_data = data.getMultiData();

                if(!filesystem::exists(sum_folder + "/" + basename +".json"))
                    filesystem::copy(json_path, sum_folder + "/"+ basename +".json");

                for(MUSE::Data &d:vec_data)
                {
                    d.setType(d.flag);

                    // Set id
                    if(d.getName() == datameta.getInfoData().id_name)
                        d.type = ID;

                    // Set coordinates
                    if(d.getName() == datameta.getInfoData().x_name || d.getName() == datameta.getInfoData().y_name || d.getName() == datameta.getInfoData().z_name)
                        d.type = COORDINATE; //settare quando leggo l'epsg!!

                    std::vector<std::string> textValues;
                    readTextValues(data_path, textValues);
                    d.setTextValues(textValues);

                    dataset.push_back(d);
                }

            }

            std::cout << "\033[0;32mCreating data stucture... COMPLETED.\033[0m"<< std::endl;
            std::cout << std::endl;

            PlotStruct dataplot;
            std::cout << "Reading MUSE format and data analysis ..." << std::endl;
            for(size_t i=0; i<dataset.size(); i++)
            {
                DataSummaryMeta datasum_meta;

                Data var = dataset.at(i);
                datasum_meta.setData(var);

                DataSummary sum;
                sum.setSummary(var);
                datasum_meta.setSummary(sum);


                if(var.type == NUMBER)
                {
                    dataplot.x.clear();

                    size_t n_sample = var.text_values.size();

                    // Active flag table
                    std::vector<Flag> table;
                    flagsTable(table);

                    // String-double conversion
                    for(size_t i =0 ; i<n_sample; i++)
                    {
                        //vedo che flag è e faccio il check per entrambi i vettori x e y
                        double val = 0.0;
                        std::string val_tmp = var.text_values.at(i);

                        if(!val_tmp.empty() && val_tmp.compare("nd") !=0) //se la stringa non è vuota ed è diversa da nd
                        {
                            if(val_tmp.compare("*")!=0)
                            {
                                if(val_tmp.compare("NA")!=0)
                                {
                                    flagActivation(table, var.getFlag());
                                    int n_activeFlag = count_activeFlag(table); //conta il numero di flag attivi
                                    for(size_t i=0; i<table.size(); i++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
                                    {
                                        if(table.at(i).activeFlag == true)
                                            table.at(i).check = getCheck(table.at(i).charFlag, val_tmp);
                                    }
                                    int n_passedCheck = count_passedCheck(table);
                                    restoreTable(table);

                                    if(n_activeFlag == n_passedCheck)
                                    {
                                        val = std::stod(val_tmp);
                                        dataplot.x.push_back(val);
                                    }
                                }
                            }
                        }
                    }

                    int n_conv_samples = dataplot.x.size(); //numero campioni convertiti da stringa a double
                    if(n_conv_samples == 0)
                        std::cerr << "\033[0;31mERROR: All values are invalid! No statistical data summary.\033[0m" << std::endl;
                    else
                    {
                        std::cout << "Statistical data summary ..." << std::endl;
                        summary(dataplot.x);

                        Statistics stats;
                        stats.setStatistics(dataplot.x);
                        datasum_meta.setStatistics(stats);

                        if(getHistogram.isSet() && n_conv_samples >= setNMaxValues.getValue()) //numero di punti sufficiente per creare un grafico
                        {
                            hist_plot(dataplot, Project.name, var.getName() + " [" + var.getUnit() + "]", "Frequency", setNbins.getValue(), setNbins.isSet());
                            matplot::save(sum_folder + "/" + var.getName(), "jpeg");
                            matplot::cla();
                            std::cout << std::endl;
                        }
                        else
                            std::cerr << "\033[0;31mERROR: Not created histogram! Number of samples is < " << setNMaxValues.getValue() << ".\033[0m" << std::endl;
                    }
                }

                else
                    std::cout << "\033[0;33mWARNING: Histogram plot is irrelevant for data type.\033[0m" << std::endl;

                datasum_meta.write(sum_folder + "/"+ var.getName() + ".json");
            }

            std::cout << std::endl;
            std::cout << "\033[0;32mReading MUSE format and data analysis... COMPLETED.\033[0m" << std::endl;
        }
    }

    } catch (ArgException &e)  // catch exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

}





//if(mergeDataset.isSet())
//{
//    std::string project_folder = projectFolder.getValue();
//    std::string project_name = project_folder.substr(project_folder.find_last_of("/")+1, project_folder.length()); //nome progetto

//    // Input folder
//    std::string in_folder = project_folder + "/in/"+app_name;

//    // Output folder
//    std::string out_folder = project_folder + "/out/" + app_name;
//    std::string project_json_file = out_folder +"/"+ project_name + ".json";

//    std::vector<std::string> list_csv = get_files(in_folder, ".csv");
//    //cout_list(list_csv);

//    char csv_delimiter = ';';
//    if(setDelimiter.isSet())
//        if(Delimiter.getValue().compare("COMMA") == 0) //DEFAULT (;) oppure COMMA
//            csv_delimiter = ',';

//    //TO DO: merge sui csv - multidataset

//    int n_rows_header = n_rowsHeader.getValue();

//    // Input
//    std::vector<std::vector<std::string>> matrix_header, matrix_header1, matrix_header_merge, matrix_data, matrix_data1, matrix_data_merge;
//    read_csv_with_header(list_csv.at(0), n_rows_header, matrix_header_merge, matrix_data_merge, csv_delimiter); //opzione sul delimitatore da linea di comando; stessa cosa anche sul separatore decimale!

//    //ciclo sulla lista dei csv e confronto
//    for(size_t i=1; i< list_csv.size(); i++) //per multidataset, fare prima un merge dei file .csv
//    {
//        std::string filename = list_csv.at(i);
//        read_csv_with_header(filename, n_rows_header, matrix_header, matrix_data, csv_delimiter); //opzione sul delimitatore da linea di comando; stessa cosa anche sul separatore decimale!

//        //confronto header: per ogni colonna dell'header, so se almeno un elemento della colonna è diverso -> false
//        std::vector<bool> comp = matrix_compare(matrix_header_merge, matrix_header);

//        for (size_t i=0; i<comp.size(); i++)
//        {
//            if(comp.at(i) == false)
//            {
//                std::vector<std::vector<std::string>> col_mat = extracting_kmatrix(matrix_header, i);
//                matrix_header_merge.insert(matrix_header_merge.end(), col_mat.begin(), col_mat.end());
//            }
//            else
//                std::cout << "NIENTE DA FARE";
//        }

//        ofstream myFile;
//        myFile.open("/Users/mariannamiola/Desktop/MUSE/MUSE_test/proj03/out/input/csv_merge.csv"); // Create file

//        for(size_t i=0; i< matrix_header_merge.size(); i++)
//        {
//            myFile << matrix_header_merge[i][0];

//            for(size_t j=1; j< matrix_header_merge.at(0).size(); j++)
//            {
//                myFile << ";" << matrix_header_merge[i][j];
//                std::cout << matrix_header_merge[i][j] << std::endl;
//            }

//            myFile << std::endl;

//        }
//        myFile.close();
//    }









////            size_t col_equal_header = 0;
////            for (size_t i=0; i<comp.size(); i++)
////                if(comp.at(i) == true)
////                    col_equal_header++;

////            int n_colA = matrix_header_merge.at(0).size();
////            int n_colB = matrix_header.at(0).size();
////            if(col_equal_header != n_colA)
////            {
////                std::vector<std::vector<std::string>> ex_matrix_header (6, std::vector<std::string> (n_colA + n_colB-col_equal_header));


////            }





////        int nets;
////        std::vector<int> netsList; // Will be used to save the numbers until the user is done
////        int maxValue = -1; // The biggest value entered by the user, you could also look in the vector for this value








////            cout << "# of Nets: " << endl;
////            cin >> nets;

////                if (nets > 0) {

////                    netsList.push_back(nets); // Save the number for later
////                    if (nets > maxValue) // Update the maximum value
////                        maxValue = nets;
////                }
////                else {

////                    // Now write to the file, you could open the file now if you wanted
////                    for (int i = 1; i <= maxValue; i++) // Loop until the biggest value is reached (these will be rows)
////                    {
////                        for (auto num : netsList) // Loop all the elements in the vector (columns)
////                        {
////                            if (num >= i) // If the value entered is greater or equal to the current i (row) value
////                                myFile << i << ","; // Write the number
////                            else
////                                myFile << " " << ","; // Write an empty case
////                        }
////                        myFile << "\n";
////                    }

////                    myFile.close(); // Close the file
////                    break; // Now quit
////                }
////            }
















////            if(equal_header == matrix_header.at(0).size())
////            {
////                std::vector<bool> comp = matrix_compare(matrix_data_merge, matrix_data);

////                size_t equal_data = 0;
////                for (size_t i=0; i<comp.size(); i++)
////                    if(comp.at(i) == true)
////                        equal_data++;

////                if(equal_data == matrix_data.at(0).size())
////                    std::cout << "Nothing to merge!!" << std::endl;
////            }


////        }



////        //read_header_csv(list_csv.at(0), n_rows_header, matrix_header0, csv_delimiter);
////        //read_header_csv(list_csv.at(1), n_rows_header, matrix_header1, csv_delimiter);

////        read_csv_with_header(list_csv.at(0), n_rows_header, matrix_header0, matrix_data0, csv_delimiter); //opzione sul delimitatore da linea di comando; stessa cosa anche sul separatore decimale!
////        read_csv_with_header(list_csv.at(1), n_rows_header, matrix_header1, matrix_data1, csv_delimiter); //opzione sul delimitatore da linea di comando; stessa cosa anche sul separatore decimale!

////        std::vector<std::pair<int, bool>> comp = matrix_compare(matrix_header0, matrix_header1);
////        int equal_header = 0;
////        for (int i=0; i<comp.size(); i++)
////        {
////            if(comp.at(i).second == true)
////                equal_header++;
////        }
////        std::vector<std::vector<std::string>> matrix_merge;
////        if(equal_header == matrix_data0.at(0).size())
////        {
////            std::vector<std::vector<std::string>> matrix0;
////            read_csv(list_csv.at(0), csv_delimiter, matrix0);

////            matrix_merge = matrix0;
////            matrix_merge.insert(matrix_merge.end(), matrix_data1.begin(), matrix_data1.end());
////        }
////        else
////        {
////            std::vector<std::vector<std::string>> matrix0;
////            read_csv(list_csv.at(0), csv_delimiter, matrix0);

////            std::vector<std::vector<std::string>> matrix1;
////            read_csv(list_csv.at(1), csv_delimiter, matrix1);

////            for(int i=0; i<matrix0.size(); i++)
////            {
////                std::vector<std::string> row0 = extracting_krow(matrix0, i);
////                std::vector<std::string> row1 = extracting_krow(matrix1, i);

////                std::vector<std::string> row_merge = row0;
////                row_merge.insert(row_merge.end(), row1.begin(), row1.end());
////                matrix_merge.push_back(row_merge);
////            }

////        }


////        for(int i=0; i<comp.size(); i++)
////        {
////            //unisci csv e salva versione
////            int num_col = comp.at(i).first;
////            bool col_is_equal = comp.at(i).second;

////            if(col_is_equal == true)
////            {
////                std::vector<std::string> data_variable = extracting_kcolumn(matrix_data1, num_col);
////                size_t row = matrix_merge.size();
////                for(size_t j=0; j<data_variable.size(); j++)
////                {
////                    matrix_merge.at(row).push_back(data_variable.at(j));
////                    row++;
////                }

////                //colonne in posizione num_col sono uguali
////            }

////        }
//}




//TODO:
//- EPSG: DI PROGETTO; EPSG SINGOLO DATASET - NEL -R DARE L'EPSG DI PROGETTO (DELL'UTENTE)

//- DATASET DIVERSI: CONTROLLARE CHE I METADATI SIANO UGUALI: CONTROLLARE CHE ABBIANO GLI STESSI N

//- DATI IN TEMPO REALE: acquisiamo e ogni 10 secondi aggiorniamo i diagrammi (+ scaricamento memoria dopo che processo i dati!!)
// -> inglobamento codice MATRAC - UDP
//? simulatore di dati (creazione di dati sintetici sopratutto per iniziare)
