MUSE = Modelling of uncertainty as a Support of Environment, version 0.0 - Muse Manipulate tool
Name app: muse_manipulate


USAGE: 
 ............. TO COMPLETE



Where: 
 ............. TO COMPLETE

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.




EXAMPLE: 

0. Points extraction from geometry model (2D/3D)
   ./muse_manipulate -E -p <project_folder> --geom <filename.off> -z <nameZ>
   
1. Compute points projection on surfaces
   ./muse_manipulate -P -p <project_folder> --type GEOMETRY --geom <filename.off>
   ./muse_manipulate -P -p <project_folder> --type GEOMETRY --geom <filename.off> --m <mesh1.off> --m <mesh2.off>
   ./muse_manipulate -P -p <project_folder> --type DEFAULT --m <mesh1.off> --m <mesh2.off> --m <mesh3.off>
   
2. Stratigraphic coordinate system transformation
   ./muse_manipulate -S -p <project_folder> --sstype PROPORTIONAL --top <name_top> --bot <name_bottom> --type GEOMETRY --geom <filename.off>
