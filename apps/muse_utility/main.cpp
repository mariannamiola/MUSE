#include <tclap/CmdLine.h>

#include "muselib/utils.h"

#include "muselib/metadata/data_meta.h"
#include "muselib/metadata/extraction_meta.h"
#include "muselib/metadata/vario_meta.h"
#include "muselib/metadata/compute_meta.h"
#include "muselib/metadata/dependencies.h"

#include "muselib/data_structures/graph.h"

#include <queue>

#ifdef __APPLE__
    using namespace std::__fs;
#else
    using namespace std;
#endif

using namespace TCLAP;

int main(int argc, char** argv)
{
    std::cout << std::endl;
    std::cout << "########### STARTING MUSE-UTILITY ..." << std::endl;
    std::cout << std::endl;

    std::string app_name = "utility";      //app name

    try {
    CmdLine cmd("MUSE = Modelling of Uncertainty as a Support of Environment; Muse Utility tool", ' ', "version 0.0");

    // ---------------------------------------------------------------------------------------------------------
    // MAIN FUNCTIONALITIES:

    // Option 0.
    SwitchArg fileList                  ("L", "list", "File List", cmd, false); //booleano
    ValueArg<std::string> projectFolder ("p", "pdir", "Directory", false, "Directory", "path", cmd);
    ValueArg<std::string> fileType      ("", "type", "Type of file", false, "Directory", "path", cmd);

    // Option 1. TO DO ...
    SwitchArg Query                     ("Q", "query", "Query for creation path", cmd, false); //booleano

    // Option 2. Print processing history following JSONs
    SwitchArg setHistory                ("H", "history", "Set JSON history", cmd, false); //booleano
    ValueArg<std::string> setJSON       ("", "json", "Set json file", false, "path", "string", cmd);
    SwitchArg setBackInfo               ("", "back", "Set JSON history (recursively - back)", cmd, false); //booleano
    SwitchArg setForwardInfo            ("", "forward", "Set JSON history (recursively - forward)", cmd, false); //booleano
    SwitchArg setMoreInfo               ("", "more", "Set JSON history (recursively - forward) and commands", cmd, false); //booleano



    // ---------------------------------------------------------------------------------------------------------
    // ADDITIONAL FUNCTIONALITIES:

    // ---------------------------------------------------------------------------------------------------------
    // PARSING:

    // Parse the argv array.
    cmd.parse(argc, argv);

    // ---------------------------------------------------------------------------------------------------------
    // SETTINGS:

    std::string ext;
    std::string type = fileType.getValue();
    if(type.compare("JSON") == 0)
        ext = ".json";
    else if (type.compare("MESH") == 0)
            ext = ".off";


    // ---------------------------------------------------------------------------------------------------------
    // STARTS:

    if(fileList.isSet())
    {     
        std::vector<std::string> dirs = get_recursive_directories(projectFolder.getValue());

        if(dirs.size() == 0)
        {
            std::cout << "Dir: " << projectFolder.getValue() << std::endl;

            if(filesystem::is_empty(projectFolder.getValue())) //se è vuota
                std::cerr << "\033[0;31mEmpty directory!\033[0m" << std::endl;
            else
            {
                std::vector<std::string> list = get_files(projectFolder.getValue(), ext);
                if(list.size() == 0)
                    std::cout << "\033[0;31mNo "<< type <<" file in this directory!\033[0m" << std::endl;
                else
                    cout_list(list);
            }
        }
        else
        {
            for(size_t i=0; i< dirs.size(); i++)
            {
                std::cout << "Dir: " << dirs.at(i) << std::endl;

                if(filesystem::is_empty(dirs.at(i))) //se è vuota
                    std::cerr << "\033[0;31mEmpty directory!\033[0m" << std::endl;
                else
                {
                    std::vector<std::string> list = get_files(dirs.at(i), ext);
                    if(list.size() == 0)
                        std::cout << "\033[0;31mNo "<< type <<" file in this directory!\033[0m" << std::endl;
                    else
                        cout_list(list);
                }

                std::cout << std::endl;
            }
        }
    }


    if(setHistory.isSet())
    {
        if(get_extension(setJSON.getValue()).compare(".json") != 0)
        {
            std::cout << FRED("ERROR. File format not correct. Only JSON file are supported!") << std::endl;
            exit(1);
        }

        std::cout << "Loading file ... " << setJSON.getValue() << std::endl;
        std::cout << std::endl;

        std::string root_project;
        if(setJSON.getValue().find("out/") != std::string::npos)
            root_project = setJSON.getValue().substr(0, setJSON.getValue().find("out/"));

        std::cout << std::endl;
        std::cout << "### JSON GENERAL FEATURES ..." << std::endl;
        std::cout << "JSON file root: " << root_project << std::endl;
        std::cout << std::endl;

        std::vector<std::string> deps, commands;
        if(setBackInfo.isSet())
        {
            //if(findDeps_from_JSON(setJSON.getValue()))
            deps = get_from_JSON(setJSON.getValue(), "dependencies");
            if(deps.size() == 0)
            {
                std::cout << "NO dependencies are found for file: " << setJSON.getValue() << std::endl;
                exit(1);
            }

            std::queue<std::string> queue;
            for(size_t i=0; i< deps.size(); i++)
                queue.push(deps.at(i));

            while (!queue.empty())
            {
                std::string curr = queue.front();
                std::cout << "---> " << curr << std::endl;
                queue.pop();

                deps.clear();
                if(curr.find(".json") != std::string::npos)
                    deps = get_from_JSON(root_project + curr, "dependencies");

                std::string indent = "\t";
                if(deps.size() > 0)
                {
                    for(size_t i=0; i< deps.size(); i++)
                        std::cout << indent + "---> " << deps.at(i) << std::endl;
                }
                else
                    std::cout << indent + "---> " << "NO dependencies are found." << std::endl;

                std::cout << std::endl;

                for(size_t i=0; i< deps.size(); i++)
                    queue.push(deps.at(i));
            }
        }


        if(setForwardInfo.isSet() && setMoreInfo.isSet())
        {
            std::deque<int> n_deps, level;
            std::deque<std::string> deque, deque_com;
            std::queue<std::string> queue;

            filesystem::path rel_path = filesystem::relative(setJSON.getValue(), root_project);
            std::cout << "JSON file: " << rel_path << std::endl;

            queue.push(rel_path);

            int id_level = 0;
            level.push_back(id_level);


            while (!queue.empty())
            {
                std::string curr = queue.front();
                std::string curr_com;

                //if(filesystem::exists(root_project + curr) )
                if(curr.find(".json") != std::string::npos)
                    curr_com = get_from_JSON(root_project + curr,"commands").at(0);
                    //curr_com = getCom_from_JSON(root_project + curr).at(0);

                queue.pop();
                //queue_com.pop();
                deque.push_back(curr);
                deque_com.push_back(curr_com);

                deps.clear();
                if(curr.find(".json") != std::string::npos)
                    deps = get_from_JSON(root_project + curr, "dependencies");

                n_deps.push_back(deps.size());

                if(deps.size() > 0)
                {
                    id_level++;
                    for(size_t i=0; i< deps.size(); i++)
                    {
                        queue.push(deps.at(i));
                        level.push_back(id_level);
                    }
                }
            }

            int k_last = 1;
            vector<Edge> edges;
            std::cout << "Creating graph ..." << std::endl;
            for(int j=0; j< deque.size(); j++)
            {
                int n_deps_node = n_deps.at(j);

                std::cout << "DEQUE: " << deque.at(j) << "; ";
                std::cout << "LEVEL: " << level.at(j) << "; ";
                std::cout << "DEPS: " << n_deps_node << "; ";
                std::cout << "INDEX_NODE: " << j << std::endl;

                if(n_deps_node != 0)
                {
                    for(int k=k_last; k< n_deps_node + k_last; k++)
                    {
                        int pos = j+k;

                        if(pos >= deque.size())
                            pos = deque.size();

                        Edge edge = {j, pos};
                        edges.push_back(edge);
                    }
                }

                k_last = k_last + n_deps.at(j) - 1;
            }
            std::cout << "Creating graph ... COMPLETED." << std::endl;
            std::cout << std::endl;


            // construct graph
            Graph graph(edges, edges.size()+1);

            // print adjacency list representation of a graph
            printGraph2(graph, edges.size()+1, deque, deque_com);

        }


        if(setForwardInfo.isSet() && !setMoreInfo.isSet())
        {
            std::deque<int> n_deps, level;
            std::deque<std::string> deque;
            std::queue<std::string> queue;

            filesystem::path rel_path = filesystem::relative(setJSON.getValue(), root_project);
            std::cout << "JSON file: " << rel_path << std::endl;

            queue.push(rel_path);

            int id_level = 0;
            level.push_back(id_level);

            while (!queue.empty())
            {
                std::string curr = queue.front();
                //std::cout << "---> " << curr << std::endl;
                queue.pop();
                deque.push_back(curr);

                deps.clear();
                if(curr.find(".json") != std::string::npos)
                    deps = get_from_JSON(root_project + curr, "dependencies");

                n_deps.push_back(deps.size());

                //std::string indent = "\t";
                if(deps.size() > 0)
                {
                    id_level++;
                    for(size_t i=0; i< deps.size(); i++)
                    {
                        queue.push(deps.at(i));
                        level.push_back(id_level);
                    }
                }
            }

            int k_last = 1;
            vector<Edge> edges;
            std::cout << "Creating graph ..." << std::endl;
            for(int j=0; j< deque.size(); j++)
            {
                int n_deps_node = n_deps.at(j);

                std::cout << "DEQUE: " << deque.at(j) << "; ";
                std::cout << "LEVEL: " << level.at(j) << "; ";
                std::cout << "DEPS: " << n_deps_node << "; ";
                std::cout << "INDEX_NODE: " << j << std::endl;

                if(n_deps_node != 0)
                {
                    for(int k=k_last; k< n_deps_node + k_last; k++)
                    {
                        int pos = j+k;

                        if(pos >= deque.size())
                            pos = deque.size();

                        Edge edge = {j, pos};
                        edges.push_back(edge);
                    }
                }

                k_last = k_last + n_deps.at(j) - 1;
            }
            std::cout << "Creating graph ... COMPLETED." << std::endl;
            std::cout << std::endl;

            // construct graph
            Graph graph(edges, edges.size()+1);

            // print adjacency list representation of a graph
            printGraph2(graph, edges.size()+1, deque);
        }

        std::cout << std::endl;
        std::cout << "Printing history of JSON: " << setJSON.getValue() << " COMPLETED." << std::endl;
    }

    } catch (ArgException &e)  // catch exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

}
