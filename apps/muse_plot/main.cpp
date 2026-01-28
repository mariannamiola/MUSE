#include <iostream>
#include <filesystem>

#include <tclap/CmdLine.h>

#include "muselib/utils.h"
#include "muselib/plot/plots.h"

#include "muselib/flag/flag.h"
#include "muselib/flag/check.h"
#include "muselib/flag/num_check.h"

#include "muselib/data_structures/data.h"

#include "muselib/metadata/data_meta.h"


//for filesystem
#ifdef __APPLE__
    using namespace std::__fs;
#else
    using namespace std;
#endif


using namespace MUSE;
using namespace TCLAP;


int main(int argc, char** argv)
{
    std::cout << std::endl;
    std::cout << "########### STARTING MUSE-PLOT ..." << std::endl;
    std::cout << std::endl;

    std::string app_name = "plot";      //app name
    std::string app_data = "data";

    try {
    CmdLine cmd("MUSE = Modelling of Uncertainty as a Support of Environment; Plot tool", ' ', "version 0.0");

    // ---------------------------------------------------------------------------------------------------------
    // MAIN FUNCTIONALITIES:

    // Option 0.
    /**

     * @brief Creation histogram plot

     * @param histogram Enable creation histogram plot

     */

    SwitchArg histogramPlot             ("H", "histogram", "Creation histogram plot", cmd, false); //booleano
    /**

     * @brief Project directory

     * @param pdir Path to project directory

     */

    ValueArg<std::string> projectFolder ("p", "pdir", "Project directory", true, "Directory", "path", cmd);
    /**

     * @brief Set values

     * @param val values

     */

    ValueArg<std::string> setValues     ("v", "val", "Set values", false, "File path", "file", cmd);

    /**


     * @brief Set min number of values, sufficient for histogram plot


     * @param nval Number of set min number of values, sufficient for histogram plot


     */


    ValueArg<int> setNMaxValues         ("", "nval", "Set min number of values, sufficient for histogram plot", false, 20, "int", cmd);
    /**

     * @brief Set number of bins for histogram plot

     * @param nbin Number of set number of bins for histogram plot

     */

    ValueArg<size_t> setNbins           ("", "nbin", "Set number of bins for histogram plot", false, 1, "size_t", cmd);

    /**


     * @brief Name variable1


     * @param x_variable Name of name variable1


     */


    ValueArg<std::string> Variable1     ("x", "x_variable", "Name variable1", false, "File path", "file", cmd);

    // Option 1a.
    /**

     * @brief Creation bivariate plot

     * @param bivariate_plot Enable creation bivariate plot

     */

    SwitchArg bivariatePlot             ("B", "bivariate_plot", "Creation bivariate plot", cmd, false); //booleano
    /**

     * @brief Name variable2

     * @param y_variable Name of name variable2

     */

    ValueArg<std::string> Variable2     ("y", "y_variable", "Name variable2", false, "File path", "file", cmd);

    // Option 2.
    /**

     * @brief Creation error plot

     * @param error_plot Enable creation error plot

     */

    SwitchArg errorPlot                 ("E", "error_plot", "Creation error plot", cmd, false); //booleano

    // Option 3.
    /**

     * @brief Creation triangular plot

     * @param triangular_plot Enable creation triangular plot

     */

    SwitchArg triangularPlot            ("T", "triangular_plot", "Creation triangular plot", cmd, false); //booleano
    /**

     * @brief Name variable3

     * @param z_variable Name of name variable3

     */

    ValueArg<std::string> Variable3     ("z", "z_variable", "Name variable3", false, "File path", "file", cmd);

    // ---------------------------------------------------------------------------------------------------------
    // ADDITIONAL FUNCTIONALITIES:


    // ---------------------------------------------------------------------------------------------------------
    // PARSING:

    // Parse the argv array.
    cmd.parse(argc, argv);


    // ---------------------------------------------------------------------------------------------------------
    // SETTINGS:

    MUSE::Project Project;
    Project.folder = projectFolder.getValue();
    Project.setName(Project.folder.substr(Project.folder.find_last_of("/")+1, Project.folder.length()));

    // 0) Commands
    std::cout << FCYN("###### Execution command ...") << std::endl;
    std::string command;
    std::cout << "Number of command arguments: " << argc << std::endl;

    filesystem::path abspath = argv[3];
    std::cout << "Absolute path: " << abspath << std::endl;

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
    std::cout << command << std::endl;
    std::cout << FCYN("###### ###### ###### ######") << std::endl;
    std::cout << std::endl;


    // 0) Set folder (in/out)
    std::string in_folder = Project.folder + "/in";
    std::string out_folder = Project.folder + "/out";

    std::string app_folder      = out_folder + "/" + app_name;
    std::string metadata_folder = out_folder + "/" + app_data + "/metadata";
    std::string data_folder     = out_folder + "/" + app_data + "/data";

    // Definition of default extension for saving figure
    std::string figext = "jpeg";


    // ---------------------------------------------------------------------------------------------------------
    // STARTS:

    if(histogramPlot.isSet())
    {
        //std::string var = Variable1.getValue(); //nome variabile

        // Output folder
        if(!filesystem::exists(app_folder))
            filesystem::create_directory(app_folder);

        std::vector<std::string> id;
//        readTextValues(Project.folder + "/out/" + app_data + "/data/" + name_ID + ".dat", id);

        std::string dir_hist = app_folder + "/hist";
        if(!filesystem::exists(dir_hist))
            filesystem::create_directory(dir_hist);

        // Storing json information into class Data
        MUSE::Data data;
        readTextValues(setValues.getValue(), data.text_values);

        data.name = setValues.getValue().substr(setValues.getValue().find_last_of("/")+1, setValues.getValue().length());
        data.name = get_basename(data.name);



        PlotStruct dataplot;

        size_t n_sample = data.text_values.size();

        // String to double Conversion
        for(size_t i =0 ; i<n_sample; i++)
        {
            //vedo che flag è e faccio il check per entrambi i vettori x e y
            double val = 0.0;
            std::string val_tmp = data.text_values.at(i);

            if(!val_tmp.empty() && val_tmp.compare("nd") !=0)
            {
                if(val_tmp.compare("*")!=0)
                {
                    val = std::stod(val_tmp);
                    dataplot.x.push_back(val);

                    if(id.size() > 0)
                        dataplot.ID.push_back(id.at(i));
                }
//                flagActivation(table, data.getFlag());
//                int n_activeFlag = count_activeFlag(table); //conta il numero di flag attivi
//                for(size_t j=0; j<table.size(); j++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
//                {
//                    if(table.at(j).activeFlag == true)
//                        table.at(j).check = getCheck(table.at(j).charFlag, val_tmp);
//                }
//                int n_passedCheck = count_passedCheck(table);
//                restoreTable(table);

//                if(n_activeFlag == n_passedCheck)
//                {
//                    val = std::stod(val_tmp);
//                    dataplot.x.push_back(val);

//                    dataplot.ID.push_back(id.at(i));
//                }
            }
        }

        int n_conv_samples = dataplot.x.size(); //numero campioni convertiti da stringa a double

        if(n_conv_samples == 0)
            std::cerr << "ERROR: All values are invalid! No statistical data summary." << std::endl;
        else
        {
            std::cout << "Statistical data summary ..." << std::endl;
            summary(dataplot.x);
            if(n_conv_samples >= setNMaxValues.getValue()) //numero di punti sufficiente per creare un grafico
            {
                hist_plot(dataplot, Project.name, data.getName() + " [" + data.getUnit() + "]", "Frequency", setNbins.getValue(), setNbins.isSet());

                std::string ext = "jpeg";
                std::string histname = dir_hist + "/" + data.getName();
                matplot::save(histname, ext);

                std::cout << FGRN("Saving histogram as ") << histname + "." + ext << std::endl;
                std::cout << std::endl;
            }
            else
                std::cerr << FRED("ERROR: Not created histogram! Number of samples is < ") << setNMaxValues.getValue() << std::endl;
        }
    }


    if(bivariatePlot.isSet())
    {
        //richiama la funzione plot bivariato
        std::string var1 = Variable1.getValue(); //nome variabile1
        std::string var2 = Variable2.getValue(); //nome variabile2

        // Output folder
        Project.folder = projectFolder.getValue();
        Project.name = projectFolder.getValue().substr(projectFolder.getValue().find_last_of("/")+1, projectFolder.getValue().length());


//        // ID variables
//        json project_json;
//        read_json(Project.folder + "/out/" + app_data + "/"+ Project.name + ".json", project_json);

//        std::string name_ID = project_json.at(json::json_pointer("/CSV File/ID"));
//        std::vector<std::string> id;
//        readTextValues(Project.folder + "/out/" +  app_data + "/data/" + name_ID + ".dat", id);


        std::string dir_biv = projectFolder.getValue() + "/out/" + app_name;
        if(!filesystem::exists(dir_biv))
            filesystem::create_directory(dir_biv);

        dir_biv = projectFolder.getValue() + "/out/" + app_name + "/biv";
        filesystem::create_directory(dir_biv);


        std::string json_path1 = projectFolder.getValue() + "/out/" +  app_data + "/metadata/" + var1 + ".json";
        std::string data_path1 = projectFolder.getValue() + "/out/" +  app_data + "/data/" + var1 + ".dat";

        std::string json_path2 = projectFolder.getValue() + "/out/" +  app_data + "/metadata/" + var2 + ".json";
        std::string data_path2 = projectFolder.getValue() + "/out/" +  app_data + "/data/" + var2 + ".dat";


        //apro i file metadata/data
//        json metadata1, metadata2;
//        read_json(json_path1, metadata1);
//        read_json(json_path2, metadata2);

        MUSE::Data metadata1, metadata2;
        metadata1.read(json_path1);
        metadata2.read(json_path2);

        // Storing json information into class Data
        Data data1, data2;

        //setData(data1, metadata1);
        data1.setType(data1.flag);
        //setType(data1);

        //setData(data2, metadata2);
        data2.setType(data2.flag);
        //setType(data2);

        std::vector<std::string> textValues1, textValues2;
        readTextValues(data_path1, textValues1);
        readTextValues(data_path2, textValues2);


        data1.setTextValues(textValues1);
        data2.setTextValues(textValues2);

        size_t n_sample = data1.text_values.size();

        PlotStruct dataplot;

        // Active flag table
        std::vector<Flag> table;
        flagsTable(table);

        if(data1.type == NUMBER && data2.type == NUMBER) //se entrambe le variabili sono numeriche
        {
            for(size_t i =0 ; i<n_sample; i++)
            {
                //vedo che flag è e faccio il check per entrambi i vettori x e y
                //std::string id_val = id.text_values.at(i); //trattare gli id come stringa (maniera più generale); tanto non leggo manco il flag del'id
                double val1 = 0.0;
                double val2 = 0.0;

                std::string val_tmp1 = data1.text_values.at(i);
                std::string val_tmp2 = data2.text_values.at(i);

                if(!val_tmp1.empty() && val_tmp1.compare("nd") !=0 && !val_tmp2.empty() && val_tmp2.compare("nd") !=0)
                {
                    //se sono diversi da nd, controllo i flag e converto in double i valori che passano i check

                    // For variable 1
                    flagActivation(table, data1.getFlag());
                    int n_activeFlag1 = count_activeFlag(table); //conta il numero di flag attivi
                    for(size_t j=0; j<table.size(); j++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
                    {
                        if(table.at(j).activeFlag == true)
                            table.at(j).check = getCheck(table.at(j).charFlag, val_tmp1);
                    }
                    int n_passedCheck1 = count_passedCheck(table);
                    restoreTable(table);

                    // For variable 2
                    flagActivation(table, data2.getFlag());
                    int n_activeFlag2 = count_activeFlag(table); //conta il numero di flag attivi
                    for(size_t j=0; j<table.size(); j++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
                    {
                        if(table.at(j).activeFlag == true)
                            table.at(j).check = getCheck(table.at(j).charFlag, val_tmp2);
                    }
                    int n_passedCheck2 = count_passedCheck(table);
                    restoreTable(table);

                    if(n_activeFlag1 == n_passedCheck1 && n_activeFlag2 == n_passedCheck2)
                    {
                        val1 = std::stod(val_tmp1);
                        dataplot.x.push_back(val1);

                        val2 = std::stod(val_tmp2);
                        dataplot.y.push_back(val2);

                        //dataplot.ID.push_back(id.at(i));
                    }
                }
            }

            std::string xlab = data1.getName() + " [" + data1.getUnit() + "]";
            std::string ylab = data2.getName() + " [" + data2.getUnit() + "]";
            biv_plot(dataplot, Project.name, xlab, ylab);

            matplot::save(dir_biv + "/" + data1.getName()+"-"+data2.getName(), "jpeg");
            std::cout << std::endl;
        }
        else
            std::cout << "\033[0;31mERROR: Data type variables are not NUMERICAL. Bivplot is not created!\033[0m" << std::endl;
    }



    //se volessi rappresentare la variabile errore -> da migliorare!!!!!
    // se la variabile è un errore (flag E) allora cerca nei parents il nome della variabile di riferimento
    if(errorPlot.isSet())
    {
        if(!Variable1.isSet() || !Variable2.isSet())
        {
            std::cout << FRED("ERROR! At least two variables must be declared as input.") << std::endl;
            exit(1);
        }

        if(!filesystem::exists(app_folder))
            filesystem::create_directory(app_folder);

        std::string json_path1 = metadata_folder + "/" + Variable1.getValue() + ".json";
        std::string data_path1 = data_folder + "/" + Variable1.getValue() + ".dat";

        std::string json_path2 = metadata_folder + "/" + Variable2.getValue() + ".json";
        std::string data_path2 = data_folder + "/" + Variable2.getValue() + ".dat";


        // Storing json information into class Data
        MUSE::Data data1, data2;
        data1.read(json_path1);
        data2.read(json_path2);

        data1.setType(data1.flag);
        data2.setType(data2.flag);

        readTextValues(data_path1, data1.text_values);
        readTextValues(data_path2, data2.text_values);


        // Active flag table
        std::vector<Flag> table;
        flagsTable(table);

        if (data1.type == NUMBER && data2.type == NUMBER) //la variabile 1 è un errore, quindi devo cercare nella lista dei json la variabile presente nei parents
        {
            std::cout << FRED("ERROR! At least one of two input variables must be of ERROR type.") << std::endl;
            exit(1);
        }
        else
        {

            // ID variables
            MUSE::DataMeta datameta;
            datameta.read(out_folder + "/" + app_data + "/info.json");

            std::vector<std::string> id;
            if(datameta.getInfoData().id_name != "Unknown")
                readTextValues(data_folder + "/" + datameta.getInfoData().id_name + ".dat", id);


            std::string json_path, data_path;
            if (data1.type == ERROR && data2.type == NUMBER)
            {
                if(data1.parents != "")
                {
                    json_path = metadata_folder + "/" + data1.parents + ".json";
                    data_path = data_folder + "/" + data1.parents + ".dat";

                    std::cout << "Variable (along X axis): " << data1.name << std::endl;
                    std::cout << "Variable (along Y axis): " << data2.name << std::endl;
                    std::cout << "Parent declaration (along X axis): " << data1.parents << std::endl;
                }
                else
                {
                    std::cout << FRED("ERROR! Parents field is empty!") << std::endl;
                    exit(1);
                }
            }
            else if (data1.type == NUMBER && data2.type == ERROR)
            {
                if(data2.parents != "")
                {
                    json_path = metadata_folder + "/" + data2.parents + ".json";
                    data_path = data_folder + "/" + data2.parents + ".dat";

                    std::cout << "Variable (along X axis): " << data1.name << std::endl;
                    std::cout << "Variable (along Y axis): " << data2.name << std::endl;
                    std::cout << "Parent declaration: (along Y axis): " << data2.parents << std::endl;
                }
                else
                {
                    std::cout << FRED("ERROR! Parents field is empty!") << std::endl;
                    exit(1);
                }
            }


            MUSE::Data data_parent;
            data_parent.read(json_path);
            data_parent.setType(data_parent.flag);
            readTextValues(data_path, data_parent.text_values);
            std::cout << FGRN("Extracting parent variable ... COMPLETED.") << std::endl;
            std::cout << std::endl;

            size_t n_sample = data1.text_values.size();

            PlotStruct dataplot;

            // Active flag table
            std::vector<Flag> table;
            flagsTable(table);

            if (data1.type == ERROR && data2.type == NUMBER)
            {
                for(size_t i =0 ; i<n_sample; i++)
                {
                    double err = 0.0; //data1
                    double vx = 0.0; //data_parent
                    double vy = 0.0; //data2

                    std::string err_tmp = data1.text_values.at(i);
                    std::string vx_tmp = data_parent.text_values.at(i);
                    std::string vy_tmp = data2.text_values.at(i);

                    if(!vx_tmp.empty() && vx_tmp.compare("nd") !=0 && !vy_tmp.empty() && vy_tmp.compare("nd") !=0 && !err_tmp.empty() && err_tmp.compare("nd") !=0)
                    {
                        // For variable 1 - ERRORE
                        flagActivation(table, data1.getFlag());
                        int n_activeFlag_err = count_activeFlag(table); //conta il numero di flag attivi
                        for(size_t j=0; j<table.size(); j++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
                        {
                            if(table.at(j).activeFlag == true)
                                table.at(j).check = getCheck(table.at(j).charFlag, err_tmp);
                        }
                        int n_passedCheck_err = count_passedCheck(table);
                        restoreTable(table);

                        // For variable 1
                        flagActivation(table, data_parent.getFlag());
                        int n_activeFlag1 = count_activeFlag(table); //conta il numero di flag attivi
                        for(size_t j=0; j<table.size(); j++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
                        {
                            if(table.at(j).activeFlag == true)
                                table.at(j).check = getCheck(table.at(j).charFlag, vx_tmp);
                        }
                        int n_passedCheck1 = count_passedCheck(table);
                        restoreTable(table);

                        // For variable 2
                        flagActivation(table, data2.getFlag());
                        int n_activeFlag2 = count_activeFlag(table); //conta il numero di flag attivi
                        for(size_t j=0; j<table.size(); j++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
                        {
                            if(table.at(j).activeFlag == true)
                                table.at(j).check = getCheck(table.at(j).charFlag, vy_tmp);
                        }
                        int n_passedCheck2 = count_passedCheck(table);
                        restoreTable(table);

                        if(n_activeFlag1 == n_passedCheck1 && n_activeFlag2 == n_passedCheck2 && n_activeFlag_err == n_passedCheck_err)
                        {
                            err = std::stod(err_tmp);
                            dataplot.err.push_back(err);

                            vx = std::stod(vx_tmp);
                            vy = std::stod(vy_tmp);

                            dataplot.x.push_back(vx);
                            dataplot.y.push_back(vy);

                            if(id.size() > 0)
                                dataplot.ID.push_back(id.at(i));
                        }
                    }
                }
                std::string xlab = data_parent.getName() + " [" + data_parent.getUnit() + "]";
                std::string ylab = data2.getName() + " [" + data2.getUnit() + "]";
                x_err_plot(dataplot, Project.name, xlab, ylab);

                bool saving = matplot::save(app_folder + "/" + data_parent.getName()+"-"+data2.getName(), figext);
                if(saving == true)
                    std::cout << FGRN("Saving plot as ") << app_folder + "/" + data_parent.getName() +"-"+ data2.getName() + figext << std::endl;
                std::cout << std::endl;
            }

            else if(data1.type == NUMBER && data2.type == ERROR)
            {
                for(size_t i =0 ; i<n_sample; i++)
                {
                    double vx = 0.0; //data1
                    double err = 0.0; //data2
                    double vy = 0.0; //data_parent

                    std::string vx_tmp = data1.text_values.at(i);
                    std::string err_tmp = data2.text_values.at(i); //banda di errore parallela all'asse y
                    std::string vy_tmp = data_parent.text_values.at(i);

                    if(!vx_tmp.empty() && vx_tmp.compare("nd") !=0 && !vy_tmp.empty() && vy_tmp.compare("nd") !=0 && !err_tmp.empty() && err_tmp.compare("nd") !=0)
                    {

                        // For variable 1
                        flagActivation(table, data1.getFlag());
                        int n_activeFlag_err = count_activeFlag(table); //conta il numero di flag attivi
                        for(size_t j=0; j<table.size(); j++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
                        {
                            if(table.at(j).activeFlag == true)
                                table.at(j).check = getCheck(table.at(j).charFlag, vx_tmp);
                        }
                        int n_passedCheck_err = count_passedCheck(table);
                        restoreTable(table);

                        // For variable 2 -ERROR
                        flagActivation(table, data2.getFlag());
                        int n_activeFlag2 = count_activeFlag(table); //conta il numero di flag attivi
                        for(size_t j=0; j<table.size(); j++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
                        {
                            if(table.at(j).activeFlag == true)
                                table.at(j).check = getCheck(table.at(j).charFlag, err_tmp);
                        }
                        int n_passedCheck2 = count_passedCheck(table);
                        restoreTable(table);

                        // For variable 2
                        flagActivation(table, data_parent.getFlag());
                        int n_activeFlag1 = count_activeFlag(table); //conta il numero di flag attivi
                        for(size_t j=0; j<table.size(); j++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
                        {
                            if(table.at(j).activeFlag == true)
                                table.at(j).check = getCheck(table.at(j).charFlag, vy_tmp);
                        }
                        int n_passedCheck1 = count_passedCheck(table);
                        restoreTable(table);

                        if(n_activeFlag1 == n_passedCheck1 && n_activeFlag2 == n_passedCheck2 && n_activeFlag_err == n_passedCheck_err)
                        {
                            err = std::stod(err_tmp);
                            dataplot.err.push_back(err);

                            vx = std::stod(vx_tmp);
                            vy = std::stod(vy_tmp);

                            dataplot.x.push_back(vx);
                            dataplot.y.push_back(vy);

                            if(id.size() > 0)
                                dataplot.ID.push_back(id.at(i));
                        }
                    }
                }

                std::string xlab = data1.getName() + " [" + data1.getUnit() + "]";
                std::string ylab = data_parent.getName() + " [" + data_parent.getUnit() + "]";
                y_err_plot(dataplot, Project.name, xlab, ylab);

                bool saving = matplot::save(app_folder + "/" + data1.getName() +"-"+ data_parent.getName(), figext);
                if(saving == true)
                    std::cout << FGRN("Saving plot as ") << app_folder + "/" + data1.getName() +"-"+ data_parent.getName() + figext << std::endl;
                std::cout << std::endl;
            }
        }
    }




    if(triangularPlot.isSet())
    {
        //richiama la funzione plot bivariato
        std::string var1 = Variable1.getValue(); //nome variabile1
        std::string var2 = Variable2.getValue(); //nome variabile2
        std::string var3 = Variable3.getValue(); //nome variabile2

        // Output folder
        Project.folder = projectFolder.getValue();
        Project.name = projectFolder.getValue().substr(projectFolder.getValue().find_last_of("/")+1, projectFolder.getValue().length());


//        // ID variables
//        json project_json;
//        read_json(Project.folder + "/out/" + app_data + "/"+ Project.name + ".json", project_json);

//        std::string name_ID = project_json.at(json::json_pointer("/CSV File/ID"));
//        std::vector<std::string> id;
//        readTextValues(Project.folder + "/out/" +  app_data + "/data/" + name_ID + ".dat", id);


        std::string dir_tri = projectFolder.getValue() + "/out/" + app_name;
        if(!filesystem::exists(dir_tri))
            filesystem::create_directory(dir_tri);

        dir_tri = projectFolder.getValue() + "/out/" + app_name + "/tri";
        filesystem::create_directory(dir_tri);


        std::string json_path1 = projectFolder.getValue() + "/out/" +  app_data + "/metadata/" + var1 + ".json";
        std::string data_path1 = projectFolder.getValue() + "/out/" +  app_data + "/data/" + var1 + ".dat";

        std::string json_path2 = projectFolder.getValue() + "/out/" +  app_data + "/metadata/" + var2 + ".json";
        std::string data_path2 = projectFolder.getValue() + "/out/" +  app_data + "/data/" + var2 + ".dat";

        std::string json_path3 = projectFolder.getValue() + "/out/" +  app_data + "/metadata/" + var3 + ".json";
        std::string data_path3 = projectFolder.getValue() + "/out/" +  app_data + "/data/" + var3 + ".dat";

        //apro i file metadata/data
//        json metadata1, metadata2, metadata3;
//        read_json(json_path1, metadata1);
//        read_json(json_path2, metadata2);
//        read_json(json_path3, metadata3);

        MUSE::Data metadata1, metadata2, metadata3;
        metadata1.read(json_path1);
        metadata2.read(json_path2);
        metadata3.read(json_path3);

        // Storing json information into class Data
        Data data1, data2, data3;
        //setData(data1, metadata1);
        data1.setType(data1.flag);
        //setType(data1);

        //setData(data2, metadata2);
        data2.setType(data2.flag);
        //setType(data2);

        //setData(data3, metadata3);
        data3.setType(data3.flag);
        //setType(data3);

        std::vector<std::string> textValues1, textValues2, textValues3;
        readTextValues(data_path1, textValues1);
        readTextValues(data_path2, textValues2);
        readTextValues(data_path3, textValues3);

        data1.setTextValues(textValues1);
        data2.setTextValues(textValues2);
        data3.setTextValues(textValues3);

        size_t n_sample = data1.text_values.size();

        PlotStruct dataplot;

        // Active flag table
        std::vector<Flag> table;
        flagsTable(table);

        if(data1.type == NUMBER && data2.type == NUMBER && data3.type == NUMBER) //se entrambe le variabili sono numeriche
        {
            for(size_t i =0 ; i<n_sample; i++)
            {
                //vedo che flag è e faccio il check per entrambi i vettori x e y
                //std::string id_val = id.text_values.at(i);
                double val1 = 0.0;
                double val2 = 0.0;
                double val3 = 0.0;

                std::string val_tmp1 = data1.text_values.at(i);
                std::string val_tmp2 = data2.text_values.at(i);
                std::string val_tmp3 = data3.text_values.at(i);

                if(!val_tmp1.empty() && val_tmp1.compare("nd") !=0 && !val_tmp2.empty() && val_tmp2.compare("nd") !=0 && !val_tmp3.empty() && val_tmp3.compare("nd") !=0)
                {

                    // For variable 1
                    flagActivation(table, data1.getFlag());
                    int n_activeFlag1 = count_activeFlag(table); //conta il numero di flag attivi
                    for(size_t j=0; j<table.size(); j++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
                    {
                        if(table.at(j).activeFlag == true)
                            table.at(j).check = getCheck(table.at(j).charFlag, val_tmp1);
                    }
                    int n_passedCheck1 = count_passedCheck(table);
                    restoreTable(table);

                    // For variable 2
                    flagActivation(table, data2.getFlag());
                    int n_activeFlag2 = count_activeFlag(table); //conta il numero di flag attivi
                    for(size_t j=0; j<table.size(); j++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
                    {
                        if(table.at(j).activeFlag == true)
                            table.at(j).check = getCheck(table.at(j).charFlag, val_tmp2);
                    }
                    int n_passedCheck2 = count_passedCheck(table);
                    restoreTable(table);

                    // For variable 3
                    flagActivation(table, data3.getFlag());
                    int n_activeFlag3 = count_activeFlag(table); //conta il numero di flag attivi
                    for(size_t j=0; j<table.size(); j++) //ho una tabella aggiornata con i flag attivi (relativi alla variabile)
                    {
                        if(table.at(j).activeFlag == true)
                            table.at(j).check = getCheck(table.at(j).charFlag, val_tmp3);
                    }
                    int n_passedCheck3 = count_passedCheck(table);
                    restoreTable(table);

                    if(n_activeFlag1 == n_passedCheck1 && n_activeFlag2 == n_passedCheck2 && n_activeFlag3 == n_passedCheck3)
                    {
                        val1 = std::stod(val_tmp1);
                        dataplot.x.push_back(val1);

                        val2 = std::stod(val_tmp2);
                        dataplot.y.push_back(val2);

                        val3 = std::stod(val_tmp3);
                        dataplot.z.push_back(val3);

                        //dataplot.ID.push_back(id.at(i));
                    }
                }
            }

            std::string xlab = data1.getName() + " [" + data1.getUnit() + "]";
            std::string ylab = data2.getName() + " [" + data2.getUnit() + "]";
            std::string zlab = data3.getName() + " [" + data3.getUnit() + "]";

            tri_plot(dataplot, Project.name, xlab, ylab, zlab);
            matplot::save(dir_tri + "/" + data1.getName()+"-"+data2.getName()+"-"+data3.getName(), "jpeg");
            std::cout << std::endl;
        }
    }


    } catch (ArgException &e)  // catch exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

}
