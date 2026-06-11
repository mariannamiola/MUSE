// MUSE = Modelling of Uncertainty as a Support of Environment
// Export tool
// This tool exports the results of the MUSE simulation to a CSV or GeoPackage file.
// The tool can export the results of a single simulation or multiple simulations.
// The tool can also append the results to an existing CSV or GeoPackage file.
// The tool can export the results of a surface or volumetric mesh.
// The tool requires the project directory, the variable name, the geometry model,
// the output file and the output format.
// Example of command line:
// ./muse_export -p /Users/pitta1981/Develop/muse/examples/13_RISL_SUpoly_mf -m su_liguria.obj --dir DIR --dim 2D -v forecast -N 10 --mf 120320181400 --append_gpkg su_liguria.gpkg  --geopkg -o ../../../export.gpkg

// LICENSE STATEMENT
// This file is part of MUSE.

// DISCLAIMER
// This software is provided "as is" and any expressed or implied warranties, including,
// but not limited to, the implied warranties of merchantability and fitness for a particular
// purpose are disclaimed. In no event shall the authors and the MUSE members be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services; loss of use,
// data, or profits; or business interruption) however caused and on any theory of liability,
// whether in contract, strict liability, or tort (including negligence or otherwise) arising
// in any way out of the use of this software, even if advised of the possibility of such damage.

#include <iostream>
#include <variant>
#include <string>
#include <vector>

#include <tclap/CmdLine.h>

#include <cinolib/profiler.h>
#include <cinolib/meshes/abstract_mesh.h>
#include "muselib/geometry/surface_mesh.h"
#include "muselib/geometry/volume_mesh.h"
#include "muselib/data_structures/project.h"
#include "muselib/utils.h"
#include "utils/geom_utils.h"

#include <ogrsf_frmts.h>

using namespace std;

using namespace MUSE;
using namespace TCLAP;

// Definizione di un tipo che può contenere i vari tipi di dati GPKG
using FieldValue = std::variant<int32_t, int64_t, double, std::string, std::nullptr_t>;

// Struttura per rappresentare una colonna con il suo tipo
struct ColumnInfo
{
    std::string name;
    OGRFieldType type;
    std::vector<FieldValue> values;
};

// Funzione di conversione per ottenere un valore double da un FieldValue
double getDoubleValue(const FieldValue &value)
{
    if (std::holds_alternative<int32_t>(value))
    {
        return static_cast<double>(std::get<int32_t>(value));
    }
    else if (std::holds_alternative<int64_t>(value))
    {
        return static_cast<double>(std::get<int64_t>(value));
    }
    else if (std::holds_alternative<double>(value))
    {
        return std::get<double>(value);
    }
    else if (std::holds_alternative<std::string>(value))
    {
        try
        {
            return std::stod(std::get<std::string>(value));
        }
        catch (...)
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }
    return std::numeric_limits<double>::quiet_NaN();
}

