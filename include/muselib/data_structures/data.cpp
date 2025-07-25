#include "data.h"

#include "muselib/flag/check.h"
#include "muselib/colors.h"

namespace MUSE
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Data::setData (const std::vector<std::string> &header)
{
    this->name = header.at(0);
    this->units = header.at(1);
    this->flag = header.at(2);
    this->parents = header.at(3);
    this->description = header.at(4);
    this->comments = header.at(5);
}

void Data::setType (const std::string &flags)
{
//    if(flags == "")
//        std::cout << FRED("ERROR. Not specified flags!") << std::endl;

    if(flags.find("V") != std::string::npos || flags.find("Y") != std::string::npos || flags.find("T") != std::string::npos)
        this->type = varType::TEXT;

    else if(flags.find("E") != std::string::npos)
        this->type = varType::ERROR;

    else if(flags.find("K") != std::string::npos)
    {
        size_t pos = flags.find("K");
        size_t found = flags.find("V", pos+1);

        if(found != std::string::npos)
            this->type = varType::CATEGORIC_TEXT;
        else
            this->type = varType::CATEGORIC;
    }
    else
        this->type = varType::NUMBER;
}

bool Data::read(const std::string filename)
{
    return readConfFileJSON(filename);
}

bool Data::write(const std::string filename)
{
    return writeConfFileJSON(filename);
}

bool Data::readConfFileJSON(const std::string filename)
{
#ifdef MUSE_USES_CEREAL
    std::ifstream ss (filename.c_str(), std::ifstream::in);

    if (!ss.is_open())
    {
        std::cerr << "[ERROR] Error in opening " << filename << "." << std::endl;
        std::cerr << "[ERROR] Configuration file cannot be read from disk." << std::endl;
        return false;
    }

    cereal::JSONInputArchive archive (ss);
    deserialize(archive);
    ss.close();

    return true;
#else
    std::cerr << "CEREAL Library is required to load an instrument from file." << std::endl;
    return false;
#endif
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


bool Data::writeConfFileJSON (const std::string filename)
{
#ifdef MUSE_USES_CEREAL
    std::ofstream ss (filename.c_str(), std::ofstream::out);

    if (!ss.is_open())
    {
        std::cerr << "[ERROR] Error in opening " << filename << "." << std::endl;
        std::cerr << "[ERROR] Configuration file cannot be written on disk." << std::endl;
        return false;
    }

    {
        cereal::JSONOutputArchive archive (ss);
        serialize(archive);
    }
    ss.close();

    return true;
#else
    std::cerr << "CEREAL Library is required to save an instrument on file." << std::endl;
    return false;
#endif
}

}

//void setData (MUSE::Data &data, json metadata)
//{
//    data.setName(metadata.at(json::json_pointer("/Name")));
//    data.setUnit(metadata.at(json::json_pointer("/Units")));
//    data.setFlag(metadata.at(json::json_pointer("/Flags")));
//    data.setParents(metadata.at(json::json_pointer("/Parents")));
//    data.setDescription(metadata.at(json::json_pointer("/Description")));
//    data.setComments(metadata.at(json::json_pointer("/Comments")));
//}




//void setType (MUSE::Data &data)
//{
//    if(data.flag.find("V") != std::string::npos || data.flag.find("Y") != std::string::npos || data.flag.find("T") != std::string::npos)
//        data.type = varType::TEXT;

//    else if(data.flag.find("E") != std::string::npos)
//        data.type = varType::ERROR;

//    else if(data.flag.find("K") != std::string::npos)
//    {
//        size_t pos = data.flag.find("K");
//        size_t found = data.flag.find("V", pos+1);

//        if(found != std::string::npos)
//            data.type = varType::CATEGORIC_TEXT;
//        else
//            data.type = varType::CATEGORIC;
//    }

//    else
//        data.type = varType::NUMBER;
//}


// For textual data class
void readTextValues (const std::string &filename, std::vector<std::string> &v)
{
    std::ifstream file;
    file.open(filename, std::fstream::in);
    if (!file.is_open())
    {
        std::cerr << "ERROR while reading in file " << filename << std::endl;
        exit(1);
    }

    std::string line;
    while(getline(file, line))
        v.push_back(line);

    file.close();
}


void readCoordinate (const std::string &filename, std::vector<double> &coord)
{
    std::ifstream file;
    file.open(filename, std::fstream::in);
    if (!file.is_open())
    {
        std::cerr << "ERROR while reading in file " << filename << std::endl;
        exit(1);
    }

    double value;
    while (file >> value)
        coord.push_back(value);

    file.close();
}
