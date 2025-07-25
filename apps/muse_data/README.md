# MUSE_Data
A toolkit to load, manage and encode heterogeneous environmental acquisition through a flexible descriptive data format. Functionalities includes CSV reading, consistence checks on variables, data format encoding, and preliminary statistics.


## Authors

- Marianna Miola (DISTAV, Università di Genova, Italy)

## Dependences 


## How to get MUSE_Data


## How to build Apps
MUSE_Data provides a *CMakeLists.txt* located in the root directory *ROOT* (where this README lies).

## How to use

   muse_data  [-ChNRS] [--version] [--inf <inf>] [--nvalhist <int>]
              [--setDel <DEFAULT|COMMA>] [--setEPSG <EPSG:n>] [--setID
              <int>] [--setX <int>] [--setY <int>] [--setZ <int>] [--sup
              <sup>] -p <path>


Where: 

   -N,  --new_project
     Creation new project

   -p <path>,  --pdir <path>
     (required) Project directory

   --setEPSG <EPSG:n>
     Set project EPSG

   -S,  --setIDXYZ
     Set n. column coordinate

   --setID <int>
     Set ID

   --setX <int>
     Set coordinate x

   --setY <int>
     Set coordinate y

   --setZ <int>
     Set coordinate z

   --setDel <DEFAULT|COMMA>
     Set type of csv delimiter

   -C,  --converter
     Converter data (csv format) into MUSE format

   --inf <inf>
     Set inf limit

   --sup <sup>
     Set sup limit

   -R,  --read
     Reading MUSE format

   --nvalhist <int>
     Set minimum number of values for histogram

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.
