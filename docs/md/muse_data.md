## muse_data

**Description:** MUSE - Modelling Uncertainty as a Support for Environment. muse-data application

### Usage

```bash
muse_data -p <PDIR> [OPTIONS]
```

### Options Index

- [`-N`, `--new_project`](#new-project) — Initialize a new project directory structure for storing (source point) data
- [`-p`, `--pdir`](#pdir) — Project directory
- [`-i`, `--input`](#input) — Copy input file(s) in the project directory (in/data/)
- [`--setEPSG`](#setEPSG) — Set project EPSG
- [`-S`, `--setIDXYZ`](#setIDXYZ) — Set n. column coordinate
- [`--setID`](#setID) — Set ID
- [`--setX`](#setX) — Set coordinate x
- [`--setY`](#setY) — Set coordinate y
- [`--setZ`](#setZ) — Set coordinate z
- [`--setDel`](#setDel) — Set type of csv delimiter
- [`-C`, `--converter`](#converter) — Converter data (csv format) into MUSE format
- [`--inf`](#inf) — Set inf limit
- [`--sup`](#sup) — Set sup limit
- [`--tol`](#tol) — Set tolerance
- [`-R`, `--read`](#read) — Reading MUSE format
- [`-v`, `--var`](#var) — Variable
- [`-n`, `--nrealiz`](#nrealiz) — Set number of realization
- [`--hist`](#hist) — Compute plot - hitogram
- [`--nval`](#nval) — Set min number of values, sufficient for histogram plot
- [`--nbin`](#nbin) — Set number of bins for histogram plot
- [`--csv`](#csv) — Saving file as csv
- [`--check-duplicates`](#check-duplicates) — Enable check of duplicated points
- [`--remove-duplicates`](#remove-duplicates) — Enable remvoval of duplicated points

### Options

---

#### `-N`, `--new_project` {#new-project}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Initialize a new project directory structure for storing (source point) data. Creates the in/data and out/data subdirectories under the path specified by --pdir.

**Default:** `false (project initialization is disabled by default).`

**Dependencies:** Requires [--pdir](#pdir) to be set; use [--input](#input) to optionally copy data file(s) into the project.

**Example:** `muse_data -N -p /path/to/project/dir --input user/path/filename.csv`

---

#### `-p`, `--pdir` {#pdir}

<div class="required"><strong>⚠ Required:</strong> true (this parameter is mandatory for every operation).</div>

**Type:** Value | `std::string`

**Format:** `string (absolute path to the project root directory)`

**Description:** Specify the directory path where the project is created (the project root directory).

**Default:** `"/path/to/project/dir" (placeholder value, should be replaced with an actual path).`

**Dependencies:** Required when using [--new_project](#new-project), [--converter](#converter) and [--read](#read) flags.

**Example:** `muse_data -N -p /path/to/project/dir`

---

#### `-i`, `--input` {#input}

**Type:** Value | `std::string`, repeatable

**Format:** `string (path to an input file); repeatable`

**Description:** Copy input file(s) into the input data project directory (project/in/data), replacing manual data copy. Multiple files can be specified by repeating the flag.

**Default:** `empty (no file is copied when the flag is not used).`

**Dependencies:** Optional. Recommended with -N to populate the input directory without manual file copying.

**Example:** `muse_data -N -p /path/to/project/dir --input user/path1/file1.csv --input user/path2/file2.csv`

---

#### `--setEPSG` {#setEPSG}

**Type:** Value | `std::string`

**Format:** `string in the form EPSG:<code>`

**Description:** Set project EPSG coordinate system (projected coordinates).

**Default:** `"Unknown" (no coordinate reference system is assigned by default).`

**Dependencies:** Optional parameter, used with [--new_project](#new-project) flag.

**Example:** `muse_data -N -p /path/to/project/dir --setEPSG EPSG:32633`

---

#### `-S`, `--setIDXYZ` {#setIDXYZ}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Enable manual setting of coordinate column numbers, overriding the automatic detection of ID/X/Y/Z columns.

**Default:** `false (columns are detected automatically by default).`

**Dependencies:** When using -S/[--setIDXYZ](#setIDXYZ), these column flags work together:
- [--setID](#setID): ID column number
- [--setX](#setX): X coordinate column number
- [--setY](#setY): Y coordinate column number
- [--setZ](#setZ): Z coordinate column number (optional)

**Example:** `-S --setID 0 --setX 1 --setY 2 --setZ 3`

---

#### `--setID` {#setID}

**Type:** Value | `int`

**Format:** `int`

**Description:** Set ID column number (0-based index of the identifier column in the input CSV).

**Default:** `0 (first column).`

**Dependencies:** Used together with -S/[--setIDXYZ](#setIDXYZ).

**Example:** `muse_data -C -p /path/to/project/dir -S --setID 0 --setX 1 --setY 2`

---

#### `--setX` {#setX}

**Type:** Value | `int`

**Format:** `int`

**Description:** Set X coordinate column number (0-based index of the X column in the input CSV).

**Default:** `0 (first column).`

**Dependencies:** Used together with -S/[--setIDXYZ](#setIDXYZ).

**Example:** `muse_data -C -p /path/to/project/dir -S --setID 0 --setX 1 --setY 2`

---

#### `--setY` {#setY}

**Type:** Value | `int`

**Format:** `int`

**Description:** Set Y coordinate column number (0-based index of the Y column in the input CSV).

**Default:** `0 (first column).`

**Dependencies:** Used together with -S/[--setIDXYZ](#setIDXYZ).

**Example:** `muse_data -C -p /path/to/project/dir -S --setID 0 --setX 1 --setY 2`

---

#### `--setZ` {#setZ}

**Type:** Value | `int`

**Format:** `int`

**Description:** Set Z coordinate column number (0-based index of the Z column in the input CSV).

**Default:** `0 (first column).`

**Dependencies:** Optional. Used together with -S/[--setIDXYZ](#setIDXYZ) for 3D datasets.

**Example:** `muse_data -C -p /path/to/project/dir -S --setID 0 --setX 1 --setY 2 --setZ 3`

---

#### `--setDel` {#setDel}

**Type:** Value | `std::string`

**Format:** `string` (`DEFAULT`, `COMMA`)

**Description:** Set CSV delimiter type used to parse the input file. DEFAULT uses the semicolon ';', COMMA uses ','.

**Default:** `"DEFAULT" (semicolon ';' delimiter).`

**Dependencies:** Used with -C/[--converter](#converter).

**Example:** `muse_data -C -p /path/to/project/dir --setDel COMMA`

---

#### `-C`, `--converter` {#converter}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Convert CSV format data into MUSE format (per-variable .dat/.json files).

**Default:** `false (conversion is disabled by default).`

**Dependencies:** When using -C/[--converter](#converter), these flags are commonly used:
- [--pdir](#pdir): Project directory (REQUIRED)
- [--setDel](#setDel): CSV delimiter type (optional)
- [--inf](#inf), [--sup](#sup): Value limits (optional)
- -S/[--setIDXYZ](#setIDXYZ): Manual column mapping (optional)

**Example:** `muse_data -C -p /project --setDel COMMA --inf 0.0 --sup 100.0`

---

#### `--inf` {#inf}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set inferior limit for variable values. Values below this limit are flagged during conversion.

**Default:** `0 (inferior limit set to 0).`

**Dependencies:** Used with -C/[--converter](#converter), together with [--sup](#sup).

**Example:** `muse_data -C -p /project --inf 0.0 --sup 100.0`

---

#### `--sup` {#sup}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set superior limit for variable values. Values above this limit are flagged during conversion.

**Default:** `1 (superior limit set to 1).`

**Dependencies:** Used with -C/[--converter](#converter), together with [--inf](#inf).

**Example:** `muse_data -C -p /project --inf 0.0 --sup 100.0`

---

#### `--tol` {#tol}

**Type:** Value | `double`

**Format:** `double`

**Description:** Set tolerance used when comparing point coordinates (e.g. to decide whether two points are duplicated).

**Default:** `1.0`

**Dependencies:** Used together with [--check-duplicates](#check-duplicates) / [--remove-duplicates](#remove-duplicates).

**Example:** `muse_data -C -p /project --check-duplicates --tol 0.001`

---

#### `-R`, `--read` {#read}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Enable reading of MUSE format files and print a summary of the stored data.

**Default:** `false (reading is disabled by default).`

**Dependencies:** When using -R/[--read](#read), these flags work together:
- [--pdir](#pdir): Project directory (REQUIRED)
- [--var](#var): Variable name (default: ALL_INPUT)
- [--nrealiz](#nrealiz): Number of realizations (optional)
- [--hist](#hist): Enable histogram plotting (optional)

**Example:** `muse_data -R -p /project --var temperature --hist --nbin 20`

---

#### `-v`, `--var` {#var}

**Type:** Value | `std::string`

**Format:** `string (variable name)`

**Description:** Specify variable name to read. Use ALL_INPUT to process every input variable.

**Default:** `"ALL_INPUT" (all input variables are read).`

**Dependencies:** Used with -R/[--read](#read).

**Example:** `muse_data -R -p /project --var temperature`

---

#### `-n`, `--nrealiz` {#nrealiz}

**Type:** Value | `int`

**Format:** `int`

**Description:** Set number of realizations to process.

**Default:** `0 (no simulated realization is read).`

**Dependencies:** Used with -R/[--read](#read).

**Example:** `muse_data -R -p /project --var temperature --nrealiz 100`

---

#### `--hist` {#hist}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Enable histogram computation and plotting for the read variable(s).

**Default:** `false (histogram computation is disabled by default).`

**Dependencies:** Used with -R/[--read](#read). When using [--hist](#hist), these flags work together:
- [--nval](#nval): Minimum number of values for plotting (default: 20)
- [--nbin](#nbin): Number of histogram bins (default: 1)
Requires sufficient data points (>= nval threshold)

**Example:** `muse_data -R -p /project --var temperature --hist --nval 50 --nbin 25`

---

#### `--nval` {#nval}

**Type:** Value | `int`

**Format:** `int`

**Description:** Set minimum number of values for histogram plotting. The histogram is drawn only if the valid samples are at least this many.

**Default:** `20`

**Dependencies:** Used with [--hist](#hist).

**Example:** `muse_data -R -p /project --var temperature --hist --nval 50`

---

#### `--nbin` {#nbin}

**Type:** Value | `size_t`

**Format:** `size_t`

**Description:** Set number of bins for histogram plot. Controls histogram resolution and detail level.

**Default:** `1 (when not set, the number of bins is chosen automatically).`

**Dependencies:** Used with [--hist](#hist).

**Example:** `muse_data -R -p /project --var temperature --hist --nbin 25`

---

#### `--csv` {#csv}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Enable CSV format for output files (in addition to the MUSE format).

**Default:** `false (output is saved only in MUSE format by default).`

**Dependencies:** Optional additional output format.

**Example:** `muse_data -C -p /project --csv`

---

#### `--check-duplicates` {#check-duplicates}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Enable check of duplicated points. Reports points sharing the same coordinates (within --tol) without modifying the data.

**Default:** `false (duplicate checking is disabled by default).`

**Dependencies:** Used with -C/[--converter](#converter). The comparison tolerance is set with [--tol](#tol).

**Example:** `muse_data -C -p /project --check-duplicates --tol 0.001`

---

#### `--remove-duplicates` {#remove-duplicates}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Enable removal of duplicated points. Removes points sharing the same coordinates (within --tol) from the converted dataset.

**Default:** `false (duplicate removal is disabled by default).`

**Dependencies:** Used with -C/[--converter](#converter). The comparison tolerance is set with [--tol](#tol).

**Example:** `muse_data -C -p /project --remove-duplicates --tol 0.001`

---

