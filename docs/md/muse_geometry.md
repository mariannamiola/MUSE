## muse_geometry

**Description:** MUSE = Modelling of Uncertainty as a Support of Environment; Geometry tool

### Usage

```bash
muse_geometry [OPTIONS]
```

### Options

#### `-N`, `--geometry`
**Type:** Switch (flag)
**Description:** Creation of new geometry

Flag to creation of new geometry

#### `-p`, `--pdir`
**Type:** Value (flag)
**Description:** Project directory

Path to project directory

#### `--setEPSG`
**Type:** Value (flag)
**Description:** Set project EPSG

project epsg

#### `-V`, `--vector`
**Type:** Switch (flag)
**Description:** Load Vector file

Enable load vector file

#### `--save`
**Type:** Switch (flag)
**Description:** Saving data content of geospatial files

Enable saving data content of geospatial files

#### `--attribute`
**Type:** Switch (flag)
**Description:** Save attribute table from geospatial file

Enable save attribute table from geospatial file

#### `-R`, `--raster`
**Type:** Switch (flag)
**Description:** Load Raster file

Enable load raster file

#### `-P`, `--pcl`
**Type:** Switch (flag)
**Description:** Load point cloud

Enable load point cloud

#### `--points`
**Type:** Value (flag)
**Description:** Load filename as POINTS geometry type

Path to load filename as points geometry type

#### `--polygon`
**Type:** Value (flag)
**Description:** Load filename as POLYGON geometry type

Path to load filename as polygon geometry type

#### `-G`, `--gridata`
**Type:** Switch (flag)
**Description:** Grid data - test

Enable grid data - test

#### `--rotaxis`
**Type:** Value (flag)
**Description:** Set rotation axis

rotation axis

#### `--rotangle`
**Type:** Value (flag)
**Description:** Set clockwise rotation angle (in degree)

clockwise rotation angle (in degree)

#### `--rotcx`
**Type:** Value (flag)
**Description:** Set coordinte X of rotation center

coordinte x of rotation center

#### `--rotcy`
**Type:** Value (flag)
**Description:** Set coordinte Y of rotation center

coordinte y of rotation center

#### `--rotcz`
**Type:** Value (flag)
**Description:** Set coordinte Z of rotation center

coordinte z of rotation center

#### `--tri`
**Type:** Switch (flag)
**Description:** Set triangulation for 2D meshing

Enable set triangulation for 2d meshing

#### `--convex`
**Type:** Switch (flag)
**Description:** Set convex hull for points triangulation

Enable set convex hull for points triangulation

#### `--concave`
**Type:** Switch (flag)
**Description:** Set concave hull for points triangulation

Enable set concave hull for points triangulation

#### `--boundary`
**Type:** Value (flag)
**Description:** Set external boundary for points triangulation

external boundary for points triangulation

#### `--opt`
**Type:** Value (flag)
**Description:** Set optimization flags

optimization flags

#### `--grid`
**Type:** Switch (flag)
**Description:** Set grid for 2D meshing

Enable set grid for 2d meshing

#### `--resx`
**Type:** Value (flag)
**Description:** Set x resolution

x resolution

#### `--resy`
**Type:** Value (flag)
**Description:** Set y resolution

y resolution

#### `--resz`
**Type:** Value (flag)
**Description:** Set z resolution

z resolution

#### `--poly`
**Type:** Switch (flag)
**Description:** Set generic polygon mesh for 2D meshing

Enable set generic polygon mesh for 2d meshing

#### `--subset`
**Type:** Value (flag)
**Description:** Set (random) subset of points

(random) subset of points

#### `--meth`
**Type:** Value (flag)
**Description:** Set method for z values

method for z values

#### `--setz`
**Type:** Value (flag)
**Description:** Set const z values for new points

const z values for new points

#### `-O`, `--offset`
**Type:** Switch (flag)
**Description:** Load polygon mesh and apply offset

Enable load polygon mesh and apply offset

#### `--delta`
**Type:** Switch (flag)
**Description:** Set DELTA offset

Enable set delta offset

#### `--abs`
**Type:** Switch (flag)
**Description:** Set ABSOLUTE ELEVATION offset

Enable set absolute elevation offset

#### `-z`, `--zoffset`
**Type:** Value (flag)
**Description:** Set offset in Z direction

offset in z direction

#### `-A`, `--append`
**Type:** Switch (flag)
**Description:** Append meshes

Enable append meshes

#### `-T`, `--triobj`
**Type:** Switch (flag)
**Description:** Load trimeshes and create an object closed by surface meshes

