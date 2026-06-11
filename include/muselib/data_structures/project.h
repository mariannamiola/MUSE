#ifndef PROJECT_H
#define PROJECT_H

#include <string>

#include <cereal/archives/json.hpp>

namespace MUSE
{
    class Project;
    class ProjectMeta;
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


class MUSE::ProjectMeta
{
public:
    struct Env
    {
        //std::string version;
        std::string created;
        std::string os;
        std::string arch;
        std::string compiler;



        // // Get Methods
        // const std::string getVersion        ()  const { return version; }
        // const std::string getCreated        ()  const { return created; }
        // const std::string getAuthor         ()  const { return author; }
        // const std::string getAffiliation    ()  const { return affiliation; }


        // // Set Methods
        // void setFolder      (const std::string s)  { version = s; }
        // void setName        (const std::string s)  { created = s; }
        // void setAuthority   (const std::string s)  { author = s; }
        // void setAffiliation (const std::string s)  { affiliation = s; }


        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            //ar (CEREAL_NVP(version));
            ar (CEREAL_NVP(created));
            ar (CEREAL_NVP(os));
            ar (CEREAL_NVP(arch));
            ar (CEREAL_NVP(compiler));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            //ar (CEREAL_NVP(version));
            ar (CEREAL_NVP(created));
            ar (CEREAL_NVP(os));
            ar (CEREAL_NVP(arch));
            ar (CEREAL_NVP(compiler));
        }
        #endif
    };

    // Get Methods
    const MUSE::Project     &getProject     () const    { return  project; }
    const Env               &getEnvironment () const    { return  environment; }

    // Set Methods
    void setProject         (const MUSE::Project &d)    { project = d; }
    void setEnv             (const Env &d) { environment = d; }

    // Additional Methods
    bool read  (const std::string filename);
    bool write (const std::string filename);



#ifdef MUSE_USES_CEREAL
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar (CEREAL_NVP(project));
        ar (CEREAL_NVP(environment));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP(project));
        ar (CEREAL_NVP(environment));
    }
#endif

private:

    MUSE::Project project;
    Env environment;


    bool readConfFileJSON   (const std::string filename);
    bool writeConfFileJSON  (const std::string filename, const int &precision = 4);
};

#ifndef STATIC_MUSELIB
#include "project.cpp"
#endif

#endif // PROJECT_H
