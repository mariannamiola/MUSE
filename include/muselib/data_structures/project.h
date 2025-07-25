#ifndef PROJECT_H
#define PROJECT_H

#include <string>

#include <cereal/archives/json.hpp>

namespace MUSE
{
    class Project;
}

class MUSE::Project
{
    public:

        std::string folder;
        std::string name;
        std::string authority = "Unknown";


        // Get Methods
        const std::string getFolder      ()  const { return folder; }
        const std::string getName        ()  const { return name; }
        const std::string getAuthority   ()  const { return authority; }


        // Set Methods
        void setFolder      (const std::string s)  { folder = s; }
        void setName        (const std::string s)  { name = s; }
        void setAuthority   (const std::string s)  { authority = s; }


        // Additional Methods
        bool read  (const std::string filename);
        bool write (const std::string filename);


    #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            //ar (CEREAL_NVP(folder));
            ar (CEREAL_NVP(name));
            ar (CEREAL_NVP(authority));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            //ar (CEREAL_NVP(folder));
            ar (CEREAL_NVP(name));
            ar (CEREAL_NVP(authority));
        }
    #endif

private:

    bool readConfFileJSON   (const std::string filename);
    bool writeConfFileJSON  (const std::string filename);
};

#ifndef STATIC_MUSELIB
#include "project.cpp"
#endif

#endif // PROJECT_H
