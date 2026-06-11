## muse_manipulate

**Description:** MUSE = Modelling of Uncertainty as a Support of Environment; Manipulate tool

### Usage

```bash
muse_manipulate [OPTIONS]
```

### Options

---

#### `-E`, `--extract` {#extract}
**Type:** Switch (flag)

**Description:** Extraction data

Enable extraction data

**Dependencies:** When using -E/[--extract](#extract), requires:
- [--geom](#geom): Geometry model (mandatory)
OPTIONAL:
- [--zcoord](#zcoord): Z coordinate specification
- [--sub](#sub): Sub-dataset extraction

**Example:** `-E --geom mesh_model --zcoord elevation`

---

#### `-p`, `--pdir` {#pdir}
**Type:** Value (flag)

**Description:** Project directory

Path to project directory

---

#### `--geom` {#geom}
**Type:** Value (flag)

**Description:** Geometry model

geometry model

**Dependencies:** Required when using -E/[--extract](#extract) flag
The geometry model must exist in the project

---

#### `-z`, `--zcoord` {#zcoord}
**Type:** Value (flag)

**Description:** Coordinate Z

coordinate z

---

#### `-I`, `--intextr` {#intextr}
**Type:** Switch (flag)

**Description:** Extraction data from interval

Enable extraction data from interval

**Dependencies:** When using -I/[--intextr](#intextr), requires:
- [--sup](#sup): Superior interval limit (mandatory)
- [--inf](#inf): Inferior interval limit (mandatory)
- [--nvar](#nvar): Variable name to check (mandatory)

**Example:** `-I --sup 100 --inf 0 --nvar temperature`

---

#### `--sup` {#sup}
**Type:** Value (flag)

**Description:** Set sup interval

sup interval

**Dependencies:** Used with -I/[--intextr](#intextr) flag. Required for interval extraction
Must be used together with [--inf](#inf) and [--nvar](#nvar)

---

#### `--inf` {#inf}
**Type:** Value (flag)

**Description:** Set inf interval

inf interval

**Dependencies:** Used with -I/[--intextr](#intextr) flag. Required for interval extraction
Must be used together with [--sup](#sup) and [--nvar](#nvar)

---

#### `--nvar` {#nvar}
**Type:** Value (flag)

**Description:** Set variable to check

Name of set variable to check

**Dependencies:** Used with -I/[--intextr](#intextr) flag. Required for interval extraction
Must be used together with [--sup](#sup) and [--inf](#inf)

---

#### `--sub` {#sub}
**Type:** Value (flag)

**Description:** Extraction sub dataset basing on geometry

extraction sub dataset basing on geometry

---

#### `--rotaxis` {#rotaxis}
**Type:** Value (flag)

**Description:** Set rotation axis

rotation axis

**Dependencies:** When using rotation, these flags work together:
- [--rotaxis](#rotaxis): Rotation axis (X, Y, Z)
- [--rotangle](#rotangle): Rotation angle (required if rotaxis != NO)
- [--rotcx](#rotcx), [--rotcy](#rotcy), [--rotcz](#rotcz): Rotation center coordinates

**Example:** `For Z-axis rotation: --rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0`

---

#### `--rotangle` {#rotangle}
**Type:** Value (flag)

**Description:** Set rotation angle (clockwise)

rotation angle (clockwise)

**Dependencies:** Used together with [--rotaxis](#rotaxis) flag. Required when rotaxis != NO

---

#### `--rotcx` {#rotcx}
**Type:** Value (flag)

**Description:** Set rotation center x

rotation center x

---

#### `--rotcy` {#rotcy}
**Type:** Value (flag)

**Description:** Set rotation center y

rotation center y

---

#### `--rotcz` {#rotcz}
**Type:** Value (flag)

**Description:** Set rotation center z

rotation center z

---

#### `-P`, `--prsurf` {#prsurf}
**Type:** Switch (flag)

**Description:** Points projection on surfaces

Enable points projection on surfaces

**Dependencies:** Projection mode selection (mutually exclusive):
- -P/[--prsurf](#prsurf): Surface projection
- -S/[--prsect](#prsect): Section projection (2D)
- -R/[--prqsect](#prqsect): Quad section projection
Choose only ONE projection mode per operation

---

#### `-S`, `--prsect` {#prsect}
**Type:** Switch (flag)

**Description:** Compute points projection on boundary (2D section case).

Flag to compute points projection on boundary (2d section case).

**Dependencies:** Mutually exclusive with -P/[--prsurf](#prsurf) and -R/[--prqsect](#prqsect)
Use for 2D section projection operations

---

#### `-R`, `--prqsect` {#prqsect}
**Type:** Switch (flag)

**Description:** Points projection on quads sections

Enable points projection on quads sections

**Dependencies:** Mutually exclusive with -P/[--prsurf](#prsurf) and -S/[--prsect](#prsect)
Use for quad-based section projection operations

---

#### `-V`, `--prvol` {#prvol}
**Type:** Switch (flag)

**Description:** Compute points projection on boundary (3D volumetric case).

Flag to compute points projection on boundary (3d volumetric case).

---

#### `--step` {#step}
**Type:** Value (flag)

**Description:** Set number of steps for geometry model

Number of set number of steps for geometry model

---

#### `--epsilon` {#epsilon}
**Type:** Value (flag)

**Description:** Set tolerance to enlarge bounding box

tolerance to enlarge bounding box

---

#### `--prdir` {#prdir}
**Type:** Value (flag)

**Description:** Set direction of projection

Path to set direction of projection

---

#### `--type` {#type}
**Type:** Value (flag)

**Description:** Set type

type

---

#### `-T`, `--strat` {#strat}
**Type:** Switch (flag)

**Description:** Points projection on surfaces

Enable points projection on surfaces

---

#### `--name` {#name}
**Type:** Value (flag)

**Description:** Name of geometry model

Name of name of geometry model

---

#### `--sttype` {#sttype}
**Type:** Value (flag)

**Description:** Set type of stratigraphic transformation

type of stratigraphic transformation

---

#### `--top` {#top}
**Type:** Value (flag)

**Description:** Top geometry model

top geometry model

---

#### `--bot` {#bot}
**Type:** Value (flag)

**Description:** Bottom geometry model

bottom geometry model

---

#### `--reggrow` {#reggrow}
**Type:** Switch (flag)

**Description:** Set region growing

Enable set region growing

---

#### `--obj` {#obj}
**Type:** Switch (flag)

**Description:** Saving trimesh in obj format

Enable saving trimesh in obj format

---

#### `--vtk` {#vtk}
**Type:** Switch (flag)

**Description:** Saving tetmesh in vtk format

Enable saving tetmesh in vtk format

---

#### `--save` {#save}
**Type:** Switch (flag)

**Description:** Saving extraction as set of points

Enable saving extraction as set of points

---

#### `-v`, `--var` {#var}
**Type:** Value (flag)

**Description:** Variable

Name of variable

---

#### `--file` {#file}
**Type:** Value (flag)

**Description:** Path file

Path to path file

---

