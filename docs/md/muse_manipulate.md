## muse_manipulate

**Description:** MUSE = Modelling of Uncertainty as a Support of Environment; Manipulate tool

### Usage

```bash
muse_manipulate [OPTIONS]
```

### Options Index

- [`-E`, `--extract`](#extract) — Set extraction data based on geometry model
- [`-p`, `--pdir`](#pdir) — Set project directory
- [`--geom`](#geom) — Set geometry model
- [`-z`, `--zcoord`](#zcoord) — Set coordinate Z
- [`-I`, `--intextr`](#intextr) — Set extraction data from interval
- [`--sup`](#sup) — Set sup interval
- [`--inf`](#inf) — Set inf interval
- [`--nvar`](#nvar) — Set variable to check
- [`--sub`](#sub) — Set sub dataset extraction based on geometry
- [`--rotaxis`](#rotaxis) — Set rotation axis
- [`--rotangle`](#rotangle) — Set rotation angle (clockwise)
- [`--rotcx`](#rotcx) — Set rotation center x
- [`--rotcy`](#rotcy) — Set rotation center y
- [`--rotcz`](#rotcz) — Set rotation center z
- [`-P`, `--prsurf`](#prsurf) — Points projection on surfaces
- [`-S`, `--prsect`](#prsect) — Compute points projection on top/bottom boundary (2D section case).
- [`-R`, `--prqsect`](#prqsect) — Points projection on quads sections
- [`-V`, `--prvol`](#prvol) — Compute points projection on boundary (3D volumetric case).
- [`--step`](#step) — Set number of steps for geometry model
- [`--epsilon`](#epsilon) — Set tolerance to enlarge bounding box
- [`-m`, `--mgeom`](#mgeom) — Multi-geometry to pass
- [`--prdir`](#prdir) — Set direction of projection
- [`--type`](#type) — Set type
- [`-T`, `--strat`](#strat) — Points projection on surfaces
- [`--name`](#name) — Name of geometry model
- [`--sttype`](#sttype) — Set type of stratigraphic transformation
- [`--top`](#top) — Top geometry model
- [`--bot`](#bot) — Bottom geometry model
- [`--reggrow`](#reggrow) — Set region growing
- [`--obj`](#obj) — Saving trimesh in obj format
- [`--vtk`](#vtk) — Saving tetmesh in vtk format
- [`--save`](#save) — Saving extraction as set of points
- [`-v`, `--var`](#var) — Variable
- [`--file`](#file) — Set path to data file

### Options

---

#### `-E`, `--extract` {#extract}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Extraction data basing on geometry model. This option allows to extract data from a geometry model, such as a surface or volume mesh, based on specified criteria. The extracted data can be used for further analysis or visualization.

**Default:** `false (extraction is disabled by default).`

**Dependencies:** When using -E/[--extract](#extract), requires:
- [--geom](#geom): Geometry model to extract from (mandatory)
- [--zcoord](#zcoord): Coordinate Z to use for extraction (optional, used for 3D models)

**Example:** `muse_manipulate -E -p /path/to/project --geom model.obj`

---

#### `-p`, `--pdir` {#pdir}

<div class="required"><strong>⚠ Required:</strong> true (this parameter is mandatory for running the application).</div>

**Type:** Value | `std::string`

**Format:** `string (path to the project directory)`

**Description:** Set project directory. This is the main directory where the project files are located. It is a required argument for running the application, as it specifies the context in which the manipulation will occur. The project directory should contain all necessary data and geometry files for the operations to be performed.

**Default:** `"path/to/project" (placeholder value, should be replaced with an actual path).`

**Dependencies:** The project directory should be organized according to the expected structure of the MUSE project, with subdirectories for data, geometry, and output as needed.

**Example:** `muse_manipulate -E -p /path/to/project --geom model.obj`

---

#### `--geom` {#geom}

**Type:** Value | `std::string`

**Format:** `string (geometry model name)`

**Description:** Set geometry model. This argument specifies the geometry model to be used for data extraction when the -E/--extract flag is enabled. The geometry model can be a surface mesh, volume mesh, or any other supported geometric representation. The specified model must exist within the project directory and will be used as the basis for extracting data according to the defined criteria.

**Default:** `"geometry-name" (placeholder value, should be replaced with an actual geometry model name).`

**Dependencies:** Required when using -E/[--extract](#extract).

**Example:** `muse_manipulate -E -p /path/to/project --geom model.obj`

---

#### `-z`, `--zcoord` {#zcoord}

**Type:** Value | `std::string`

**Format:** `string (name of the variable used as Z coordinate)`

**Description:** Set coordinate Z. This argument specifies the coordinate Z to be used for data extraction when the -E/--extract flag is enabled. It is optional and used for 3D models.

**Default:** `"z_name" (placeholder value, should be replaced with an actual variable name).`

**Dependencies:** Optional. Used with -E/[--extract](#extract) for 3D models. ⚠ Partially implemented: only the PRESENCE of -z/[--zcoord](#zcoord) is currently used; the passed value is not read (the Z variable name is taken from the project metadata), so the flag currently behaves as a boolean switch.

**Example:** `muse_manipulate -E -p /path/to/project --geom model.obj --zcoord elevation`

---

#### `-I`, `--intextr` {#intextr}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set extraction data from interval. Extracts the subset of samples whose value of a given variable falls within the [inf, sup] interval.

**Default:** `false (interval extraction is disabled by default).`

**Dependencies:** When using -I/[--intextr](#intextr), requires:
- [--sup](#sup): Superior interval limit (mandatory)
- [--inf](#inf): Inferior interval limit (mandatory)
- [--nvar](#nvar): Variable name to check (mandatory)

**Example:** `muse_manipulate -I -p /path/to/project --sup 100 --inf 0 --nvar temperature`

---

#### `--sup` {#sup}

**Type:** Value | `int`

**Format:** `int`

**Description:** Set sup interval. Superior limit of the interval used to select samples during interval extraction.

**Default:** `0`

**Dependencies:** Used with -I/[--intextr](#intextr) flag. Required for interval extraction. Must be used together with [--inf](#inf) and [--nvar](#nvar).

**Example:** `muse_manipulate -I -p /path/to/project --sup 100 --inf 0 --nvar temperature`

---

#### `--inf` {#inf}

**Type:** Value | `int`

**Format:** `int`

**Description:** Set inf interval. Inferior limit of the interval used to select samples during interval extraction.

**Default:** `0`

**Dependencies:** Used with -I/[--intextr](#intextr) flag. Required for interval extraction. Must be used together with [--sup](#sup) and [--nvar](#nvar).

**Example:** `muse_manipulate -I -p /path/to/project --sup 100 --inf 0 --nvar temperature`

---

#### `--nvar` {#nvar}

**Type:** Value | `std::string`

**Format:** `string (variable name)`

**Description:** Set variable to check. Name of the variable whose values are tested against the [inf, sup] interval.

**Default:** `"var_name" (placeholder value, should be replaced with an actual variable name).`

**Dependencies:** Used with -I/[--intextr](#intextr) flag. Required for interval extraction. Must be used together with [--sup](#sup) and [--inf](#inf).

**Example:** `muse_manipulate -I -p /path/to/project --sup 100 --inf 0 --nvar temperature`

---

#### `--sub` {#sub}

**Type:** Value | `std::string`

**Format:** `string (sub-dataset name)`

**Description:** Set sub dataset extraction based on geometry. Name of the sub-dataset produced by extracting the samples contained in the given geometry.

**Default:** `"name" (placeholder value, should be replaced with an actual sub-dataset name).`

**Dependencies:** Used with -E/[--extract](#extract).

**Example:** `muse_manipulate -E -p /path/to/project --geom model.obj --sub my_subset`

---

#### `--rotaxis` {#rotaxis}

**Type:** Value | `std::string`

**Format:** `string` (`X`, `Y`, `Z`, `NO`)

**Description:** Set rotation axis. Axis around which the geometry/points are rotated; NO disables the rotation.

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

**Description:** Set rotation angle (clockwise), expressed in degrees.

**Default:** `0.0`

**Dependencies:** Used together with [--rotaxis](#rotaxis) flag. Required when rotaxis != NO.

**Example:** `muse_manipulate -E -p /path/to/project --rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0`

---

#### `--rotcx` {#rotcx}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set rotation center x. X coordinate of the rotation center.

**Default:** `0.0`

**Dependencies:** Used together with [--rotaxis](#rotaxis) and [--rotangle](#rotangle).

**Example:** `muse_manipulate -E -p /path/to/project --rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0`

---

#### `--rotcy` {#rotcy}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set rotation center y. Y coordinate of the rotation center.

**Default:** `0.0`

**Dependencies:** Used together with [--rotaxis](#rotaxis) and [--rotangle](#rotangle).

**Example:** `muse_manipulate -E -p /path/to/project --rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0`

---

#### `--rotcz` {#rotcz}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set rotation center z. Z coordinate of the rotation center.

**Default:** `0.0`

**Dependencies:** Used together with [--rotaxis](#rotaxis) and [--rotangle](#rotangle).

**Example:** `muse_manipulate -E -p /path/to/project --rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0`

---

#### `-P`, `--prsurf` {#prsurf}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Points projection on surfaces. Projects the input points onto one or more surface meshes.

**Default:** `false (surface projection is disabled by default).`

**Dependencies:** Projection mode selection (mutually exclusive):
- -P/[--prsurf](#prsurf): Surface projection
- -S/[--prsect](#prsect): Section projection (2D)
- -R/[--prqsect](#prqsect): Quad section projection
Choose only ONE projection mode per operation

---

#### `-S`, `--prsect` {#prsect}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Compute points projection on top/bottom boundary (2D section case).

**Default:** `false (section projection is disabled by default).`

**Dependencies:** Mutually exclusive with -P/[--prsurf](#prsurf) and -R/[--prqsect](#prqsect). Use for 2D section projection operations.

---

#### `-R`, `--prqsect` {#prqsect}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Points projection on quads sections. Projects the input points onto quad-based section meshes.

**Default:** `false (quad-section projection is disabled by default).`

**Dependencies:** Mutually exclusive with -P/[--prsurf](#prsurf) and -S/[--prsect](#prsect). Use for quad-based section projection operations.

---

#### `-V`, `--prvol` {#prvol}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Compute points projection on boundary (3D volumetric case). Projects the input points onto the boundary of a volumetric mesh.

**Default:** `false (volumetric projection is disabled by default).`

**Dependencies:** Projection mode for the 3D volumetric case.

---

#### `--step` {#step}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set number of steps for geometry model. Discretization step used when building/scanning the geometry during projection.

**Default:** `0.0 (the step is chosen automatically).`

**Dependencies:** Used with the projection operations (-P/-S/-R/-V).

**Example:** `muse_manipulate -P -p /path/to/project -m surf.obj --step 1.0`

---

#### `--epsilon` {#epsilon}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set tolerance to enlarge bounding box. Epsilon used to expand the bounding box so that boundary points are not discarded during projection.

**Default:** `1.0`

**Dependencies:** Used with the projection operations (-P/-S/-R/-V).

**Example:** `muse_manipulate -P -p /path/to/project -m surf.obj --epsilon 0.5`

---

#### `-m`, `--mgeom` {#mgeom}

**Type:** Value | `std::string`, repeatable

**Format:** `string (path/name of a geometry file); repeatable`

**Description:** Multi-geometry to pass. One or more geometry (mesh) files used as targets for the projection operations. Repeat the flag to pass several geometries.

**Default:** `empty (no geometry is passed when the flag is not used).`

**Dependencies:** Used with the projection operations (-P/-S/-R/-V).

**Example:** `muse_manipulate -P -p /path/to/project -m top.obj -m bottom.obj`

---

#### `--prdir` {#prdir}

**Type:** Value | `std::string`

**Format:** `string` (`X`, `Y`, `Z`)

**Description:** Set direction of projection. Axis along which the points are projected onto the target geometry.

**Default:** `"Y" (projection along the Y axis).`

**Dependencies:** Used with the projection operations (-P/-S/-R/-V).

**Example:** `muse_manipulate -P -p /path/to/project -m surf.obj --prdir Z`

---

#### `--type` {#type}

**Type:** Value | `std::string`

**Format:** `string` (`SAMPLES`, `TET`, `HEX`, `VOLUME`, `GEOMETRY`, `QUADMESH`)

**Description:** Set type of the geometry/data handled by the operation (kind of mesh or sample set to process).

**Default:** `"SAMPLES" (operate on the point samples).`

**Dependencies:** Selects how the input is interpreted during extraction/projection.

**Example:** `muse_manipulate -E -p /path/to/project --geom model.vtk --type TET`

---

#### `-T`, `--strat` {#strat}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Stratigraphic coordinate transformation. Maps the points into a stratigraphic reference frame defined by a top and a bottom surface.

**Default:** `false (stratigraphic transformation is disabled by default).`

**Dependencies:** When using -T/[--strat](#strat), requires [--top](#top), [--bot](#bot) and [--sttype](#sttype) (stratigraphic condition).

**Example:** `muse_manipulate -T -p /path/to/project --top top.obj --bot bot.obj --sttype PROPORTIONAL`

---

#### `--name` {#name}

**Type:** Value | `std::string`

**Format:** `string (geometry model name)`

**Description:** Name of geometry model. Name assigned to the geometry produced/handled by the operation.

**Default:** `"name" (placeholder value, should be replaced with an actual geometry model name).`

**Example:** `muse_manipulate -T -p /path/to/project --name strat_grid --top top.obj --bot bot.obj`

---

#### `--sttype` {#sttype}

**Type:** Value | `std::string`

**Format:** `string` (`PROPORTIONAL`, `TRUNCATION`, `ONLAP`, `COMBINATION`)

**Description:** Set type of stratigraphic transformation (the stratigraphic condition applied between the top and bottom surfaces).

**Default:** `"NO" (no stratigraphic condition).`

**Dependencies:** Used with -T/[--strat](#strat).

**Example:** `muse_manipulate -T -p /path/to/project --top top.obj --bot bot.obj --sttype PROPORTIONAL`

---

#### `--top` {#top}

**Type:** Value | `std::string`

**Format:** `string (geometry model name)`

**Description:** Top geometry model. Name of the surface used as the top boundary of the stratigraphic transformation.

**Default:** `"name top geometry" (placeholder value, should be replaced with an actual geometry model name).`

**Dependencies:** Used with -T/[--strat](#strat).

**Example:** `muse_manipulate -T -p /path/to/project --top top.obj --bot bot.obj --sttype PROPORTIONAL`

---

#### `--bot` {#bot}

**Type:** Value | `std::string`

**Format:** `string (geometry model name)`

**Description:** Bottom geometry model. Name of the surface used as the bottom boundary of the stratigraphic transformation.

**Default:** `"name bottom geometry" (placeholder value, should be replaced with an actual geometry model name).`

**Dependencies:** Used with -T/[--strat](#strat).

**Example:** `muse_manipulate -T -p /path/to/project --top top.obj --bot bot.obj --sttype PROPORTIONAL`

---

#### `--reggrow` {#reggrow}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set region growing. Enables a region-growing step to segment/label the geometry during the operation.

**Default:** `false (region growing is disabled by default).`

**Example:** `muse_manipulate -E -p /path/to/project --geom model.vtk --reggrow`

---

#### `--obj` {#obj}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Saving trimesh in obj format. Writes the resulting triangular mesh as an OBJ file.

**Default:** `false (OBJ output is disabled by default).`

**Dependencies:** Optional output format.

**Example:** `muse_manipulate -E -p /path/to/project --geom model.obj --obj`

---

#### `--vtk` {#vtk}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Saving tetmesh in vtk format. Writes the resulting tetrahedral mesh as a VTK file.

**Default:** `false (VTK output is disabled by default).`

**Dependencies:** Optional output format.

**Example:** `muse_manipulate -E -p /path/to/project --geom model.vtk --type TET --vtk`

---

#### `--save` {#save}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Saving extraction as set of points. Writes the extracted/projected result as a point set.

**Default:** `false (the extraction is not saved by default).`

**Dependencies:** Optional output. Typically used with -E/[--extract](#extract).

**Example:** `muse_manipulate -E -p /path/to/project --geom model.obj --save`

---

#### `-v`, `--var` {#var}

**Type:** Value | `std::string`

**Format:** `string (variable name)`

**Description:** Variable. Name of the variable to analyse/carry along during the manipulation.

**Default:** `"variable to analyse" (placeholder value, should be replaced with an actual variable name).`

**Example:** `muse_manipulate -E -p /path/to/project --geom model.obj -v temperature`

---

#### `--file` {#file}

**Type:** Value | `std::string`

**Format:** `string (path to a data file)`

**Description:** Set path to data file. This argument allows the user to specify the path to a data file that will be used in the manipulation process. The data file should contain relevant information that the application can read and process. It is optional.

**Default:** `"string" (placeholder value; when not set the default project directory structure is used).`

**Dependencies:** If not set, the application will look for data files in the default project directory structure. If set, the specified file will be used for data manipulation.

**Example:** `muse_manipulate -E -p /path/to/project --file path/to/datafile.dat`

---

