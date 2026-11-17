#include "save_obj.h"

#include <charconv>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace MUSE
{
// Keep the library's mesh export (faces, materials and other records), then
// format OBJ coordinates with the shortest lossless decimal representation.
template<class Mesh>
void save_obj(const Mesh& mesh, const std::string& filename)
{
    const auto extension = std::filesystem::path(filename).extension().string();
    if(extension!=".obj" && extension!=".OBJ")
        throw std::invalid_argument("save_obj requires an OBJ filename: " + filename);
    mesh.save(filename.c_str());

    std::ifstream input(filename);
    if(!input) throw std::runtime_error("Could not read exported OBJ: " + filename);
    std::ostringstream formatted;
    std::string line;
    while(std::getline(input,line))
    {
        std::istringstream record(line);
        std::string kind;
        record >> kind;
        if(kind!="v" && kind!="vt" && kind!="vn")
        {
            formatted << line << '\n';
            continue;
        }
        formatted << kind;
        std::string token;
        while(record >> token)
        {
            if(token.front()=='#')
            {
                formatted << ' ' << token << record.rdbuf();
                break;
            }
            double value;
            const auto parsed = std::from_chars(token.data(),token.data()+token.size(),value);
            if(parsed.ec!=std::errc() || parsed.ptr!=token.data()+token.size())
                throw std::runtime_error("Invalid exported OBJ coordinate: " + token);
            char buffer[64];
            const auto result = std::to_chars(buffer,buffer+sizeof(buffer),value);
            if(result.ec!=std::errc()) throw std::runtime_error("Could not format OBJ coordinate");
            formatted << ' ';
            formatted.write(buffer,result.ptr-buffer);
        }
        formatted << '\n';
    }
    if(input.bad()) throw std::runtime_error("Could not read exported OBJ: " + filename);
    input.close();
    // Keep the successfully exported file if formatting or writing fails.
    const std::string temporary = filename + ".format.tmp";
    try
    {
        std::ofstream output(temporary,std::ios::trunc);
        output << formatted.str();
        output.close();
        if(!output) throw std::runtime_error("Could not write formatted OBJ: " + filename);
        std::filesystem::rename(temporary,filename);
    }
    catch(...)
    {
        std::error_code ignored;
        std::filesystem::remove(temporary,ignored);
        throw;
    }
}
}

