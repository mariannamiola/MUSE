## muse_compute

**Description:** MUSE = Modelling of Uncertainty as a Support of Environment; Compute tool

### Usage

```bash
muse_compute [OPTIONS]
```

### Options

#### `-C`, `--compute`
**Type:** Switch (flag)
**Description:** Enable computation mode for MUSE

Flag to enable computation mode

#### `--mode`
**Type:** Value (flag)
**Description:** Set computation mode

Computation mode setting (default: AUTO)

#### `-p`, `--pdir`
**Type:** Value (flag)
**Description:** Specify project directory

Path to the project directory

#### `-v`, `--var`
**Type:** Value (flag)
**Description:** Specify variable name to analyze

Name of the variable to process

#### `-m`, `--geom`
**Type:** Value (flag)
**Description:** Specify geometry model name

Name of the geometry model to use

#### `--sub`
**Type:** Value (flag)
**Description:** Extract sub-dataset based on geometry

Path to sub-dataset extraction directory

#### `--rotaxis`
**Type:** Value (flag)
**Description:** Set rotation axis for data transformation

Axis for rotation (default: NO)

#### `--rotangle`
**Type:** Value (flag)
**Description:** Set rotation angle in degrees (clockwise)

Rotation angle in degrees

#### `--rotcx`
**Type:** Value (flag)
**Description:** Set X coordinate of rotation center

X coordinate of rotation center

#### `--rotcy`
**Type:** Value (flag)
**Description:** Set Y coordinate of rotation center

Y coordinate of rotation center

#### `--rotcz`
**Type:** Value (flag)
**Description:** Set Z coordinate of rotation center

Z coordinate of rotation center

#### `--vario`
**Type:** Value (flag)
**Description:** Set fixed variogram parameters

Path to variogram configuration file

#### `--dir`
**Type:** Value (flag)
**Description:** Set variogram direction type

Type of variogram direction (OMNI or DIR)

#### `--dim`
**Type:** Value (flag)
**Description:** Set variogram dimension type

Type of variogram dimension (3D, 3Dxy, 3Dz, 2D, 1Dz, 1D)

#### `--zrange`
**Type:** Value (flag)
**Description:** Set range in Z direction

Range value in Z direction

#### `--crit`
**Type:** Value (flag)
**Description:** Set interpolation algorithm

Interpolation algorithm (SGS, IK, or SISIM)

#### `--bnscore`
**Type:** Switch (flag)
**Description:** Enable back normal score transformation integrated into SGS

Flag to enable back normal score transformation

#### `--extr`
**Type:** Value (flag)
**Description:** Set extrapolation type

Type of extrapolation (default: none)

#### `--minextr`
**Type:** Value (flag)
**Description:** Set minimum value for extrapolation

Minimum extrapolation value

#### `--maxextr`
**Type:** Value (flag)
**Description:** Set maximum value for extrapolation

Maximum extrapolation value

#### `--nsim`
**Type:** Value (flag)
**Description:** Set number of simulation iterations

Number of simulation iterations (default: 10)

#### `--csize`
**Type:** Value (flag)
**Description:** Set cell size for 2D declustering

Cell size for 2D declustering

#### `--nstep`
**Type:** Value (flag)
**Description:** Set number of steps for 2D declustering grid translation

Number of steps for 2D declustering

#### `-S`, `--stats`
**Type:** Switch (flag)
**Description:** Enable statistical analysis on simulation results

Flag to compute statistical analysis

#### `--space`
**Type:** Value (flag)
**Description:** Set space type for analysis

Space type (NORMAL or VAR)

#### `-B`, `--bns`
**Type:** Switch (flag)
**Description:** Enable back normal score transformation

Flag to perform back normal score transformation

#### `-f`, `--file`
**Type:** Value (flag)
**Description:** Set input file path

Path to input file

#### `-D`, `--db`
**Type:** Switch (flag)
**Description:** Enable database creation from simulations

Flag to create database from simulation results

#### `--input`
**Type:** Value (flag)
**Description:** Set number of input samples

Number of input samples to use (default: 4)

#### `--simulated`
**Type:** Value (flag)
**Description:** Set number of simulated points

Number of points to simulate (default: 3)

#### `--scaleradius`
**Type:** Value (flag)
**Description:** Set scale factor of search radius

Scale factor for search radius (default: 1.0)

#### `--octant`
**Type:** Switch (flag)
**Description:** Enable octant search in SGS algorithm

Flag to enable octant search

#### `--csv`
**Type:** Switch (flag)
**Description:** Enable CSV format for output files

Flag to save files in CSV format

#### `--out`
**Type:** Value (flag)
**Description:** Set type of SGS output

Type of SGS output (MEAN or VECSIM)

#### `--format`
**Type:** Value (flag)
**Description:** Set data format encoding for output

Enable data format encoding (default: YES)

---

