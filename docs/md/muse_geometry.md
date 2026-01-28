## muse_geometry

**Description:** MUSE = Modelling of Uncertainty as a Support of Environment; Geometry tool

### Usage

```bash
muse_geometry [OPTIONS]
```

### Options

#### `-N`, `--geometry` {#geometry}
**Type:** Switch (flag)
**Description:** Creation of new geometry

Flag to creation of new geometry

**Dependencies:** When creating new geometry, requires:
- Input data source (choose one):
- -V/[--vector](#vector): Load vector file (.shp, .gpkg)
- -R/[--raster](#raster): Load raster file (.ASCII)
- -P/[--pcl](#pcl): Load point cloud (.xyz, .dat, .txt)
- Processing method (choose one):
- [--tri](#tri): Triangulation for 2D meshing
- [--grid](#grid): Grid for 2D meshing
OPTIONAL:
- [--pdir](#pdir): Project directory
- [--setEPSG](#setEPSG): Set coordinate system

**Example:** `muse_geometry -N --vector --tri --pdir /path/to/project`

#### `-p`, `--pdir` {#pdir}
**Type:** Value (flag)
**Description:** Project directory

Path to project directory

#### `--setEPSG` {#setEPSG}
**Type:** Value (flag)
**Description:** Set project EPSG

project epsg

#### `-V`, `--vector` {#vector}
**Type:** Switch (flag)
**Description:** Load Vector file

Enable load vector file

**Dependencies:** Mutually exclusive with -R/[--raster](#raster) and -P/[--pcl](#pcl)
Used with -N/[--geometry](#geometry) for geometry creation
Supports: .shp, .gpkg formats
OPTIONAL modifiers:
- [--save](#save): Save data content
- [--attribute](#attribute): Save attribute table

#### `--save` {#save}
**Type:** Switch (flag)
**Description:** Saving data content of geospatial files

Enable saving data content of geospatial files

#### `--attribute` {#attribute}
**Type:** Switch (flag)
**Description:** Save attribute table from geospatial file

Enable save attribute table from geospatial file

#### `-R`, `--raster` {#raster}
**Type:** Switch (flag)
**Description:** Load Raster file

Enable load raster file

**Dependencies:** Mutually exclusive with -V/[--vector](#vector) and -P/[--pcl](#pcl)
Used with -N/[--geometry](#geometry) for geometry creation
Supports: .ASCII format

#### `-P`, `--pcl` {#pcl}
**Type:** Switch (flag)
**Description:** Load point cloud

Enable load point cloud

**Dependencies:** Mutually exclusive with -V/[--vector](#vector) and -R/[--raster](#raster)
Used with -N/[--geometry](#geometry) for geometry creation
Supports: .xyz, .dat, .txt formats
Can be used with:
- [--points](#points): Specify points geometry file
- [--polygon](#polygon): Specify polygon geometry file

#### `--points` {#points}
**Type:** Value (flag)
**Description:** Load filename as POINTS geometry type

Path to load filename as points geometry type

#### `--polygon` {#polygon}
**Type:** Value (flag)
**Description:** Load filename as POLYGON geometry type

Path to load filename as polygon geometry type

#### `-G`, `--gridata` {#gridata}
**Type:** Switch (flag)
**Description:** Grid data - test

Enable grid data - test

#### `--rotaxis` {#rotaxis}
**Type:** Value (flag)
**Description:** Set rotation axis

rotation axis

**Dependencies:** When using rotation, these flags work together:
- [--rotaxis](#rotaxis): Rotation axis (X, Y, Z)
- [--rotangle](#rotangle): Rotation angle (required if rotaxis != NO)
- [--rotcx](#rotcx), [--rotcy](#rotcy), [--rotcz](#rotcz): Rotation center coordinates

**Example:** `For Z-axis rotation: --rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0`

#### `--rotangle` {#rotangle}
**Type:** Value (flag)
**Description:** Set clockwise rotation angle (in degree)

clockwise rotation angle (in degree)

**Dependencies:** Used together with [--rotaxis](#rotaxis) flag. Required when rotaxis != NO

#### `--rotcx` {#rotcx}
**Type:** Value (flag)
**Description:** Set coordinte X of rotation center

coordinte x of rotation center

#### `--rotcy` {#rotcy}
**Type:** Value (flag)
**Description:** Set coordinte Y of rotation center

coordinte y of rotation center

#### `--rotcz` {#rotcz}
**Type:** Value (flag)
**Description:** Set coordinte Z of rotation center

coordinte z of rotation center

#### `--tri` {#tri}
**Type:** Switch (flag)
**Description:** Set triangulation for 2D meshing

Enable set triangulation for 2d meshing

**Dependencies:** Mutually exclusive with [--grid](#grid) meshing method
Used with -N/[--geometry](#geometry) for mesh creation
Triangulation configuration (choose one):
- [--convex](#convex): Convex hull triangulation (mutually exclusive with [--concave](#concave))
- [--concave](#concave): Concave hull triangulation (mutually exclusive with [--convex](#convex))
OPTIONAL:
- [--boundary](#boundary): External boundary file
- [--opt](#opt): Optimization flags

**Example:** `--tri --convex OR --tri --concave --boundary /path/to/boundary.shp`

#### `--convex` {#convex}
**Type:** Switch (flag)
**Description:** Set convex hull for points triangulation

Enable set convex hull for points triangulation

**Dependencies:** Used with [--tri](#tri) flag. Mutually exclusive with [--concave](#concave)

#### `--concave` {#concave}
**Type:** Switch (flag)
**Description:** Set concave hull for points triangulation

Enable set concave hull for points triangulation

**Dependencies:** Used with [--tri](#tri) flag. Mutually exclusive with [--convex](#convex)

#### `--boundary` {#boundary}
**Type:** Value (flag)
**Description:** Set external boundary for points triangulation

external boundary for points triangulation

#### `--opt` {#opt}
**Type:** Value (flag)
**Description:** Set optimization flags

optimization flags

#### `--grid` {#grid}
**Type:** Switch (flag)
**Description:** Set grid for 2D meshing

Enable set grid for 2d meshing

**Dependencies:** Mutually exclusive with [--tri](#tri) triangulation method
Used with -N/[--geometry](#geometry) for mesh creation
Grid configuration requires:
- [--resx](#resx): X resolution (mandatory)
- [--resy](#resy): Y resolution (mandatory)
OPTIONAL:
- [--resz](#resz): Z resolution (for 3D grids)

**Example:** `--grid --resx 10.0 --resy 10.0`

#### `--resx` {#resx}
**Type:** Value (flag)
**Description:** Set x resolution

x resolution

**Dependencies:** Used with [--grid](#grid) flag. Required for grid meshing
Must be used together with [--resy](#resy)

#### `--resy` {#resy}
**Type:** Value (flag)
**Description:** Set y resolution

y resolution

**Dependencies:** Used with [--grid](#grid) flag. Required for grid meshing
Must be used together with [--resx](#resx)

#### `--resz` {#resz}
**Type:** Value (flag)
**Description:** Set z resolution

z resolution

#### `--poly` {#poly}
**Type:** Switch (flag)
**Description:** Set generic polygon mesh for 2D meshing

Enable set generic polygon mesh for 2d meshing

#### `--subset` {#subset}
**Type:** Value (flag)
**Description:** Set (random) subset of points

(random) subset of points

#### `--meth` {#meth}
**Type:** Value (flag)
**Description:** Set method for z values

method for z values

#### `--setz` {#setz}
**Type:** Value (flag)
**Description:** Set const z values for new points

const z values for new points

#### `-O`, `--offset` {#offset}
**Type:** Switch (flag)
**Description:** Load polygon mesh and apply offset

Enable load polygon mesh and apply offset

#### `--delta` {#delta}
**Type:** Switch (flag)
**Description:** Set DELTA offset

Enable set delta offset

#### `--abs` {#abs}
**Type:** Switch (flag)
**Description:** Set ABSOLUTE ELEVATION offset

Enable set absolute elevation offset

#### `-z`, `--zoffset` {#zoffset}
**Type:** Value (flag)
**Description:** Set offset in Z direction

offset in z direction

#### `-A`, `--append` {#append}
**Type:** Switch (flag)
**Description:** Append meshes

Enable append meshes

#### `-T`, `--triobj` {#triobj}
**Type:** Switch (flag)
**Description:** Load trimeshes and create an object closed by surface meshes

Flag to load trimeshes and create an object closed by surface meshes

#### `-Q`, `--quadobj` {#quadobj}
**Type:** Switch (flag)
**Description:** Load quadmeshes and create an object closed by surface meshes

Flag to load quadmeshes and create an object closed by surface meshes

#### `--clean` {#clean}
**Type:** Switch (flag)
**Description:** Clean quadrilateral mesh from isolated polys

Enable clean quadrilateral mesh from isolated polys

#### `-M`, `--volmesh` {#volmesh}
**Type:** Switch (flag)
**Description:** Load polygonal mesh and create polyedral mesh

Flag to load polygonal mesh and create polyedral mesh

#### `--tet` {#tet}
**Type:** Switch (flag)
**Description:** Set tetrahedralization

Enable set tetrahedralization

#### `--vox` {#vox}
**Type:** Switch (flag)
**Description:** Set voxel as polyedralmesh

Enable set voxel as polyedralmesh

#### `--hex` {#hex}
**Type:** Switch (flag)
**Description:** Set hexmesh as polyedral

Enable set hexmesh as polyedral

#### `--nmaxvox` {#nmaxvox}
**Type:** Value (flag)
**Description:** Set n max voxel per side

Number of set n max voxel per side

#### `-L`, `--trimesh` {#trimesh}
**Type:** Switch (flag)
**Description:** Load trimesh file

Enable load trimesh file

#### `--splmet` {#splmet}
**Type:** Value (flag)
**Description:** Set polys split method

polys split method

#### `--remesh` {#remesh}
**Type:** Switch (flag)
**Description:** Set remeshing

Enable set remeshing

#### `--mark` {#mark}
**Type:** Switch (flag)
**Description:** Set marked boundary edges for remeshing

Enable set marked boundary edges for remeshing

#### `--collapse` {#collapse}
**Type:** Switch (flag)
**Description:** Set collapse on edge to simplify mesh boundary

Enable set collapse on edge to simplify mesh boundary

#### `--extractbp` {#extractbp}
**Type:** Switch (flag)
**Description:** Set extract boundary points

Enable set extract boundary points

#### `--it` {#it}
**Type:** Value (flag)
**Description:** Set number of iterations

Number of set number of iterations

#### `--scale` {#scale}
**Type:** Switch (flag)
**Description:** Set scale mesh

Enable set scale mesh

#### `--sx` {#sx}
**Type:** Value (flag)
**Description:** Set scale factor in X direction

scale factor in x direction

#### `--sy` {#sy}
**Type:** Value (flag)
**Description:** Set scale factor in Y direction

scale factor in y direction

#### `--sz` {#sz}
**Type:** Value (flag)
**Description:** Set scale factor in Z direction

scale factor in z direction

#### `-Z`, `--tetmesh` {#tetmesh}
**Type:** Switch (flag)
**Description:** Load tetmesh file

Enable load tetmesh file

#### `--surf` {#surf}
**Type:** Switch (flag)
**Description:** Extract surface from volume

Enable extract surface from volume

#### `--obj` {#obj}
**Type:** Switch (flag)
**Description:** Saving mesh in obj format

Enable saving mesh in obj format

#### `--vtk` {#vtk}
**Type:** Switch (flag)
**Description:** Saving mesh in vtk format

Enable saving mesh in vtk format

#### `-U`, `--merge` {#merge}
**Type:** Switch (flag)
**Description:** Merge two trimesh

Enable merge two trimesh

#### `--proxthresh` {#proxthresh}
**Type:** Value (flag)
**Description:** Set proximaty threshold

proximaty threshold

#### `-S`, `--split` {#split}
**Type:** Switch (flag)
**Description:** Split two trimesh

Enable split two trimesh

#### `-F`, `--cscalar` {#cscalar}
**Type:** Switch (flag)
**Description:** Create scalar field from centroids configuration and real samples

Flag to create scalar field from centroids configuration and real samples

#### `--smesh` {#smesh}
**Type:** Value (flag)
**Description:** Set samples mesh associated to (real) values

samples mesh associated to (real) values

#### `--sval` {#sval}
**Type:** Value (flag)
**Description:** Set samples values associated to each vertex of samples mesh

samples values associated to each vertex of samples mesh

#### `--rscalar` {#rscalar}
**Type:** Switch (flag)
**Description:** Restore scalar field from centroids configuration and real samples

Enable restore scalar field from centroids configuration and real samples

#### `-D`, `--res` {#res}
**Type:** Switch (flag)
**Description:** Set multiresolution

Enable set multiresolution

#### `-f`, `--file` {#file}
**Type:** Value (flag)
**Description:** Set scalar field file

Path to set scalar field file

#### `--refmod` {#refmod}
**Type:** Value (flag)
**Description:** Geometry model

geometry model

#### `--outf` {#outf}
**Type:** Value (flag)
**Description:** Set folder to save outputs

Path to set folder to save outputs

---

