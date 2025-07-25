MUSE = Modelling of uncertainty as a Support of Environment, version 0.0 - Import tool
Name app: input_data


USAGE: 

input  [-CDFGhNRU] [--version] [-d <del>] [-g <EPSG>] [-n <file>] [-p
          <path>] [-r <row>] [-x <int>] [-y <int>] [-z <int>]
          [n_rows_header]


Where: 

   -N,  --new_project
     Creation new project

   -n <file>,  --name <file>
     Name new project

   -p <path>,  --pdir <path>
     Project directory

   -G,  --set_epsg
     Set EPSG

   -g <EPSG>,  --coordinate <EPSG>
     Set coordinate

   -U,  --coord
     Set n. column coordinate

   -x <int>,  --x_coord <int>
     Set coordinate x

   -y <int>,  --y_coord <int>
     Set coordinate y

   -z <int>,  --z_coord <int>
     Set coordinate z

   -C,  --converter
     Converter data (csv format) into MUSE format

   -F,  --set_flag
     Set flag row

   -r <row>,  --row <row>
     Set row 

   -D,  --set_delimiter
     Set delimiter

   -d <del>,  --delimiter <del>
     Set delimiter

   -R,  --read
     Reading MUSE format

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.

   n_rows_header <int>
     Number of header rows




EXAMPLE: 

0. Creation of new input project, defining work folder and settings
   -> setting EPSG (optional)
   -N -p /Users/mariannamiola/Desktop/work/EPA (-G --epsg 1234)


1. Updating of json project -> setting coordinate columns and csv delimiter
   --setIDXY -p /Users/mariannamiola/Desktop/work/EPA --id 1 -X 4 -Y 5 --setDel COMMA
   --setIDXYZ -p /Users/mariannamiola/Desktop/work/EPA --id 1 -X 4 -Y 5 -Z 6 --setDel COMMA


1. Converter csv file into MUSE format; setting csv delimiter
   input  [-C] [-f <filename>] [n_rows_header = 6]
   -C -p /Users/mariannamiola/Desktop/work/EPA --setDel COMMA


2. Reading MUSE format file and storing into data structure:
   input  [-R] [-j <json_filename>]
   -R -p /Users/mariannamiola/Desktop/work/EPA