Flag to load trimeshes and create an object closed by surface meshes

#### `-Q`, `--quadobj`
**Type:** Switch (flag)
**Description:** Load quadmeshes and create an object closed by surface meshes

Flag to load quadmeshes and create an object closed by surface meshes

#### `--clean`
**Type:** Switch (flag)
**Description:** Clean quadrilateral mesh from isolated polys

Enable clean quadrilateral mesh from isolated polys

#### `-M`, `--volmesh`
**Type:** Switch (flag)
**Description:** Load polygonal mesh and create polyedral mesh

Flag to load polygonal mesh and create polyedral mesh

#### `--tet`
**Type:** Switch (flag)
**Description:** Set tetrahedralization

Enable set tetrahedralization

#### `--vox`
**Type:** Switch (flag)
**Description:** Set voxel as polyedralmesh

Enable set voxel as polyedralmesh

#### `--hex`
**Type:** Switch (flag)
**Description:** Set hexmesh as polyedral

Enable set hexmesh as polyedral

#### `--nmaxvox`
**Type:** Value (flag)
**Description:** Set n max voxel per side

Number of set n max voxel per side

#### `-L`, `--trimesh`
**Type:** Switch (flag)
**Description:** Load trimesh file

Enable load trimesh file

#### `--splmet`
**Type:** Value (flag)
**Description:** Set polys split method

polys split method

#### `--remesh`
**Type:** Switch (flag)
**Description:** Set remeshing

Enable set remeshing

#### `--mark`
**Type:** Switch (flag)
**Description:** Set marked boundary edges for remeshing

Enable set marked boundary edges for remeshing

#### `--collapse`
**Type:** Switch (flag)
**Description:** Set collapse on edge to simplify mesh boundary

Enable set collapse on edge to simplify mesh boundary

#### `--extractbp`
**Type:** Switch (flag)
**Description:** Set extract boundary points

Enable set extract boundary points

#### `--it`
**Type:** Value (flag)
**Description:** Set number of iterations

Number of set number of iterations

#### `--scale`
**Type:** Switch (flag)
**Description:** Set scale mesh

Enable set scale mesh

#### `--sx`
**Type:** Value (flag)
**Description:** Set scale factor in X direction

scale factor in x direction

#### `--sy`
**Type:** Value (flag)
**Description:** Set scale factor in Y direction

scale factor in y direction

#### `--sz`
**Type:** Value (flag)
**Description:** Set scale factor in Z direction

scale factor in z direction

#### `-Z`, `--tetmesh`
**Type:** Switch (flag)
**Description:** Load tetmesh file

Enable load tetmesh file

#### `--surf`
**Type:** Switch (flag)
**Description:** Extract surface from volume

Enable extract surface from volume

#### `--obj`
**Type:** Switch (flag)
**Description:** Saving mesh in obj format

Enable saving mesh in obj format

#### `--vtk`
**Type:** Switch (flag)
**Description:** Saving mesh in vtk format

Enable saving mesh in vtk format

#### `-U`, `--merge`
**Type:** Switch (flag)
**Description:** Merge two trimesh

Enable merge two trimesh

#### `--proxthresh`
**Type:** Value (flag)
**Description:** Set proximaty threshold

proximaty threshold

#### `-S`, `--split`
**Type:** Switch (flag)
**Description:** Split two trimesh

Enable split two trimesh

#### `-F`, `--cscalar`
**Type:** Switch (flag)
**Description:** Create scalar field from centroids configuration and real samples

Flag to create scalar field from centroids configuration and real samples

#### `--smesh`
**Type:** Value (flag)
**Description:** Set samples mesh associated to (real) values

samples mesh associated to (real) values

#### `--sval`
**Type:** Value (flag)
**Description:** Set samples values associated to each vertex of samples mesh

samples values associated to each vertex of samples mesh

#### `--rscalar`
**Type:** Switch (flag)
**Description:** Restore scalar field from centroids configuration and real samples

Enable restore scalar field from centroids configuration and real samples

#### `-D`, `--res`
**Type:** Switch (flag)
**Description:** Set multiresolution

Enable set multiresolution

#### `-f`, `--file`
**Type:** Value (flag)
**Description:** Set scalar field file

Path to set scalar field file

#### `--refmod`
**Type:** Value (flag)
**Description:** Geometry model

geometry model

#### `--outf`
**Type:** Value (flag)
**Description:** Set folder to save outputs

Path to set folder to save outputs

---

