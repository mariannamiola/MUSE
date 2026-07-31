## muse_export

**Description:** MUSE = Modelling of Uncertainty as a Support of Environment; Export tool

### Usage

```bash
muse_export [--geopkg] [--csv] [OPTIONS]
```

### Options Index

- [`-p`, `--pdir`](#pdir) — Project directory
- [`-v`, `--var`](#var) — Variable
- [`-m`, `--geom`](#geom) — Geometry model
- [`-o`, `--out`](#out) — Output directory
- [`--type`](#type) — Type of analysis
- [`--mf`](#mf) — multiframe name
- [`--dir`](#dir) — type of variogram direction
- [`--dim`](#dim) — type of variogram dimension
- [`-N`, `--nsim`](#nsim) — Number of simulations to export
- [`--append_csv_table`](#append-csv-table) — Append existing table (csv)
- [`--append_sep`](#append-sep) — Separator for append table
- [`--append_gpkg`](#append-gpkg) — Append existing table (gpkg)
- [`--geopkg`](#geopkg) — Export to geopkg
- [`--csv`](#csv) — Export to csv

### Options

---

#### `-p`, `--pdir` {#pdir}

<div class="required"><strong>⚠ Required:</strong> true (mandatory for all export operations).</div>

**Type:** Value | `std::string`

**Format:** `string (path to the project directory)`

**Description:** Project directory. Path to the MUSE project whose computed results are exported; data are read from its out/ subfolders.

**Default:** `"Directory" (placeholder value, should be replaced with an actual project directory path).`

**Dependencies:** The project directory must contain MUSE data files.

**Example:** `muse_export -p /path/to/project -v forecast -m su_liguria.obj --csv -o export.csv`

---

#### `-v`, `--var` {#var}

<div class="required"><strong>⚠ Required:</strong> true (mandatory for all export operations).</div>

**Type:** Value | `std::string`

**Format:** `string (variable name)`

**Description:** Variable. Name of the variable whose results are exported.

**Default:** `"name_var" (placeholder value, should be replaced with an actual variable name).`

**Dependencies:** The variable must exist in the project data.

**Example:** `muse_export -p /path/to/project -v forecast -m su_liguria.obj --csv -o export.csv`

---

#### `-m`, `--geom` {#geom}

**Type:** Value | `std::string`

**Format:** `string (geometry model name, e.g. mesh file name)`

**Description:** Geometry model. Name of the geometry model (surface or volumetric mesh) on which the variable has been computed.

**Default:** `"name_geometry" (placeholder value, should be replaced with an actual geometry model name).`

**Dependencies:** Often required depending on export type. The geometry model must be available in the project.

**Example:** `muse_export -p /path/to/project -v forecast -m su_liguria.obj --csv -o export.csv`

---

#### `-o`, `--out` {#out}

**Type:** Value | `std::string`

**Format:** `string (output path)`

**Description:** Output file/directory. Path of the file (or directory) where the exported table is written.

**Default:** `"output_directory" (placeholder value, should be replaced with an actual output path).`

**Example:** `muse_export -p /path/to/project -v forecast -m su_liguria.obj --csv -o ../export.csv`

---

#### `--type` {#type}

**Type:** Value | `std::string`

**Format:** `string`

**Description:** Type of analysis. Selects which computed statistics are exported; use "INDICATOR" to export the results of an indicator analysis (the "_best" values), otherwise the statistical moments computed in the variogram space are exported.

**Default:** `"type" (placeholder value; when not INDICATOR the statistical-moments export is used).`

**Dependencies:** Used to locate the correct compute output subfolder.

**Example:** `muse_export -p /path/to/project -v forecast -m mesh.obj --type INDICATOR --csv -o export.csv`

---

#### `--mf` {#mf}

**Type:** Value | `std::string`

**Format:** `string (multiframe identifier)`

**Description:** Multiframe name. Name of the multiframe (time frame) whose compute results are exported; when set, results are read from out/compute/<mf>/...

**Default:** `"multiframe" (placeholder value; when not set, non-multiframe results are exported).`

**Example:** `muse_export -p /path/to/project -v forecast -m mesh.obj --mf 120320181400 --csv -o export.csv`

---

#### `--dir` {#dir}

**Type:** Value | `std::string`

**Format:** `string` (`OMNI`, `DIR`)

**Description:** Type of variogram direction. Identifies the directional setting used during the computation, needed to locate the correct compute output folder.

**Default:** `"OMNI" (omnidirectional).`

**Dependencies:** Used with variogram export operations. Works together with [--dim](#dim) for variogram configuration.

**Example:** `muse_export -p /path/to/project -v forecast -m mesh.obj --dir DIR --dim 3D --csv -o export.csv`

---

#### `--dim` {#dim}

**Type:** Value | `std::string`

**Format:** `string` (`3D`, `3Dxy`, `3Dz`, `2D`, `1Dz`, `1D`)

**Description:** Type of variogram dimension. Identifies the dimensional setting used during the computation, needed to locate the correct compute output folder.

**Default:** `"3D"`

**Dependencies:** Used with variogram export operations. Works together with [--dir](#dir) for variogram configuration.

**Example:** `muse_export -p /path/to/project -v forecast -m mesh.obj --dir OMNI --dim 2D --csv -o export.csv`

---

#### `-N`, `--nsim` {#nsim}

**Type:** Value | `int`

**Format:** `int`

**Description:** Number of simulations to export. When greater than 0, the individual simulation realizations are appended to the exported table.

**Default:** `0 (no individual simulations are exported; only the statistics).`

**Dependencies:** Used for exporting multiple simulation results. Requires simulation data to be available in project.

**Example:** `muse_export -p /path/to/project -v forecast -m mesh.obj -N 10 --csv -o export.csv`

---

#### `--append_csv_table` {#append-csv-table}

**Type:** Value | `std::string`

**Format:** `string (path to an existing .csv file)`

**Description:** Append existing table (csv). Path to an existing CSV table whose columns are merged into the exported CSV output.

**Default:** `"append_table" (placeholder value; when not set nothing is appended).`

**Dependencies:** Use with the [--csv](#csv) output format.

**Example:** `muse_export -p /path/to/project -v forecast -m mesh.obj --csv --append_csv_table base.csv -o export.csv`

---

#### `--append_sep` {#append-sep}

**Type:** Value | `std::string`

**Format:** `string (single character)`

**Description:** Separator for append table. Field separator used to read the CSV table provided with --append_csv_table.

**Default:** `";" (semicolon).`

**Dependencies:** Used together with [--append_csv_table](#append-csv-table).

**Example:** `muse_export -p /path/to/project -v forecast -m mesh.obj --csv --append_csv_table base.csv --append_sep "," -o export.csv`

---

#### `--append_gpkg` {#append-gpkg}

**Type:** Value | `std::string`

**Format:** `string (path to an existing .gpkg file)`

**Description:** Append existing table (gpkg). Path to an existing GeoPackage whose layer is used as a base to which the exported columns are added.

**Default:** `"append_table" (placeholder value; when not set nothing is appended).`

**Dependencies:** Requires an existing GeoPackage file at the specified path. Use with the [--geopkg](#geopkg) output format.

**Example:** `muse_export -p /path/to/project -v forecast -m mesh.obj --geopkg --append_gpkg base.gpkg -o export.gpkg`

---

#### `--geopkg` {#geopkg}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Export to GeoPackage. Writes the exported table as a GeoPackage (.gpkg) vector layer.

**Default:** `false (GeoPackage export is disabled by default).`

**Dependencies:** Output format selection (choose [--geopkg](#geopkg) or [--csv](#csv)). Combine with [--append_gpkg](#append-gpkg) to extend an existing GeoPackage.

**Example:** `muse_export -p /path/to/project -v forecast -m mesh.obj --geopkg -o export.gpkg`

---

#### `--csv` {#csv}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Export to CSV. Writes the exported table as a comma/semicolon separated values (.csv) file.

**Default:** `false (CSV export is disabled by default).`

**Dependencies:** Output format selection (choose [--geopkg](#geopkg) or [--csv](#csv)). Combine with [--append_csv_table](#append-csv-table) to extend an existing CSV table.

**Example:** `muse_export -p /path/to/project -v forecast -m mesh.obj --csv -o export.csv`

---

