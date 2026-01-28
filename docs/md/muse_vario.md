## muse_vario

**Description:** MUSE = Modelling of Uncertainty as a Support of Environment; Vario tool

### Usage

```bash
muse_vario [OPTIONS]
```

### Options

---

#### `-V`, `--variogram` {#variogram}
**Type:** Switch (flag)
**Description:** Compute variogram

Flag to compute variogram

**Dependencies:** MANDATORY when computing variograms. Requires:
- [--pdir](#pdir): Project directory (mandatory)
- [--var](#var): Variable name (mandatory)
OPTIONAL but commonly used:
- [--vario](#vario): Variogram type (EXPERIMENTAL, MODEL)
- [--dir](#dir): Direction type (OMNI, DIR)
- [--dim](#dim): Dimension (3D, 2D, 1Dz, etc.)

**Example:** `muse_vario -V --pdir /path/to/project --var temperature --vario EXPERIMENTAL --dir OMNI --dim 3D`

---

#### `-p`, `--pdir` {#pdir}
**Type:** Value (flag)
**Description:** Project directory

Path to project directory

---

#### `-v`, `--var` {#var}
**Type:** Value (flag)
**Description:** Variable

Name of variable

**Dependencies:** MANDATORY when using -V/[--variogram](#variogram) flag
The variable must exist in the project data

---

#### `--sub` {#sub}
**Type:** Value (flag)
**Description:** Set extracted sub-dataset referring to specified geometry domain

extracted sub-dataset referring to specified geometry domain

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

#### `--nscore` {#nscore}
**Type:** Value (flag)
**Description:** Set normal score transformation

Number of set normal score transformation

---

#### `--decl` {#decl}
**Type:** Switch (flag)
**Description:** Set 2D declustering

Enable set 2d declustering

**Dependencies:** When using declustering, requires:
- [--csize](#csize): Cell size for declustering (mandatory)
- [--nstep](#nstep): Number of grid translation steps (mandatory)

**Example:** `--decl --csize 100 --nstep 5`

---

#### `--csize` {#csize}
**Type:** Value (flag)
**Description:** Set cell size for 2D declustering

cell size for 2d declustering

**Dependencies:** Used together with [--decl](#decl) flag for 2D declustering

---

#### `--nstep` {#nstep}
**Type:** Value (flag)
**Description:** Set n steps for 2D declustering (grid translation)

Number of set n steps for 2d declustering (grid translation)

**Dependencies:** Used together with [--decl](#decl) flag for 2D declustering

---

#### `--sttype` {#sttype}
**Type:** Value (flag)
**Description:** Set stratigraphic condition for coordinate transformation

stratigraphic condition for coordinate transformation

**Dependencies:** When using stratigraphic transformation, requires:
- [--filestrat](#filestrat): Filename of samples in stratigraphic coordinates (mandatory)
Available conditions: PROPORTIONAL, TRUNCATION, ONLAP, COMBINATION

**Example:** `--sttype PROPORTIONAL --filestrat /path/to/strat_coords.dat`

---

#### `-f`, `--filestrat` {#filestrat}
**Type:** Value (flag)
**Description:** Set filename of samples in stratigraphic coordinates

Path to set filename of samples in stratigraphic coordinates

**Dependencies:** Required when using [--sttype](#sttype) flag for stratigraphic transformation

---

#### `--vario` {#vario}
**Type:** Value (flag)
**Description:** type of variogram

Name of type of variogram

---

#### `--dir` {#dir}
**Type:** Value (flag)
**Description:** type of variogram direction

Path to type of variogram direction

**Dependencies:** When using DIR (directional), directional parameters become important:
- [--deg](#deg): Degree step (default: 45°)
- [--degtol](#degtol): Tolerance (default: 45°)
- [--zdegtol](#zdegtol): Vertical tolerance (default: 22.5°)
- [--bandw](#bandw): Bandwidth (optional)
- [--vertbandw](#vertbandw): Vertical bandwidth (optional)

**Example:** `--dir DIR --deg 30 --degtol 15 --zdegtol 10`

---

#### `--dim` {#dim}
**Type:** Value (flag)
**Description:** type of variogram dimension

type of variogram dimension

---

#### `--lagspac` {#lagspac}
**Type:** Value (flag)
**Description:** Set lag spacing type

lag spacing type

---

#### `--spac` {#spac}
**Type:** Value (flag)
**Description:** Set spacing samples for vertical variogram

spacing samples for vertical variogram

---

#### `--deg` {#deg}
**Type:** Value (flag)
**Description:** Set degree step (in degree)

degree step (in degree)

**Dependencies:** Used with [--dir](#dir) DIR for directional variogram computation
Works together with [--degtol](#degtol) and [--zdegtol](#zdegtol)

---

#### `--degtol` {#degtol}
**Type:** Value (flag)
**Description:** Set tolerance (in degree)

tolerance (in degree)

**Dependencies:** Used with [--dir](#dir) DIR for directional variogram computation
Works together with [--deg](#deg) and [--zdegtol](#zdegtol) for direction tolerance

---

#### `--zdegtol` {#zdegtol}
**Type:** Value (flag)
**Description:** Set vertical tolerance (in degree)

vertical tolerance (in degree)

**Dependencies:** Used with [--dir](#dir) DIR for directional variogram computation
Works together with [--deg](#deg) and [--degtol](#degtol) for vertical direction tolerance

---

#### `--dirs` {#dirs}
**Type:** Value (flag)
**Description:** Load discrete directions (in degree)

Path to load discrete directions (in degree)

---

#### `--bandw` {#bandw}
**Type:** Value (flag)
**Description:** Set bandwidth

bandwidth

**Dependencies:** Used with [--dir](#dir) DIR for directional variogram computation
Often used together with [--vertbandw](#vertbandw) for 3D directional analysis

---

#### `--vertbandw` {#vertbandw}
**Type:** Value (flag)
**Description:** Set vertical bandwidth

vertical bandwidth

**Dependencies:** Used with [--dir](#dir) DIR for directional variogram computation
Often used together with [--bandw](#bandw) for 3D directional analysis

---

#### `--type` {#type}
**Type:** Value (flag)
**Description:** Set type of model variogram

type of model variogram

**Dependencies:** When using [--vario](#vario) MODEL, this flag becomes important
For model fitting, use together with:
- [--nugget](#nugget): Nugget value (optional)
- [--sill](#sill): Sill value (optional, not fully enabled)
Available models: AUTO, SPHERICAL, GAUSSIAN, EXPONENTIAL, LINEAR, DEFAULT

**Example:** `--vario MODEL --type SPHERICAL --nugget 0.1`

---

#### `--nugget` {#nugget}
**Type:** Value (flag)
**Description:** Nugget

Number of nugget

**Dependencies:** Used with [--vario](#vario) MODEL and [--type](#type) for model fitting
Often used together with [--sill](#sill) for complete model specification

---

#### `--sill` {#sill}
**Type:** Value (flag)
**Description:** Sill (NOT ENABLE)

sill (not enable)

**Dependencies:** Used with [--vario](#vario) MODEL and [--type](#type) for model fitting
Often used together with [--nugget](#nugget) for complete model specification
WARNING: This parameter is not fully enabled in current version

---

#### `--npoints` {#npoints}
**Type:** Value (flag)
**Description:** Set number of experimental variogram points

Number of set number of experimental variogram points

---

#### `--vclean` {#vclean}
**Type:** Value (flag)
**Description:** Set cleaning variogram points

cleaning variogram points

---

#### `--rangestep` {#rangestep}
**Type:** Value (flag)
**Description:** Set range step

range step

---

#### `--nugstep` {#nugstep}
**Type:** Value (flag)
**Description:** Set nugget step

Number of set nugget step

---

#### `--maxdist` {#maxdist}
**Type:** Value (flag)
**Description:** Set maximum distance between points for computing experimental variogram

maximum distance between points for computing experimental variogram

---

#### `--tolfac` {#tolfac}
**Type:** Value (flag)
**Description:** Set tolerance factor for computing experimental variogram

tolerance factor for computing experimental variogram

---

#### `--fac` {#fac}
**Type:** Value (flag)
**Description:** Set multiplier factor for computing variable lag spacing for experimental variogram

multiplier factor for computing variable lag spacing for experimental variogram

---

#### `--weight` {#weight}
**Type:** Switch (flag)
**Description:** Set weight on nugget to compute directional variogram

Flag to set weight on nugget to compute directional variogram

---

#### `--expvario` {#expvario}
**Type:** Value (flag)
**Description:** Load experimental variogram

Name of load experimental variogram

---

#### `--eps` {#eps}
**Type:** Value (flag)
**Description:** Set eps for plot centering

eps for plot centering

---

#### `--epsy` {#epsy}
**Type:** Value (flag)
**Description:** Set eps_y for plot on y axis

eps_y for plot on y axis

---

