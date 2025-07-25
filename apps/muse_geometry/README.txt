MUSE = Modelling of uncertainty as a Support of Environment, version 0.0 - Geometry tool
Name app: geometry

USAGE: 

   muse_geometry  [-EGhMNPRSTUVZ] [--abs] [--concave] [--convex] [--delta]
                  [--obj] [--savejson] [--tri] [--version] [--vtk] [-m
                  <string> ...] [-p <path>] [-z <int>] [--boundary
                  <string>] [--dim <3D|2D>] [--epsg <EPSG>] [--newres
                  <string>] [--opt <flag>] [--plane <string>] [--points
                  <string>] [--polygon <string>] [--resx <double>] [--resy
                  <double>] [--resz <double>] [--rotangle <double>]
                  [--rotaxis <rot_axis>] [--rotcx <double>] [--rotcy
                  <double>] [--rotcz <double>] [--setz <double>]


Where: 

   -N,  --geometry
     Creation of new geometry

   -p <path>,  --pdir <path>
     Project directory

   -E,  --set_epsg
     Set project EPSG

   --epsg <EPSG>
     EPSG

   -V,  --vector
     Load vector file

   -R,  --raster
     Load raster file

   -P,  --point_cloud
     Load point_cloud file

   --points <string>
     Set geometry type - points

   --polygon <string>
     Set geometry type - polygon

   --tri
     Set triangulation

   --opt <flag>
     Set optimization flags

   --setz <double>
     Set const z values for new points

   --rotaxis <rot_axis>
     Set rotation axis

   --rotangle <double>
     Set rotation angle (clockwise)

   --rotcx <double>
     Set rotation center x

   --rotcy <double>
     Set rotation center y

   --rotcz <double>
     Set rotation center z

   --convex
     Set convex hull

   --concave
     Set concave hull

   --boundary <string>
     Set external boundary plane

   -G,  --grid
     Grid points (from cell centers)

   --dim <3D|2D>
     Grid dimension

   --resx <double>
     Set x resolution

   --resy <double>
     Set y resolution

   --resz <double>
     Set z resolution

   --plane <string>
     Set plane

   -T,  --triobject
     Load trimesh file and create triobject

   -m <string>,  --mesh <string>  (accepted multiple times)
     mesh file

   --delta
     Extrusion deltaz_shape file

   --abs
     Extrusion absz_shape file

   -z <int>,  --z_offset <int>
     Set z offset

   -M,  --tetobject
     Load trimesh and create tetobject

   --vtk
     Saving tet in vtk conversion

   --obj
     Saving trimesh in obj format

   -S,  --trimesh
     Load trimesh file

   --newres <string>
     Set new resolution

   --savejson
     Save json file after trimesh loading

   -Z,  --tetmesh
     Load tetmesh file

   -U,  --merge
     Merge two trimesh

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.

   MUSE = Modelling of Uncertainty as a Support of Environment; Geometry
   tool




EXAMPLE: 

0. Creation of new geometry (folder and json)
   ./geometry -G -p /Users/mariannamiola/Desktop/work/EPA


1. Read shape file and triangulation (optional)
   ./geometry -V -p /Users/mariannamiola/Desktop/work/EPA
   ./geometry -V -p /Users/mariannamiola/Desktop/work/EPA --tri --opt <flags_triangle>
   ./geometry -V -p /Users/mariannamiola/Desktop/work/EPA --tri --convex
   ./geometry -V -p /Users/mariannamiola/Desktop/work/EPA --tri --concave 
   ./geometry -V -p /Users/mariannamiola/Desktop/work/EPA --tri --boundary -m <filename>   

   
2. Read xyz file and triangulation (optional)
   ./geometry -P -p /Users/mariannamiola/Desktop/work/EPA
   ./geometry -P -p /Users/mariannamiola/Desktop/work/EPA --tri


3. Read mesh and creation of triObject (with 2 type of extrusion or 2 different meshes)
   ./geometry -T -m /home/marianna/Scrivania/work/EPA/out/geometry/<_>.off --delta -z 100
   ./geometry -T -m /home/marianna/Scrivania/work/EPA/out/geometry/<_>.off --abs -z 100
   ./geometry -T -m /home/marianna/Scrivania/work/EPA/out/geometry/<_1>.off -m /home/marianna/Scrivania/work/EPA/out/geometry/<_2>.off


4. Read triobject (closed_mesh.off) and creation of tetObject
   ./geometry -M -m /home/marianna/Scrivania/work/EPA/out/geometry/<_>.off
   ./geometry -M -m /home/marianna/Scrivania/work/EPA/out/geometry/<_>.off --opt <flags_tetgen>
   ./geometry -M -m /home/marianna/Scrivania/work/EPA/out/geometry/<_>.off --vtk
   



