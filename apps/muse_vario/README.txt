MUSE = Modelling of uncertainty as a Support of Environment, version 0.0 - Vario tool
Name app: vario


USAGE: 

   muse_vario  [-hV] [--version] [--weight] [-z <Name Z coordinate>]
               [--bandw <double>] [--csize <double>] [--decl <string>]
               [--deg <degree>] [--degtol <degree>] [--dim <3D|2D|1D>]
               [--dir <OMNI|DIR>] [--dirs <string>] [--eps <double>]
               [--expvario <string>] [--lagspac <VARIABLE|CONSTANT>]
               [--maxdist <double>] [--npoints <int>] [--nscore <string>]
               [--nstartpoints <int>] [--nstep <int>] [--nugget <double>]
               [--nugstep <double>] [--pstep <double>] [--range <double>]
               [--rangestep <double>] [--rotangle <double>] [--rotaxis
               <rot_axis>] [--rotcx <double>] [--rotcy <double>] [--rotcz
               <double>] [--sill <double>] [--sttype <PROPORTIONAL|
               TRUNCATION|ONLAP|COMBINATION>] [--sub <path>] [--tolfac
               <double>] [--type <AUTO|SPHERICAL|GAUSSIAN|EXPONENTIAL|
               LINEAR|DEFAULT>] [--vario <EXPERIMENTAL|MODEL>] [--vclean
               <int>] -p <path> -v <name variable>


Where: 

   -V,  --variogram
     Compute variogram

   -p <path>,  --pdir <path>
     (required) Project directory

   -v <name variable>,  --var <name variable>
     (required) Variable
     
   -z <Name Z coordinate>,  --setZ <Name Z coordinate>
     Set Z Coordinate

   --sub <path>
     Extraction sub dataset basing on geometry

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

   --nscore <string>
     Set normal score transformation

   --decl <string>
     Set 2D declustering

   --csize <double>
     Set 2D cell size for 2D declustering

   --nstep <int>
     Set n steps for 2D declustering

   --sttype <PROPORTIONAL|TRUNCATION|ONLAP|COMBINATION>
     Set stratigraphic condition for coordinate transformation

   --vario <EXPERIMENTAL|MODEL>
     type of variogram

   --dir <OMNI|DIR>
     type of variogram direction

   --dim <3D|2D|1D>
     type of variogram dimension

   --lagspac <VARIABLE|CONSTANT>
     Set lag spacing type

   --deg <degree>
     Set degree step (in degree)

   --degtol <degree>
     Set tolerance (in degree)

   --dirs <string>
     Load discrete directions (in degree)

   --bandw <double>
     Set bandwidth

   --type <AUTO|SPHERICAL|GAUSSIAN|EXPONENTIAL|LINEAR|DEFAULT>
     type of model variogram

   --nugget <double>
     Nugget

   --sill <double>
     Sil

   --range <double>
     Range

   --expvario <string>
     Load experimental variogram

   --npoints <int>
     Number of discretized experimental variogram

   --nstartpoints <int>
     Number of discretized experimental variogram in coverage/2 extended
     zone

   --pstep <double>
     Set points step

   --vclean <int>
     Set variogram clean points

   --rangestep <double>
     Set range step

   --nugstep <double>
     Set nugget step

   --maxdist <double>
     Set max distance for computing variogram

   --tolfac <double>
     Set tolerance factor for computing variogram
     
   --weight
     Set weight on nugget to compute directional variogram

   --eps <double>
     Set eps for plot centering

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.



EXAMPLE: 

1a. Compute variogram: --var = variable --vario = sperimentale
    -V -p /Users/mariannamiola/Desktop/MUSE/MUSE_test/PROJECT_070822 --var Cl (--vario EXPERIMENTAL --dir OMNI)

1b. Compute variogram: --var = variable, --vario = modello, --mode = manuale, impostando il tipo di modello (--type = sferico) da considerare
   -V -p /Users/mariannamiola/Desktop/MUSE/MUSE_test/PROJECT_070822 --var Cl --vario MODEL --dir OMNI --mode MANUAL --type SPHERIC



2. FULL 3D ANISOTROPIC VARIOGRAM (ELLIPSOID) - dim 3D + dir DIR + vario MODEL:
   The directional analysis validated on the XY plane is repeated on arbitrary planes
   (each defined by dip azimuth and dip, in degree). The common structure (model type
   + nugget) is selected by free-fitting a spread subset of the valid planes (~10%,
   at least 3, covering different orientations) and keeping the candidate with the
   lowest weighted MSE; the same structure and nugget are then shared by every
   plane/direction. The fitted ranges of all planes are combined to fit the 3D
   anisotropy ellipsoid: 3 semi-axes (range_max, range_min, range_z) + azimuth, roll,
   pitch (setrot/GSLIB convention, directly usable by the simulation covariance).

   --planes "dipazimuth!dip,dipazimuth!dip,..." sets the analysis planes;
   --planes AUTO (default) scans the plane orientation space with a constant angular
   step (--pstep, default 45 degree): horizontal reference plane (dip 0), tilted
   planes at every intermediate dip over the full azimuth turn (so they do not share
   a common axis), and vertical planes (dip 90) over half a turn.

   For each plane the app saves: one plot per computed direction (<var>_p<i>_dir<j>.jpeg)
   and the rose diagram of ranges with the local anisotropy ellipse (<var>_p<i>_RangesDiagram.jpeg).
   The JSON metadata stores the per-plane variograms (planes_vario) and the ellipsoid (ellipsoid).

   EXAMPLE:
   -V -p /path/to/project --var phi --nscore YES --vario MODEL --dim 3D --dir DIR
      --deg 45 --degtol 22.5 --zdegtol 10 --lagspac VARIABLE --planes AUTO