int main(int argc, char **argv)
{
    // Print start message
    std::cout << std::endl;
    std::cout << "########### STARTING MUSE-EXPORT ..." << std::endl;
    std::cout << std::endl;

    // Define application names
    std::string app_name = "export";           // app name
    std::string app_vario = "vario";           // app vario name
    std::string app_data = "data";             // app data name
    std::string app_manipulate = "manipulate"; // app manipulate name

    try
    {
        // Define command line parser and arguments
        CmdLine cmd("MUSE = Modelling of Uncertainty as a Support of Environment; Export tool", ' ', "version 0.5");

        // Define main functionalities options
        /**
         * @brief Project directory
         * @param pdir Path to project directory
         * @note MANDATORY for all export operations
         * The project directory must contain MUSE data files
         */

        ValueArg<std::string> projectFolder("p", "pdir", "Project directory", true, "Directory", "path", cmd);
        /**
         * @brief Variable
         * @param var Name of variable
         * @note MANDATORY for all export operations
         * The variable must exist in the project data
         */

        ValueArg<std::string> variable("v", "var", "Variable", true, "name_var", "string", cmd);
        /**
         * @brief Geometry model
         * @param geom geometry model
         * @note Often required depending on export type
         * The geometry model must be available in the project
         */

        ValueArg<std::string> geomModel("m", "geom", "Geometry model", false, "name_geometry", "string", cmd);
        /**

         * @brief Output directory

         * @param out Path to output directory

         */

        ValueArg<std::string> output("o", "out", "Output directory", false, "output_directory", "path", cmd);

        /**


         * @brief Type of analysis


         * @param type type of analysis


         */


        ValueArg<std::string> analysis("", "type", "Type of analysis", false, "type", "string", cmd);

        /**


         * @brief multiframe name


         * @param mf multiframe name


         */


        ValueArg<std::string> multiframe("", "mf", "multiframe name", false, "multiframe", "string", cmd);
        // Define variogram direction options
        std::vector<std::string> allowedVarioDir = {"OMNI", "DIR"};
        ValuesConstraint<std::string> allowedValsVD(allowedVarioDir);
        /**
         * @brief type of variogram direction
         * @param dir Path to type of variogram direction
         * @note Used with variogram export operations
         * Works together with --dim for variogram configuration
         * Available options: OMNI, DIR
         * @example --dir DIR --dim 3D
         */

        ValueArg<std::string> varioDirection("", "dir", "type of variogram direction", false, "OMNI", &allowedValsVD, cmd);

        // Define variogram dimension options
        std::vector<std::string> allowedVarioDim = {"3D", "3Dxy", "3Dz", "2D", "1Dz", "1D"};
        ValuesConstraint<std::string> allowedValsVDm(allowedVarioDim);
        /**
         * @brief type of variogram dimension
         * @param dim type of variogram dimension
         * @note Used with variogram export operations
         * Works together with --dir for variogram configuration
         * Available options: 3D, 3Dxy, 3Dz, 2D, 1Dz, 1D
         * @example --dir OMNI --dim 2D
         */

        ValueArg<std::string> varioDimension("", "dim", "type of variogram dimension", false, "3D", &allowedValsVDm, cmd);

        // Define number of simulations option
        /**
         * @brief Number of simulations to export
         * @param nsim Number of number of simulations to export
         * @note Used for exporting multiple simulation results
         * Requires simulation data to be available in project
         */

        ValueArg<int> numSim("N", "nsim", "Number of simulations to export", false, 0, "int", cmd);

        // Define append existing table
        /**

         * @brief Append existing table (csv)

         * @param append_csv_table append existing table (csv)

         */

        ValueArg<std::string> appendTable("", "append_csv_table", "Append existing table (csv)", false, "append_table", "path", cmd);
        /**

         * @brief Separator for append table

         * @param append_sep separator for append table

         */

        ValueArg<std::string> appendSep("", "append_sep", "Separator for append table", false, ";", "string", cmd);
        // Define append existing gpkg table
        /**
         * @brief Append existing table (gpkg)
         * @param append_gpkg append existing table (gpkg)
         * @note Requires existing geopackage file at specified path
         * Use with --geopkg output format
         */

        ValueArg<std::string> appendGpkg("", "append_gpkg", "Append existing table (gpkg)", false, "append_table", "path", cmd);

        // Define export options
        SwitchArg geopkg("", "geopkg", "Export to geopkg", cmd);
        SwitchArg csv("", "csv", "Export to csv", cmd);

        // Parse the argv array
        cmd.parse(argc, argv);

        // Project settings
        MUSE::Project Project;
        Project.folder = projectFolder.getValue();                                                           // project path
        Project.name = Project.folder.substr(Project.folder.find_last_of("/") + 1, Project.folder.length()); // project name

        std::string geom_name = geomModel.getValue().substr(geomModel.getValue().find_last_of("/") + 1, geomModel.getValue().length());
        std::string ext_mesh = get_extension(geom_name);

        // Define the path to the geometry folder in the project directory
        std::string geom_path = Project.getFolder() + "/out/geometry/";
        std::string geom_in_path = Project.getFolder() + "/in/geometry/";

        // Build compute data folder path
        std::string compute_folder;

        if (multiframe.isSet())
            compute_folder = Project.getFolder() + "/out/compute/" + multiframe.getValue() + "/" +
                             variable.getValue() + "_" + varioDirection.getValue() + varioDimension.getValue() + "_" + get_basename(geom_name);
        else
            compute_folder = Project.getFolder() + "/out/compute/" +
                             variable.getValue() + "_" + varioDirection.getValue() + varioDimension.getValue() + "_" + get_basename(geom_name);

        // Build a list of suffixes for compute output filenames
        std::vector<std::string> suffix = {"_mean", "_median", "_Q1", "_Q3", "_QCD" ,"_stdev", "_mean_m_stdev", "_mean_p_stdev"};
        if(analysis.getValue().compare("INDICATOR") == 0)
        {
            suffix.clear();
            std::vector<std::string> suffix_tmp = {"_best"};
            suffix = suffix_tmp;
        }
        else
        {
            compute_folder += "_varspace";
        }

        // Inizializza la struttura dati per le colonne
        std::vector<ColumnInfo> table_data;

        // Aggiungi le colonne base per le coordinate X, Y, Z
        ColumnInfo colX = {"X", OFTReal, {}};
        ColumnInfo colY = {"Y", OFTReal, {}};
        ColumnInfo colZ = {"Z", OFTReal, {}};
        table_data.push_back(colX);
        table_data.push_back(colY);
        table_data.push_back(colZ);

        // Read data from compute files
        for (const std::string &s : suffix)
        {
            //std::string compute_file = compute_folder + "/_varspace/_stats/" + variable.getValue() + s + ".csv";
            std::string compute_file = compute_folder + "/_stats/" + variable.getValue() + s + ".csv";
            std::cout << "Working on file:\t" << compute_file << std::endl;
            std::ifstream file(compute_file);
            if (!file)
            {
                std::cerr << "Cannot open file:\t" << compute_file << std::endl;
                continue;
            }

            ColumnInfo col = {s, OFTReal, {}};
            double num;
            while (file >> num)
            {
                col.values.push_back(num);
            }
            file.close();
            table_data.push_back(col);
        }

        // If number of simulations is greater than 0, read simulation data
        if (numSim.getValue() > 0)
        {
            if(analysis.getValue().compare("INDICATOR") == 0)
                compute_folder += "/pdf_";
            else
                compute_folder += "/";

            for (int i = 0; i < numSim.getValue(); i++)
            {
                std::stringstream ss;
                if(analysis.getValue().compare("INDICATOR") == 0)
                    ss << i+1;
                else
                    ss << setw(4) << setfill('0') << i;

                string number_str = ss.str();


                //std::string compute_file = compute_folder + "/_varspace/" + variable.getValue() + "_" + number_str + ".csv";
                std::string compute_file = compute_folder + variable.getValue() + "_" + number_str + ".csv";
                std::cout << "Working on file: " << compute_file << std::endl;
                std::ifstream file(compute_file);
                if (!file)
                {
                    std::cerr << "Cannot open file:\t" + compute_file + "\n";
                    continue;
                }

                ColumnInfo col = {variable.getValue() + "_" + number_str, OFTReal, {}};
                double num;
                while (file >> num)
                {
                    col.values.push_back(num);
                }
                file.close();
                table_data.push_back(col);
            }
        }

        // If append_table is set, read the existing table
        if (appendTable.isSet())
        {
            std::ifstream file(appendTable.getValue());
            if (!file)
            {
                std::cerr << "Cannot open file:\t" << appendTable.getValue() << std::endl;
                return 1;
            }

            std::vector<ColumnInfo> temp_cols;
            std::string line;

            // Read the headers
            if (std::getline(file, line))
            {
                std::stringstream ss(line);
                std::string header;
                while (std::getline(ss, header, appendSep.getValue()[0]))
                {
                    temp_cols.push_back({header, OFTReal, {}});
                }
            }

            // Read the data
            while (std::getline(file, line))
            {
                std::stringstream ss(line);
                std::string value;
                int col_idx = 0;

                while (std::getline(ss, value, appendSep.getValue()[0]) && col_idx < temp_cols.size())
                {
                    try
                    {
                        double val = std::stod(value);
                        temp_cols[col_idx].values.push_back(val);
                    }
                    catch (...)
                    {
                        temp_cols[col_idx].values.push_back(std::string(value));
                    }
                    col_idx++;
                }
            }
            file.close();

            // Aggiungi le colonne lette dal file CSV alla table_data
            for (const auto &col : temp_cols)
            {
                table_data.push_back(col);
            }
        }

        // If append_gpkg is set, read the existing table
        if (appendGpkg.isSet())
        {
            // full path of the append gpkg file
            std::string appendGpkgPath = geom_in_path + appendGpkg.getValue();

            // Register all OGR drivers
            OGRRegisterAll();

            // Open the GeoPackage file
            GDALDataset *poDS = (GDALDataset *)GDALOpenEx(appendGpkgPath.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
            if (poDS == NULL)
            {
                std::cerr << "Cannot open GPKG file: " << appendGpkgPath << std::endl;
                return 1;
            }

            std::cout << "Working on file:\t" << appendGpkgPath << std::endl;

            // Get the layer
            OGRLayer *poLayer = poDS->GetLayer(0); // Get the first layer
            if (poLayer == NULL)
            {
                std::cerr << "Cannot get layer from GPKG file: " << appendGpkg.getValue() << std::endl;
                GDALClose(poDS);
                return 1;
            }

            // Get the layer definition
            OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();

            // Create temporary storage for each field
            int fieldCount = poFDefn->GetFieldCount();
            std::vector<ColumnInfo> gpkg_columns(fieldCount);

            // Initialize columns with field names and types
            for (int i = 0; i < fieldCount; i++)
            {
                OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(i);
                gpkg_columns[i].name = poFieldDefn->GetNameRef();
                gpkg_columns[i].type = poFieldDefn->GetType();
            }

            // Reset reading
            poLayer->ResetReading();

            // Read all features
            OGRFeature *poFeature;
            while ((poFeature = poLayer->GetNextFeature()) != NULL)
            {
                for (int i = 0; i < fieldCount; i++)
                {
                    if (poFeature->IsFieldSet(i))
                    {
                        OGRFieldType fieldType = gpkg_columns[i].type;
                        if (fieldType == OFTInteger)
                        {
                            gpkg_columns[i].values.push_back(poFeature->GetFieldAsInteger(i));
                        }
                        else if (fieldType == OFTInteger64)
                        {
                            gpkg_columns[i].values.push_back(poFeature->GetFieldAsInteger64(i));
                        }
                        else if (fieldType == OFTReal)
                        {
                            gpkg_columns[i].values.push_back(poFeature->GetFieldAsDouble(i));
                        }
                        else if (fieldType == OFTString)
                        {
                            gpkg_columns[i].values.push_back(std::string(poFeature->GetFieldAsString(i)));
                        }
                        else
                        {
                            gpkg_columns[i].values.push_back(std::nullptr_t{});
                        }
                    }
                    else
                    {
                        gpkg_columns[i].values.push_back(std::nullptr_t{});
                    }
                }
                OGRFeature::DestroyFeature(poFeature);
            }

            // Add the GPKG columns to our table_data
            for (const auto &col : gpkg_columns)
            {
                table_data.push_back(col);
            }

            // Close the dataset
            GDALClose(poDS);
        }

        // If output is CSV
        if (csv.isSet())
        {
            if (output.isSet())
            {
                std::ofstream file(output.getValue());
                if (!file)
                {
                    std::cerr << "Impossibile aprire il file\t" + output.getValue() + "\n";
                    return 1;
                }
                std::cout << "Working on file:\t" << output.getValue() << std::endl;
                // Set the precision of the output to 15 decimal places
                file << std::fixed << std::setprecision(15);

                // Distinguish between surface and volumetric meshes based on the extension
                if (ext_mesh.compare(".off") == 0 || ext_mesh.compare(".obj") == 0)
                {
                    std::cout << "Mesh is surface." << std::endl;

                    MUSE::SurfaceMesh<> surf_mesh;
                    geom_path = geom_path + "surf/" + geomModel.getValue();
                    surf_mesh.load(geom_path.c_str());

                    // Riempi i valori delle coordinate X, Y, Z
                    for (uint pid = 0; pid < surf_mesh.num_polys(); pid++)
                    {
                        table_data[0].values.push_back(surf_mesh.poly_centroid(pid).x()); // X
                        table_data[1].values.push_back(surf_mesh.poly_centroid(pid).y()); // Y
                        table_data[2].values.push_back(surf_mesh.poly_centroid(pid).z()); // Z
                    }

                    // Write the headers to the file
                    for (size_t i = 0; i < table_data.size(); i++)
                    {
                        file << table_data[i].name;
                        if (i < table_data.size() - 1)
                        {
                            file << ";";
                        }
                    }
                    file << std::endl;

                    // Write the data rows
                    size_t numRows = surf_mesh.num_polys();
                    for (size_t row = 0; row < numRows; row++)
                    {
                        for (size_t col = 0; col < table_data.size(); col++)
                        {
                            if (row < table_data[col].values.size())
                            {
                                if (std::holds_alternative<double>(table_data[col].values[row]))
                                {
                                    file << std::get<double>(table_data[col].values[row]);
                                }
                                else if (std::holds_alternative<int32_t>(table_data[col].values[row]))
                                {
                                    file << std::get<int32_t>(table_data[col].values[row]);
                                }
                                else if (std::holds_alternative<int64_t>(table_data[col].values[row]))
                                {
                                    file << std::get<int64_t>(table_data[col].values[row]);
                                }
                                else if (std::holds_alternative<std::string>(table_data[col].values[row]))
                                {
                                    file << std::get<std::string>(table_data[col].values[row]);
                                }
                                else
                                {
                                    file << "NaN";
                                }
                            }
                            else
                            {
                                file << "NaN";
                            }

                            if (col < table_data.size() - 1)
                            {
                                file << ";";
                            }
                        }
                        file << std::endl;
                    }
                }
                else
                {
                    std::cout << "Mesh is volumetric." << std::endl;

                    MUSE::VolumeMesh<> vol_mesh;
                    vol_mesh.load(geomModel.getValue().c_str());

                    // Riempi i valori delle coordinate X, Y, Z
                    for (uint pid = 0; pid < vol_mesh.num_polys(); pid++)
                    {
                        table_data[0].values.push_back(vol_mesh.poly_centroid(pid).x()); // X
                        table_data[1].values.push_back(vol_mesh.poly_centroid(pid).y()); // Y
                        table_data[2].values.push_back(vol_mesh.poly_centroid(pid).z()); // Z
                    }

                    // Write the headers to the file
                    for (size_t i = 0; i < table_data.size(); i++)
                    {
                        file << table_data[i].name;
                        if (i < table_data.size() - 1)
                        {
                            file << ";";
                        }
                    }
                    file << std::endl;

                    // Write the data rows
                    size_t numRows = vol_mesh.num_polys();
                    for (size_t row = 0; row < numRows; row++)
                    {
                        for (size_t col = 0; col < table_data.size(); col++)
                        {
                            if (row < table_data[col].values.size())
                            {
                                if (std::holds_alternative<double>(table_data[col].values[row]))
                                {
                                    file << std::get<double>(table_data[col].values[row]);
                                }
                                else if (std::holds_alternative<int32_t>(table_data[col].values[row]))
                                {
                                    file << std::get<int32_t>(table_data[col].values[row]);
                                }
                                else if (std::holds_alternative<int64_t>(table_data[col].values[row]))
                                {
                                    file << std::get<int64_t>(table_data[col].values[row]);
                                }
                                else if (std::holds_alternative<std::string>(table_data[col].values[row]))
                                {
                                    file << std::get<std::string>(table_data[col].values[row]);
                                }
                                else
                                {
                                    file << "NaN";
                                }
                            }
                            else
                            {
                                file << "NaN";
                            }

                            if (col < table_data.size() - 1)
                            {
                                file << ";";
                            }
                        }
                        file << std::endl;
                    }
                }
                file.close();
            }
        }

        // If output is GeoPackage
        if (geopkg.isSet())
        {
            if (output.isSet())
            {
                std::string output_file = output.getValue();
                std::string output_folder = output_file.substr(0, output_file.find_last_of("/"));
                std::string output_name = output_file.substr(output_file.find_last_of("/") + 1, output_file.length());
                std::string output_name_noext = output_name.substr(0, output_name.find_last_of("."));
                std::string output_ext = output_name.substr(output_name.find_last_of("."), output_name.length());

                std::cout << "Working on file:\t" << output_file << std::endl;
                // Distinguish between surface and volumetric meshes based on the extension
                if (ext_mesh.compare(".off") == 0 || ext_mesh.compare(".obj") == 0)
                {
                    std::cout << "Mesh is surface." << std::endl;

                    MUSE::SurfaceMesh<> surf_mesh;
                    geom_path = geom_path + "surf/" + geomModel.getValue();
                    surf_mesh.load(geom_path.c_str());

                    // Riempi i valori delle coordinate X, Y, Z se non già presenti
                    if (table_data[0].values.empty())
                    {
                        for (uint pid = 0; pid < surf_mesh.num_polys(); pid++)
                        {
                            table_data[0].values.push_back(surf_mesh.poly_centroid(pid).x()); // X
                            table_data[1].values.push_back(surf_mesh.poly_centroid(pid).y()); // Y
                            table_data[2].values.push_back(surf_mesh.poly_centroid(pid).z()); // Z
                        }
                    }

                    // Register all OGR drivers
                    OGRRegisterAll();
                    GDALDriver *poDriver;
                    poDriver = GetGDALDriverManager()->GetDriverByName("GPKG");
                    if (poDriver == NULL)
                    {
                        std::cerr << "GPKG driver not available." << std::endl;
                        exit(1);
                    }
                    // Create the output file
                    GDALDataset *poDS;
                    poDS = poDriver->Create(output_file.c_str(), 0, 0, 0, GDT_Unknown, NULL);
                    if (poDS == NULL)
                    {
                        std::cerr << "Creation of output file failed." << std::endl;
                        exit(1);
                    }

                    OGRLayer *poLayer;
                    poLayer = poDS->CreateLayer(output_name_noext.c_str(), NULL, wkbPolygon25D, NULL);
                    if (poLayer == NULL)
                    {
                        std::cerr << "Layer creation failed." << std::endl;
                        exit(1);
                    }

                    // Crea i campi nella tabella GPKG
                    for (const auto &col : table_data)
                    {
                        OGRFieldDefn oField(col.name.c_str(), col.type);
                        if (poLayer->CreateField(&oField) != OGRERR_NONE)
                        {
                            std::cerr << "Creating field " << col.name << " failed." << std::endl;
                            exit(1);
                        }
                    }

                    // Crea le feature con la geometria e i valori degli attributi
                    for (size_t pid = 0; pid < surf_mesh.num_polys(); pid++)
                    {
                        std::vector<cinolib::vec3d> v = surf_mesh.poly_verts(pid);
                        vector<point3d> points;
                        for (size_t i = 0; i < v.size(); i++)
                        {
                            point3d p = point3d({v.at(i).x(), v.at(i).y(), v.at(i).z()});
                            points.push_back(p);
                        }

                        OGRFeature *poFeature = OGRFeature::FromHandle(
                            OGR_F_Create(reinterpret_cast<OGRFeatureDefnH>(poLayer->GetLayerDefn())));

                        OGRPolygon polygon;
                        OGRLinearRing ring;
                        for (const point3d &p : points)
                        {
                            ring.addPoint(p.get(0), p.get(1), p.get(2));
                        }
                        ring.closeRings();
                        polygon.addRing(&ring);

                        poFeature->SetGeometry(&polygon);

                        // Imposta gli attributi
                        for (size_t i = 0; i < table_data.size(); i++)
                        {
                            if (pid < table_data[i].values.size())
                            {
                                if (std::holds_alternative<double>(table_data[i].values[pid]))
                                {
                                    poFeature->SetField(table_data[i].name.c_str(), std::get<double>(table_data[i].values[pid]));
                                }
                                else if (std::holds_alternative<int32_t>(table_data[i].values[pid]))
                                {
                                    poFeature->SetField(table_data[i].name.c_str(), std::get<int32_t>(table_data[i].values[pid]));
                                }
                                else if (std::holds_alternative<int64_t>(table_data[i].values[pid]))
                                {
                                    long long int lli = std::get<int64_t>(table_data[i].values[pid]);
                                    poFeature->SetField(table_data[i].name.c_str(), lli);
                                }
                                else if (std::holds_alternative<std::string>(table_data[i].values[pid]))
                                {
                                    poFeature->SetField(table_data[i].name.c_str(), std::get<std::string>(table_data[i].values[pid]).c_str());
                                }
                            }
                        }

                        if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
                        {
                            std::cerr << "Failed to create feature in gpkg." << std::endl;
                            exit(1);
                        }

                        OGRFeature::DestroyFeature(poFeature);
                    }
                    GDALClose(poDS);
                }
            }
        }
    }
    catch (ArgException &e) // catch exceptions
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        exit(-3);
    }

    return 0;
}
