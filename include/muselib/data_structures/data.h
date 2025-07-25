#ifndef DATA_H
#define DATA_H

#include <vector>
#include <string>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

enum varType
{
    COORDINATE, //0
    NUMBER,     //1
    ERROR,      //2
    CATEGORIC,
    CATEGORIC_TEXT,
    ID,
    TEXT        //3
};

namespace MUSE
{
    class Data;
}


class MUSE::Data
{
    public:

        // Class members (variable to be used)
        std::string name;
        std::string units;
        std::string flag;
        std::string parents;
        std::string description;
        std::string comments;

        varType    type;

        std::vector<std::string> text_values;


        // Get Methods
        const std::string getName        ()  const { return name; }
        const std::string getUnit        ()  const { return units; }
        const std::string getFlag        ()  const { return flag; }
        const std::string getParents     ()  const { return parents; }
        const std::string getDescription ()  const { return description; }
        const std::string getComments    ()  const { return comments; }


        // Set Methods
        void setName        (const std::string s) { name = s; }
        void setUnit        (const std::string s) { units = s; }
        void setFlag        (const std::string s) { flag = s; }
        void setParents     (const std::string s) { parents = s; }
        void setDescription (const std::string s) { description = s; }
        void setComments    (const std::string s) { comments = s; }

        void setTextValues  (const std::vector<std::string> v) { text_values = v; }

        void setData        (const std::vector<std::string> &header);
        void setType        (const std::string &flags);


        // Additional Methods
        bool read  (const std::string filename);
        bool write (const std::string filename);


#ifdef MUSE_USES_CEREAL
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar (CEREAL_NVP(name));
        ar (CEREAL_NVP(units));
        ar (CEREAL_NVP(flag));
        ar (CEREAL_NVP(parents));
        ar (CEREAL_NVP(description));
        ar (CEREAL_NVP(comments));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP(name));
        ar (CEREAL_NVP(units));
        ar (CEREAL_NVP(flag));
        ar (CEREAL_NVP(parents));
        ar (CEREAL_NVP(description));
        ar (CEREAL_NVP(comments));
    }
#endif

private:

    bool readConfFileJSON   (const std::string filename);
    bool writeConfFileJSON  (const std::string filename) ;

};


//void setData (MUSE::Data &data, json metadata);
//void setType (MUSE::Data &data);

void readTextValues (const std::string &filename, std::vector<std::string> &v);

void readCoordinate (const std::string &filename, std::vector<double> &coord);



#ifndef STATIC_MUSELIB
#include "data.cpp"
#endif

#endif // DATA_H
