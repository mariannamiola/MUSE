## muse_compute

**Description:** MUSE = Modelling of Uncertainty as a Support of Environment; Compute tool

### Usage

```bash
muse_compute [OPTIONS]
```

### Options

---

#### `-C`, `--compute` {#compute}
**Type:** Switch (flag)
**Description:** Enable computation mode for MUSE

Flag to enable computation mode

**Dependencies:** When using -C/[--compute](#compute), these flags work together:
REQUIRED flags:
- [--var](#var): Variable name is mandatory
- [--geom](#geom): Geometry model is mandatory
OPTIONAL but commonly used:
- [--mode](#mode): Computation mode (AUTO, MANUAL)
- [--pdir](#pdir): Project directory
- [--sub](#sub): Sub-dataset extraction

**Example:** `muse_compute -C --var temperature --geom mesh_model --mode AUTO`

---

#### `--mode` {#mode}
**Type:** Value (flag)
**Description:** Set computation mode

Computation mode setting (default: AUTO)

**Dependencies:** Used in combination with -C/[--compute](#compute) flag
Available modes: AUTO, MANUAL
- AUTO: Automatic parameter selection
- MANUAL: Manual parameter configuration required

---

#### `-p`, `--pdir` {#pdir}
**Type:** Value (flag)
**Description:** Specify project directory

Path to the project directory

---

#### `-v`, `--var` {#var}
**Type:** Value (flag)
**Description:** Specify variable name to analyze

Name of the variable to process

**Dependencies:** MANDATORY when using -C/[--compute](#compute) flag
The variable must exist in the project data

---

#### `-m`, `--geom` {#geom}
**Type:** Value (flag)
**Description:** Specify geometry model name

Name of the geometry model to use

**Dependencies:** MANDATORY when using -C/[--compute](#compute) flag
The geometry model must be available in the project

---

#### `--sub` {#sub}
**Type:** Value (flag)
**Description:** Extract sub-dataset based on geometry

Path to sub-dataset extraction directory

---

#### `--rotaxis` {#rotaxis}
**Type:** Value (flag)
**Description:** Set rotation axis for data transformation

Axis for rotation (default: NO)

**Dependencies:** When using rotation, these flags work together:
- [--rotaxis](#rotaxis): Rotation axis (X, Y, Z)
- [--rotangle](#rotangle): Rotation angle (required if rotaxis != NO)
- [--rotcx](#rotcx), [--rotcy](#rotcy), [--rotcz](#rotcz): Rotation center coordinates

**Example:** `--rotaxis Z --rotangle 45.0 --rotcx 0.0 --rotcy 0.0`

---

#### `--rotangle` {#rotangle}
**Type:** Value (flag)
**Description:** Set rotation angle in degrees (clockwise)

Rotation angle in degrees

**Dependencies:** Used together with [--rotaxis](#rotaxis) flag. Required when rotaxis != NO

---

#### `--rotcx` {#rotcx}
**Type:** Value (flag)
**Description:** Set X coordinate of rotation center

X coordinate of rotation center

**Dependencies:** Used together with [--rotaxis](#rotaxis) and [--rotangle](#rotangle) for data rotation

---

#### `--rotcy` {#rotcy}
**Type:** Value (flag)
**Description:** Set Y coordinate of rotation center

Y coordinate of rotation center

---

#### `--rotcz` {#rotcz}
**Type:** Value (flag)
**Description:** Set Z coordinate of rotation center

Z coordinate of rotation center

---

#### `--vario` {#vario}
**Type:** Value (flag)
**Description:** Set fixed variogram parameters

Path to variogram configuration file

---

#### `--dir` {#dir}
**Type:** Value (flag)
**Description:** Set variogram direction type

Type of variogram direction (OMNI or DIR)

---

#### `--dim` {#dim}
**Type:** Value (flag)
**Description:** Set variogram dimension type

Type of variogram dimension (3D, 3Dxy, 3Dz, 2D, 1Dz, 1D)

**Dependencies:** Variogram configuration flags work together:
- [--dir](#dir): Direction type (OMNI, DIR)
- [--dim](#dim): Dimension type
- [--zrange](#zrange): Z direction range (used with 3D dimensions)

**Example:** `--dir OMNI --dim 3D --zrange 50.0`

---

#### `--zrange` {#zrange}
**Type:** Value (flag)
**Description:** Set range in Z direction

Range value in Z direction

---

#### `--crit` {#crit}
**Type:** Value (flag)
**Description:** Set interpolation algorithm

Interpolation algorithm (SGS, IK, or SISIM)

---

#### `--bnscore` {#bnscore}
**Type:** Switch (flag)
**Description:** Enable back normal score transformation integrated into SGS

Flag to enable back normal score transformation

---

#### `--extr` {#extr}
**Type:** Value (flag)
**Description:** Set extrapolation type

Type of extrapolation (default: none)

**Dependencies:** When using extrapolation (extr != "none"), these flags work together:
- [--minextr](#minextr): Minimum extrapolation value
- [--maxextr](#maxextr): Maximum extrapolation value

**Example:** `--extr linear --minextr 0.0 --maxextr 100.0`

---

#### `--minextr` {#minextr}
**Type:** Value (flag)
**Description:** Set minimum value for extrapolation

Minimum extrapolation value

---

#### `--maxextr` {#maxextr}
**Type:** Value (flag)
**Description:** Set maximum value for extrapolation

Maximum extrapolation value

---

#### `--nsim` {#nsim}
**Type:** Value (flag)
**Description:** Set number of simulation iterations

Number of simulation iterations (default: 10)

---

#### `--csize` {#csize}
**Type:** Value (flag)
**Description:** Set cell size for 2D declustering

Cell size for 2D declustering

**Dependencies:** When using 2D declustering, these flags work together:
- [--csize](#csize): Cell size (required for declustering)
- [--nstep](#nstep): Number of steps for grid translation

**Example:** `--csize 10.0 --nstep 5`

---

#### `--nstep` {#nstep}
**Type:** Value (flag)
**Description:** Set number of steps for 2D declustering grid translation

Number of steps for 2D declustering

---

#### `-S`, `--stats` {#stats}
**Type:** Switch (flag)
**Description:** Enable statistical analysis on simulation results

Flag to compute statistical analysis

---

#### `--space` {#space}
**Type:** Value (flag)
**Description:** Set space type for analysis

Space type (NORMAL or VAR)

---

#### `-B`, `--bns` {#bns}
**Type:** Switch (flag)
**Description:** Enable back normal score transformation

Flag to perform back normal score transformation

---

#### `-f`, `--file` {#file}
**Type:** Value (flag)
**Description:** Set input file path

Path to input file

---

#### `-D`, `--db` {#db}
**Type:** Switch (flag)
**Description:** Enable database creation from simulations

Flag to create database from simulation results

---

#### `--input` {#input}
**Type:** Value (flag)
**Description:** Set number of input samples

Number of input samples to use (default: 4)

---

#### `--simulated` {#simulated}
**Type:** Value (flag)
**Description:** Set number of simulated points

Number of points to simulate (default: 3)

---

#### `--scaleradius` {#scaleradius}
**Type:** Value (flag)
**Description:** Set scale factor of search radius

Scale factor for search radius (default: 1.0)

---

#### `--octant` {#octant}
**Type:** Switch (flag)
**Description:** Enable octant search in SGS algorithm

Flag to enable octant search

---

#### `--csv` {#csv}
**Type:** Switch (flag)
**Description:** Enable CSV format for output files

Flag to save files in CSV format

---

#### `--out` {#out}
**Type:** Value (flag)
**Description:** Set type of SGS output

Type of SGS output (MEAN or VECSIM)

---

#### `--format` {#format}
**Type:** Value (flag)
**Description:** Set data format encoding for output

Enable data format encoding (default: YES)

---

