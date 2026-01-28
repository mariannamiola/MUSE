## muse_data

**Description:** MUSE = Modelling of Uncertainty as a Support of Environment; Data tool

### Usage

```bash
muse_data [OPTIONS]
```

### Options

#### `-N`, `--new_project`
**Type:** Switch (flag)
**Description:** Create a new project

Flag to create a new project

#### `-p`, `--pdir`
**Type:** Value (flag)
**Description:** Specify project directory

Path to the project directory (required)

#### `--setEPSG`
**Type:** Value (flag)
**Description:** Set project EPSG coordinate system

EPSG authority code (default: Unknown)

#### `-S`, `--setIDXYZ`
**Type:** Switch (flag)
**Description:** Enable manual setting of coordinate column numbers

Flag to set coordinate column numbers

#### `--setID`
**Type:** Value (flag)
**Description:** Set ID column number

Column number for ID field

#### `--setX`
**Type:** Value (flag)
**Description:** Set X coordinate column number

Column number for X coordinate

#### `--setY`
**Type:** Value (flag)
**Description:** Set Y coordinate column number

Column number for Y coordinate

#### `--setZ`
**Type:** Value (flag)
**Description:** Set Z coordinate column number

Column number for Z coordinate

#### `--setDel`
**Type:** Value (flag)
**Description:** Set CSV delimiter type

Type of CSV delimiter (DEFAULT or COMMA)

#### `-C`, `--converter`
**Type:** Switch (flag)
**Description:** Convert CSV format data into MUSE format

Flag to enable data conversion

#### `--inf`
**Type:** Value (flag)
**Description:** Set inferior limit for variable values

Inferior limit value

#### `--sup`
**Type:** Value (flag)
**Description:** Set superior limit for variable values

Superior limit value

#### `-R`, `--read`
**Type:** Switch (flag)
**Description:** Enable reading of MUSE format files

Flag to enable MUSE format reading

#### `-v`, `--var`
**Type:** Value (flag)
**Description:** Specify variable name to read

Variable name (default: ALL_INPUT)

#### `-n`, `--nrealiz`
**Type:** Value (flag)
**Description:** Set number of realization to process

Number of realization (default: 0)

#### `--hist`
**Type:** Switch (flag)
**Description:** Enable histogram computation and plotting

Flag to compute histogram plots

#### `--nval`
**Type:** Value (flag)
**Description:** Set minimum number of values for histogram plotting

Minimum number of values (default: 20)

#### `--nbin`
**Type:** Value (flag)
**Description:** Set number of bins for histogram plot

Number of bins for histogram (default: 1)

#### `--csv`
**Type:** Switch (flag)
**Description:** Enable CSV format for output files

Flag to save files in CSV format

---

