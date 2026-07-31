## muse_geometry

**Description:** MUSE - Modelling Uncertainty as a Support of Environment. MUSE-geometry application

### Usage

```bash
muse_geometry -p <PDIR> [OPTIONS]
```

### Options Index

- [`-N`, `--geometry`](#geometry) — Creation of new geometry environment in the project folder
- [`-p`, `--pdir`](#pdir) — Set project directory
- [`--setEPSG`](#setEPSG) — Set project EPSG
- [`-V`, `--vector`](#vector) — Load Vector file
- [`--save`](#save) — Saving data content of geospatial files
- [`--attribute`](#attribute) — Save attribute table from geospatial file
- [`-R`, `--raster`](#raster) — Load Raster file
- [`-P`, `--pcl`](#pcl) — Load point cloud
- [`--points`](#points) — Load filename as POINTS geometry type
- [`--polygon`](#polygon) — Load filename as POLYGON geometry type
- [`-G`, `--gridata`](#gridata) — Grid data - test
- [`--bbp`](#bbp) — Set bounding box points
- [`--rotaxis`](#rotaxis) — Set rotation axis
- [`--rotangle`](#rotangle) — Set clockwise rotation angle (in degree)
- [`--rotcx`](#rotcx) — Set coordinte X of rotation center
- [`--rotcy`](#rotcy) — Set coordinte Y of rotation center
- [`--rotcz`](#rotcz) — Set coordinte Z of rotation center
- [`--tri`](#tri) — Set triangulation for 2D meshing
- [`--convex`](#convex) — Set convex hull for points triangulation
- [`--concave`](#concave) — Set concave hull for points triangulation
- [`--boundary`](#boundary) — Set external boundary for points triangulation
- [`--opt`](#opt) — Set optimization flags enabled for triangulation or tetrahedralization
- [`--grid`](#grid) — Set grid for 2D meshing
- [`--resx`](#resx) — Set x resolution
- [`--resy`](#resy) — Set y resolution
- [`--resz`](#resz) — Set z resolution
- [`--poly`](#poly) — Set generic polygon mesh for 2D meshing
- [`--subset`](#subset) — Set (random) subset of points
- [`--meth`](#meth) — Set method for z values
- [`--setz`](#setz) — Set const z values for new points
- [`-O`, `--offset`](#offset) — Load polygon mesh and apply offset
- [`--delta`](#delta) — Set DELTA offset
- [`--abs`](#abs) — Set ABSOLUTE ELEVATION offset
- [`-z`, `--zoffset`](#zoffset) — Set offset in Z direction
- [`-A`, `--append`](#append) — Append meshes
- [`-T`, `--triobj`](#triobj) — Load trimeshes and create an object closed by surface meshes
- [`-m`, `--mesh`](#mesh) — Set (multi) mesh files
- [`-Q`, `--quadobj`](#quadobj) — Load quadmeshes and create an object closed by surface meshes
- [`--clean`](#clean) — Clean quadrilateral mesh from isolated polys
- [`-M`, `--volmesh`](#volmesh) — Load polygonal mesh and create polyedral mesh
- [`--tet`](#tet) — Set tetrahedralization (using TetGen C++ library)
- [`--vox`](#vox) — Set voxel as polyedralmesh
- [`--hex`](#hex) — Set hexmesh as polyedral
- [`--nmaxvox`](#nmaxvox) — Set n max voxel per side
- [`-W`, `--vmwells`](#vmwells) — Load (closed) polygonal mesh and create a tetrahedral mesh constrained to well(s)
- [`--generate-box`](#generate-box) — Generate box with dimensions: width,height,depth (e.g., \
- [`-o`, `--output`](#output) — Output triangle mesh file (.off format)
- [`-w`, `--well`](#well) — Well specification. Defines one or more wells (cylindrical, box or polygonal) to constrain the tetrahedral mesh. Repeat the flag to add several wells. Each value encodes the well shape and geometry; see
- [`-e`, `--edge-length`](#edge-length) — Target edge length for remeshing (default: auto from input mesh)
- [`--refine-cylinders`](#refine-cylinders) — Refine cylinder mesh by halving the target edge length
- [`--cylinder-edge-scale`](#cylinder-edge-scale) — Scale factor for cylinder edge length (e.g., 0.5 for finer, 2.0 for coarser)
- [`-v`, `--verbose`](#verbose) — Enable verbose output
- [`--max-tet-volume`](#max-tet-volume) — Maximum tetrahedron volume for TetGen (default: auto)
- [`--save-no-wells`](#save-no-wells) — Save tetrahedral mesh without wells (removes tets inside wells, suffix: _no_wells.mesh)
- [`--save-only-wells`](#save-only-wells) — Save tetrahedral mesh with only wells (removes tets outside wells, label -1, suffix: _only_wells.mesh)
- [`-L`, `--trimesh`](#trimesh) — Load trimesh file
- [`--splmet`](#splmet) — Set polys split method
- [`--remesh`](#remesh) — Set remeshing
- [`--mark`](#mark) — Set marked boundary edges for remeshing
- [`--extractbp`](#extractbp) — Set extract boundary points
- [`--it`](#it) — Set number of iterations
- [`--scale`](#scale) — Set scale mesh
- [`--translate`](#translate) — Set translate mesh of the quantity specified by the string
- [`--sx`](#sx) — Set scale factor in X direction
- [`--sy`](#sy) — Set scale factor in Y direction
- [`--sz`](#sz) — Set scale factor in Z direction
- [`-Z`, `--tetmesh`](#tetmesh) — Load tetmesh file
- [`--surf`](#surf) — Extract surface from volume
- [`--obj`](#obj) — Saving surface mesh in obj format (default: .off)
- [`--vtk`](#vtk) — Saving volume mesh in vtk format (default: .mesh)
- [`--xyz`](#xyz) — Saving text file in xyz format
- [`--csv`](#csv) — Saving text file in csv format
- [`-U`, `--merge`](#merge) — Merge two trimesh
- [`--proxthresh`](#proxthresh) — Set proximaty threshold
- [`-S`, `--split`](#split) — Split two trimesh
- [`-F`, `--cscalar`](#cscalar) — Create scalar field from centroids configuration and real samples
- [`--smesh`](#smesh) — Set samples mesh associated to (real) values
- [`--sval`](#sval) — Set samples values associated to each vertex of samples mesh
- [`--rscalar`](#rscalar) — Restore scalar field from centroids configuration and real samples
- [`-D`, `--res`](#res) — Set multiresolution
- [`-f`, `--file`](#file) — Set scalar field file
- [`--refmod`](#refmod) — Geometry model
- [`--outf`](#outf) — Set folder to save outputs
- [`--prec`](#prec) — Set precision
- [`--tol`](#tol) — Set tolerance

### Options

---

#### `-N`, `--geometry` {#geometry}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Creation of new geometry environment in Project directory

**Default:** `false (geometry environment creation is disabled by default).`

**Dependencies:** This command initializes a new geometry environment within the specified project directory.
It sets up the necessary folder structure ([--pdir](#pdir) path/to/project/dir) to store geometrical items and models created by various geospatial input data sources and processing methods (computational geometry techniques).
It is used with:
- [--pdir](#pdir): Project directory
- [--setEPSG](#setEPSG): Set coordinate reference system of geometry environment (optional)

**Example:** `muse_geometry -N -p /path/to/project`

---

#### `-p`, `--pdir` {#pdir}

**Type:** Value | `std::string`

**Format:** `string (path to the project directory)`

**Description:** Project directory

**Default:** `"/path/to/project/dir" (placeholder value, should be replaced with an actual path).`

**Dependencies:** Used with -N/[--geometry](#geometry) for geometry environment creation (and in other cases in which the project locations must be specified).
Required to specify the project directory where the geometry environment will be created.
The command initializes the necessary folder structure within the project directory to store geometrical items and models created by various geospatial input data sources and processing methods (computational geometry techniques).

**Example:** `-p /path/to/project/dir`

---

#### `--setEPSG` {#setEPSG}

**Type:** Value | `std::string`

**Format:** `string in the form EPSG:<code>`

**Description:** Set project EPSG

**Default:** `"Unknown" (no coordinate reference system is assigned by default).`

**Dependencies:** Optional parameter, used with -N/[--geometry](#geometry).

**Example:** `muse_geometry -N -p /path/to/project --setEPSG EPSG:32633`

---

#### `-V`, `--vector` {#vector}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Load Vector file

**Default:** `false (vector loading is disabled by default).`

**Dependencies:** Mutually exclusive with -R/[--raster](#raster) and -P/[--pcl](#pcl)
Used with -N/[--geometry](#geometry) for geometry creation
Supports: .shp, .gpkg formats
OPTIONAL modifiers:
- [--save](#save): Save data content
- [--attribute](#attribute): Save attribute table

---

#### `--save` {#save}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Saving data content of geospatial files

**Default:** `false (data content is not saved by default).`

**Dependencies:** Optional modifier for -V/[--vector](#vector).

---

#### `--attribute` {#attribute}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Save attribute table from geospatial file

**Default:** `false (attribute table is not saved by default).`

**Dependencies:** Optional modifier for -V/[--vector](#vector).

---

#### `-R`, `--raster` {#raster}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Load Raster file

**Default:** `false (raster loading is disabled by default).`

**Dependencies:** Mutually exclusive with -V/[--vector](#vector) and -P/[--pcl](#pcl)
Used with -N/[--geometry](#geometry) for geometry creation
Supports: .ASCII format

---

#### `-P`, `--pcl` {#pcl}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Load point cloud

**Default:** `false (point cloud loading is disabled by default).`

**Dependencies:** Mutually exclusive with -V/[--vector](#vector) and -R/[--raster](#raster)
Used with -N/[--geometry](#geometry) for geometry creation
Supports: .xyz, .dat, .txt formats
Can be used with:
- [--points](#points): Specify points geometry file
- [--polygon](#polygon): Specify polygon geometry file

---

#### `--points` {#points}

**Type:** Value | `std::string`

**Format:** `string (path to a file loaded as POINTS geometry)`

**Description:** Load filename as POINTS geometry type

**Default:** `empty (no points file is loaded).`

**Dependencies:** Used with -P/[--pcl](#pcl).

---

#### `--polygon` {#polygon}

**Type:** Value | `std::string`

**Format:** `string (path to a file loaded as POLYGON geometry)`

**Description:** Load filename as POLYGON geometry type

**Default:** `empty (no polygon file is loaded).`

**Dependencies:** Used with -P/[--pcl](#pcl).

---

#### `-G`, `--gridata` {#gridata}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Grid data - test

**Default:** `false (grid data test is disabled by default).`

---

#### `--bbp` {#bbp}

**Type:** Value | `std::string`, repeatable

**Format:** `string (point coordinates, e.g. "x,y,z"); repeatable`

**Description:** Set bounding box points. Coordinates of the points defining the bounding box used by the grid-data test. Repeat the flag to pass several points.

**Default:** `empty (no bounding box points are set).`

**Dependencies:** Used with -G/[--gridata](#gridata).

**Example:** `muse_geometry -G --bbp 0,0,0 --bbp 10,10,0`

---

#### `--rotaxis` {#rotaxis}

**Type:** Value | `std::string`

**Format:** `string` (`X`, `Y`, `Z`, `NO`)

**Description:** Set rotation axis

**Default:** `"NO" (no rotation is applied).`

**Dependencies:** When using rotation, these flags work together:
- [--rotaxis](#rotaxis): Rotation axis (X, Y, Z)
- [--rotangle](#rotangle): Rotation angle (required if rotaxis != NO)
- [--rotcx](#rotcx), [--rotcy](#rotcy), [--rotcz](#rotcz): Rotation center coordinates

**Example:** `For Z-axis rotation: --rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0`

---

#### `--rotangle` {#rotangle}

**Type:** Value | `double`

**Format:** `double (degrees)`

**Description:** Set clockwise rotation angle (in degree)

**Default:** `0.0`

**Dependencies:** Used together with [--rotaxis](#rotaxis) flag. Required when rotaxis != NO

---

#### `--rotcx` {#rotcx}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set coordinte X of rotation center

**Default:** `0.0`

---

#### `--rotcy` {#rotcy}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set coordinte Y of rotation center

**Default:** `0.0`

---

#### `--rotcz` {#rotcz}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set coordinte Z of rotation center

**Default:** `0.0`

---

#### `--tri` {#tri}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set triangulation for 2D meshing

**Default:** `false (triangulation is disabled by default).`

**Dependencies:** Mutually exclusive with [--grid](#grid) meshing method
Used with -N/[--geometry](#geometry) for mesh creation
Triangulation configuration (choose one):
- [--convex](#convex): Convex hull triangulation (mutually exclusive with [--concave](#concave))
- [--concave](#concave): Concave hull triangulation (mutually exclusive with [--convex](#convex))
OPTIONAL:
- [--boundary](#boundary): External boundary file
- [--opt](#opt): Optimization flags

**Example:** `--tri --convex OR --tri --concave --boundary /path/to/boundary.shp`

---

#### `--convex` {#convex}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set convex hull for points triangulation

**Default:** `false (convex hull triangulation is disabled by default).`

**Dependencies:** Used with [--tri](#tri) flag. Mutually exclusive with [--concave](#concave)

---

#### `--concave` {#concave}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set concave hull for points triangulation

**Default:** `false (concave hull triangulation is disabled by default).`

**Dependencies:** Used with [--tri](#tri) flag. Mutually exclusive with [--convex](#convex)

---

#### `--boundary` {#boundary}

**Type:** Value | `std::string`

**Format:** `string (path to a boundary file)`

**Description:** Set external boundary for points triangulation

**Default:** `empty (no external boundary is used).`

**Dependencies:** Optional. Used with [--tri](#tri).

---

#### `--opt` {#opt}

**Type:** Value | `std::string`

**Format:** `string (Triangle/TetGen optimization flags, e.g. "pq20")`

**Description:** Set optimization flags to optimize triangulation (e.g., "pq20" for preserve+quality 20) or tetrahedralization (e.g., "pq20" for preserve+quality 20 in TetGen)

**Default:** `empty (no optimization flags are passed).`

**Dependencies:** Used with [--tri](#tri) flag for triangulation optimization and with [--tet](#tet) flag for tetrahedralization optimization.
Refer to Triangle and TetGen documentation for available flags and options.

**Example:** `For Triangle: --tri --opt "pq20" OR For TetGen: --tet --opt "pq20"`

---

#### `--grid` {#grid}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set grid for 2D meshing

**Default:** `false (grid meshing is disabled by default).`

**Dependencies:** Mutually exclusive with [--tri](#tri) triangulation method
Used with -N/[--geometry](#geometry) for mesh creation
Grid configuration requires:
- [--resx](#resx): X resolution (mandatory)
- [--resy](#resy): Y resolution (mandatory)
OPTIONAL:
- [--resz](#resz): Z resolution (for 3D grids)

**Example:** `--grid --resx 10.0 --resy 10.0`

---

#### `--resx` {#resx}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set x resolution

**Default:** `1.0`

**Dependencies:** Used with [--grid](#grid) flag. Required for grid meshing
Must be used together with [--resy](#resy)

---

#### `--resy` {#resy}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set y resolution

**Default:** `1.0`

**Dependencies:** Used with [--grid](#grid) flag. Required for grid meshing
Must be used together with [--resx](#resx)

---

#### `--resz` {#resz}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set z resolution

**Default:** `1.0`

**Dependencies:** Optional. Used with [--grid](#grid) for 3D grids.

---

#### `--poly` {#poly}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set generic polygon mesh for 2D meshing

**Default:** `false (generic polygon meshing is disabled by default).`

---

#### `--subset` {#subset}

**Type:** Value | `int`

**Format:** `int (number of points in the random subset)`

**Description:** Set (random) subset of points

**Default:** `10`

**Dependencies:** Optional. Extracts a random subset of the input points.

---

#### `--meth` {#meth}

**Type:** Value | `std::string`

**Format:** `string` (`MEAN`, `CONSTANT`, `NEAR`, `KRIGING`)

**Description:** Set method for z values

**Default:** `"CONSTANT"`

---

#### `--setz` {#setz}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set const z values for new points

**Default:** `0.0`

**Dependencies:** Used with [--meth](#meth) CONSTANT.

---

#### `-O`, `--offset` {#offset}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Load polygon mesh and apply offset

**Default:** `false (offset is disabled by default).`

---

#### `--delta` {#delta}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set DELTA offset

**Default:** `false (delta offset is disabled by default).`

**Dependencies:** Used with -O/[--offset](#offset). Mutually exclusive with [--abs](#abs).

---

#### `--abs` {#abs}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set ABSOLUTE ELEVATION offset

**Default:** `false (absolute-elevation offset is disabled by default).`

**Dependencies:** Used with -O/[--offset](#offset). Mutually exclusive with [--delta](#delta).

---

#### `-z`, `--zoffset` {#zoffset}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set offset in Z direction

**Default:** `0.0`

**Dependencies:** Used with -O/[--offset](#offset).

---

#### `-A`, `--append` {#append}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Append meshes

**Default:** `false (mesh appending is disabled by default).`

---

#### `-T`, `--triobj` {#triobj}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Load trimeshes and create an object closed by surface meshes

**Default:** `false (tri-object creation is disabled by default).`

---

#### `-m`, `--mesh` {#mesh}

**Type:** Value | `std::string`, repeatable

**Format:** `string (path to a mesh file); repeatable`

**Description:** Set (multi) mesh files. One or more mesh files used as input for the object-creation operations (e.g. -T/--triobj, -Q/--quadobj). Repeat the flag to pass several meshes.

**Default:** `empty (no mesh file is passed).`

**Dependencies:** Used with the object-creation operations.

**Example:** `muse_geometry -T -p /path/to/project -m top.off -m bottom.off`

---

#### `-Q`, `--quadobj` {#quadobj}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Load quadmeshes and create an object closed by surface meshes

**Default:** `false (quad-object creation is disabled by default).`

---

#### `--clean` {#clean}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Clean quadrilateral mesh from isolated polys

**Default:** `false (cleaning is disabled by default).`

**Dependencies:** Used with -Q/[--quadobj](#quadobj).

---

#### `-M`, `--volmesh` {#volmesh}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Load polygonal mesh and create polyedral mesh

**Default:** `false (volumetric object creation is disabled by default).`

---

#### `--tet` {#tet}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set tetrahedralization

**Default:** `false (tetrahedralization is disabled by default).`

**Dependencies:** Used with -M/[--volmesh](#volmesh).

---

#### `--vox` {#vox}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set voxel as polyedralmesh

**Default:** `false (voxel meshing is disabled by default).`

**Dependencies:** Used with -M/[--volmesh](#volmesh).

---

#### `--hex` {#hex}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set hexmesh as polyedral

**Default:** `false (hexmesh meshing is disabled by default).`

**Dependencies:** Used with -M/[--volmesh](#volmesh).

---

#### `--nmaxvox` {#nmaxvox}

**Type:** Value | `int`

**Format:** `int`

**Description:** Set n max voxel per side

**Default:** `1`

**Dependencies:** Used with [--vox](#vox).

---

#### `-W`, `--vmwells` {#vmwells}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Load (closed) polygonal mesh and create a tetrahedral mesh constrained to specific well(s)

**Default:** `false (well-constrained volume creation is disabled by default).`

**Dependencies:** When using this option, these flags work together:
- -W/[--vmwells](#vmwells): Load (closed) polygonal mesh and create a tetrahedral mesh constrained to specific well(s)
- [--well](#well): Specify well(s) file(s) (can be used multiple times for multiple wells)

**Example:** `muse_geometry -M --tet --vmwells --well /path/to/well1.shp --well /path/to/well2.shp`

---

#### `--generate-box` {#generate-box}

**Type:** Value | `std::string`

**Format:** `string in the form "width,height,depth" (e.g. "10,5,8")`

**Description:** Generate a box mesh with the given dimensions (width,height,depth) to be used as input geometry for the well-constrained volume creation.

**Default:** `empty (no box is generated).`

**Dependencies:** Used with -W/[--vmwells](#vmwells) as an alternative to loading an input mesh.

**Example:** `muse_geometry -W --generate-box "10,5,8" -o box.off`

---

#### `-o`, `--output` {#output}

**Type:** Value | `std::string`

**Format:** `string (path to an .off file)`

**Description:** Output triangle mesh file. Path of the .off file where the generated/remeshed triangle mesh is written.

**Default:** `empty (a default output name is used).`

**Dependencies:** Used with -W/[--vmwells](#vmwells) / [--generate-box](#generate-box).

**Example:** `muse_geometry -W --generate-box "10,5,8" -o box.off`

---

#### `-w`, `--well` {#well}

**Type:** Value | `std::string`, repeatable

**Format:** `for the accepted encodings.`

**Description:** Well specification. Defines one or more wells (cylindrical, box or polygonal) to constrain the tetrahedral mesh. Repeat the flag to add several wells. Each value encodes the well shape and geometry; see

**Default:** `empty (no well is created).`

**Dependencies:** Used with -W/[--vmwells](#vmwells).

**Example:** `muse_geometry -W --vmwells --well CYL:XYH:100,200,50,5 --well BOX:XYB:100,200,10,10`

---

#### `-e`, `--edge-length` {#edge-length}

**Type:** Value | `double`

**Format:** `double (length; -1 = automatic)`

**Description:** Target edge length for remeshing. Desired edge length used when remeshing the input/generated surface before tetrahedralization.

**Default:** `-1.0 (auto: derived from the input mesh).`

**Dependencies:** Used with -W/[--vmwells](#vmwells).

**Example:** `muse_geometry -W --vmwells --edge-length 2.0`

---

#### `--refine-cylinders` {#refine-cylinders}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Refine cylinder mesh by halving the target edge length, producing a finer discretization of the cylindrical wells.

**Default:** `false (cylinder refinement is disabled by default).`

**Dependencies:** Used with -W/[--vmwells](#vmwells).

---

#### `--cylinder-edge-scale` {#cylinder-edge-scale}

**Type:** Value | `double`

**Format:** `double (scale factor)`

**Description:** Scale factor for cylinder edge length. Multiplies the target edge length used for cylindrical wells (e.g. 0.5 for finer, 2.0 for coarser).

**Default:** `1.0 (same edge length as the rest of the mesh).`

**Dependencies:** Used with -W/[--vmwells](#vmwells).

**Example:** `muse_geometry -W --vmwells --cylinder-edge-scale 0.5`

---

#### `-v`, `--verbose` {#verbose}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Enable verbose output, printing additional diagnostic information during the well/tetrahedralization process.

**Default:** `false (verbose output is disabled by default).`

---

#### `--max-tet-volume` {#max-tet-volume}

**Type:** Value | `double`

**Format:** `double (volume; -1 = automatic)`

**Description:** Maximum tetrahedron volume for TetGen. Upper bound on the volume of the generated tetrahedra (quality/size constraint).

**Default:** `-1.0 (auto: chosen by TetGen).`

**Dependencies:** Used with -W/[--vmwells](#vmwells) / [--tet](#tet).

**Example:** `muse_geometry -W --vmwells --max-tet-volume 100.0`

---

#### `--save-no-wells` {#save-no-wells}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Save tetrahedral mesh without wells. Removes the tetrahedra inside the wells and writes the result with suffix _no_wells.mesh.

**Default:** `false (this variant is not saved by default).`

**Dependencies:** Used with -W/[--vmwells](#vmwells).

---

#### `--save-only-wells` {#save-only-wells}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Save tetrahedral mesh with only wells. Removes the tetrahedra outside the wells (label -1) and writes the result with suffix _only_wells.mesh.

**Default:** `false (this variant is not saved by default).`

**Dependencies:** Used with -W/[--vmwells](#vmwells).

---

#### `-L`, `--trimesh` {#trimesh}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Load trimesh file

**Default:** `false (trimesh loading is disabled by default).`

---

#### `--splmet` {#splmet}

**Type:** Value | `std::string`

**Format:** `string (split method name)`

**Description:** Set polys split method

**Default:** `"CENTROID"`

**Dependencies:** Used with -L/[--trimesh](#trimesh).

---

#### `--remesh` {#remesh}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set remeshing

**Default:** `false (remeshing is disabled by default).`

---

#### `--mark` {#mark}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set marked boundary edges for remeshing

**Default:** `false (edge marking is disabled by default).`

**Dependencies:** Used with [--remesh](#remesh).

---

#### `--extractbp` {#extractbp}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set extract boundary points

**Default:** `false (boundary-point extraction is disabled by default).`

---

#### `--it` {#it}

**Type:** Value | `int`

**Format:** `int`

**Description:** Set number of iterations

**Default:** `1`

**Dependencies:** Number of iterations used by the iterative operations (e.g. remeshing).

---

#### `--scale` {#scale}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set scale mesh

**Default:** `false (mesh scaling is disabled by default).`

---

#### `--translate` {#translate}

**Type:** Value | `std::string`

**Format:** `string in the form "x,y,z" (translation along each axis)`

**Description:** Set translate mesh of the quantity specified by the string (e.g., "10,20,5"). This can be used to translate the loaded surface mesh by the specified amounts in the x, y, and z directions. For example, if the string is "10,20,5", the mesh will be translated by 10 units in the x direction, 20 units in the y direction, and 5 units in the z direction.

**Default:** `"0,0,0" (no translation).`

**Dependencies:** The string must be in the format "x,y,z" where x, y, and z are the translation values in each direction. The command is able for loading surface switch.

**Example:** `-L -p /path/to/project -m /path/to/mesh.off --translate 10,20,5`

---

#### `--sx` {#sx}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set scale factor in X direction

**Default:** `1.0`

**Dependencies:** Used with [--scale](#scale).

---

#### `--sy` {#sy}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set scale factor in Y direction

**Default:** `1.0`

**Dependencies:** Used with [--scale](#scale).

---

#### `--sz` {#sz}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set scale factor in Z direction

**Default:** `1.0`

**Dependencies:** Used with [--scale](#scale).

---

#### `-Z`, `--tetmesh` {#tetmesh}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Load tetmesh file

**Default:** `false (tetmesh loading is disabled by default).`

---

#### `--surf` {#surf}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Extract surface from volume

**Default:** `false (surface extraction is disabled by default).`

**Dependencies:** Used with -Z/[--tetmesh](#tetmesh).

---

#### `--obj` {#obj}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Saving surface mesh in obj format

**Default:** `false (OBJ output is disabled by default; default is .off).`

**Example:** `muse_geometry -L -p .. --obj`

---

#### `--vtk` {#vtk}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Saving volume mesh in vtk format

**Default:** `false (VTK output is disabled by default; default is .mesh).`

**Example:** `muse_geometry -M -p .. --vtk`

---

#### `--xyz` {#xyz}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Saving text file in xyz format (instead of the default .dat).

**Default:** `false (xyz output is disabled by default).`

---

#### `--csv` {#csv}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Saving text file in csv format (instead of the default .dat).

**Default:** `false (csv output is disabled by default).`

---

#### `-U`, `--merge` {#merge}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Merge two trimesh

**Default:** `false (mesh merging is disabled by default).`

---

#### `--proxthresh` {#proxthresh}

**Type:** Value | `int`

**Format:** `int`

**Description:** Set proximaty threshold

**Default:** `0`

**Dependencies:** Used with -U/[--merge](#merge).

---

#### `-S`, `--split` {#split}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Split two trimesh

**Default:** `false (mesh splitting is disabled by default).`

---

#### `-F`, `--cscalar` {#cscalar}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Create scalar field from centroids configuration and real samples

**Default:** `false (scalar field creation is disabled by default).`

---

#### `--smesh` {#smesh}

**Type:** Value | `std::string`

**Format:** `string (path to the samples mesh)`

**Description:** Set samples mesh associated to (real) values

**Default:** `empty (no samples mesh is set).`

**Dependencies:** Used with -F/[--cscalar](#cscalar).

---

#### `--sval` {#sval}

**Type:** Value | `std::string`

**Format:** `string (path to the samples values)`

**Description:** Set samples values associated to each vertex of samples mesh

**Default:** `empty (no samples values are set).`

**Dependencies:** Used with -F/[--cscalar](#cscalar).

---

#### `--rscalar` {#rscalar}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Restore scalar field from centroids configuration and real samples

**Default:** `false (scalar field restore is disabled by default).`

---

#### `-D`, `--res` {#res}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set multiresolution

**Default:** `false (multiresolution is disabled by default).`

---

#### `-f`, `--file` {#file}

**Type:** Value | `std::string`

**Format:** `string (path to the scalar field file)`

**Description:** Set scalar field file

**Default:** `"Directory" (placeholder value, should be replaced with an actual path).`

**Dependencies:** Used with -D/[--res](#res).

---

#### `--refmod` {#refmod}

**Type:** Value | `std::string`

**Format:** `string (geometry model name)`

**Description:** Geometry model

**Default:** `"name_geometry" (placeholder value, should be replaced with an actual geometry model name).`

**Dependencies:** Used with -D/[--res](#res).

---

#### `--outf` {#outf}

**Type:** Value | `std::string`

**Format:** `string (path to the output folder)`

**Description:** Set folder to save outputs

**Default:** `"Directory" (placeholder value, should be replaced with an actual output folder).`

---

#### `--prec` {#prec}

**Type:** Value | `int`

**Format:** `int (number of decimal digits)`

**Description:** Set precision. Number of decimal digits used when writing numerical outputs.

**Default:** `6`

---

#### `--tol` {#tol}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set tolerance. Numerical tolerance used in geometric comparisons (e.g. point/vertex matching).

**Default:** `1e-02`

---

