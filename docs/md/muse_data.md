## muse_data

**Description:** MUSE = Modelling of Uncertainty as a Support of Environment; Data tool

### Usage

```bash
muse_data [OPTIONS]
```

### Options

#### `-N`, `--new_project` {#new-project}
**Type:** Switch (flag)
**Description:** Create a new project

Flag to create a new project

#### `-p`, `--pdir` {#pdir}
**Type:** Value (flag)
**Description:** Specify project directory

Path to the project directory (required)

#### `--setEPSG` {#setEPSG}
**Type:** Value (flag)
**Description:** Set project EPSG coordinate system

EPSG authority code (default: Unknown)

#### `-S`, `--setIDXYZ` {#setIDXYZ}
**Type:** Switch (flag)
**Description:** Enable manual setting of coordinate column numbers

Flag to set coordinate column numbers

**Dependencies:** When using -S/[--setIDXYZ](#setIDXYZ), these column flags work together:
- [--setID](#setID): ID column number
- [--setX](#setX): X coordinate column number
- [--setY](#setY): Y coordinate column number
- [--setZ](#setZ): Z coordinate column number (optional)

**Example:** `-S --setID 0 --setX 1 --setY 2 --setZ 3`

#### `--setID` {#setID}
**Type:** Value (flag)
**Description:** Set ID column number

Column number for ID field

#### `--setX` {#setX}
**Type:** Value (flag)
**Description:** Set X coordinate column number

Column number for X coordinate

#### `--setY` {#setY}
**Type:** Value (flag)
**Description:** Set Y coordinate column number

Column number for Y coordinate

#### `--setZ` {#setZ}
**Type:** Value (flag)
**Description:** Set Z coordinate column number

Column number for Z coordinate

#### `--setDel` {#setDel}
**Type:** Value (flag)
**Description:** Set CSV delimiter type

Type of CSV delimiter (DEFAULT or COMMA)

#### `-C`, `--converter` {#converter}
**Type:** Switch (flag)
**Description:** Convert CSV format data into MUSE format

Flag to enable data conversion

**Dependencies:** When using -C/[--converter](#converter), these flags are commonly used:
- [--pdir](#pdir): Project directory (REQUIRED)
- [--setDel](#setDel): CSV delimiter type (optional)
- [--inf](#inf), [--sup](#sup): Value limits (optional)
- -S/[--setIDXYZ](#setIDXYZ): Manual column mapping (optional)

**Example:** `-C --pdir /project --setDel COMMA --inf 0.0 --sup 100.0`

#### `--inf` {#inf}
**Type:** Value (flag)
**Description:** Set inferior limit for variable values

Inferior limit value

#### `--sup` {#sup}
**Type:** Value (flag)
**Description:** Set superior limit for variable values

Superior limit value

#### `-R`, `--read` {#read}
**Type:** Switch (flag)
**Description:** Enable reading of MUSE format files

Flag to enable MUSE format reading

**Dependencies:** When using -R/[--read](#read), these flags work together:
- [--pdir](#pdir): Project directory (REQUIRED)
- [--var](#var): Variable name (default: ALL_INPUT)
- [--nrealiz](#nrealiz): Number of realizations (optional)
- [--hist](#hist): Enable histogram plotting (optional)

**Example:** `-R --pdir /project --var temperature --hist --nbin 20`

#### `-v`, `--var` {#var}
**Type:** Value (flag)
**Description:** Specify variable name to read

Variable name (default: ALL_INPUT)

#### `-n`, `--nrealiz` {#nrealiz}
**Type:** Value (flag)
**Description:** Set number of realization to process

Number of realization (default: 0)

#### `--hist` {#hist}
**Type:** Switch (flag)
**Description:** Enable histogram computation and plotting

Flag to compute histogram plots

**Dependencies:** When using [--hist](#hist), these flags work together:
- [--nval](#nval): Minimum number of values for plotting (default: 20)
- [--nbin](#nbin): Number of histogram bins (default: 1)
Requires sufficient data points (>= nval threshold)

**Example:** `--hist --nval 50 --nbin 25`

#### `--nval` {#nval}
**Type:** Value (flag)
**Description:** Set minimum number of values for histogram plotting

Minimum number of values (default: 20)

#### `--nbin` {#nbin}
**Type:** Value (flag)
**Description:** Set number of bins for histogram plot

Number of bins for histogram (default: 1)

#### `--csv` {#csv}
**Type:** Switch (flag)
**Description:** Enable CSV format for output files

Flag to save files in CSV format

---

