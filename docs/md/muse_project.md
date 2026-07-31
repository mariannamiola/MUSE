## muse_project

**Description:** MUSE - Modelling Uncertainty as a Support of Environment. MUSE-project application

### Usage

```bash
muse_project -p <PDIR> -n <NAME> [-N] [--timestamp] [--overwrite] [OPTIONS]
```

### Options

---

#### `-N`, `--new_project` {#new-project}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Creation of a new project. With this flag that needs -p (--pdir) and -n (--name) options, MUSE-project create a filesystem space where organize hyerarchically all the information about input, output and metadata files.

**Default:** `false (project creation is disabled by default).`

**Dependencies:** When using [--new_project](#new-project), the following flags are typically used together:
- [--pdir](#pdir) (required): Specify project directory
- [--name](#name) (required): Specify project name
- [--setEPSG](#setEPSG) (optional): Set coordinate reference system

**Example:** `muse_project -N -p /path/to/project/dir -n 00_test --setEPSG EPSG:4326`

---

#### `-p`, `--pdir` {#pdir}

<div class="required"><strong>⚠ Required:</strong> true (this parameter is mandatory to create a new project).</div>

**Type:** Value | `std::string`

**Format:** `string (path to the directory that will contain the project)`

**Description:** Specify the directory path where the project is created

**Default:** `"/path/to/project/dir" (placeholder value, should be replaced with an actual path).`

**Dependencies:** Required when using [--new_project](#new-project) flag

**Example:** `muse_project -N -p /path/to/project/dir -n 00_test`

---

#### `-n`, `--name` {#name}

**Type:** Value | `std::string`

**Format:** `string (name of the project)`

**Description:** Specify name of the new project. Check the project name and, if it exists, the project is not created (unless --overwrite or --timestamp are used).

**Default:** `"project_name" (placeholder value, should be replaced with the actual project name).`

**Dependencies:** Required when using [--new_project](#new-project) flag. The project name will be used as:
- Directory name: /path/to/project/dir/project_name
- JSON config file in output folder: /path/to/project/dir/project_name/out/project_name.json

**Example:** `-n 00_test`

---

#### `--setEPSG` {#setEPSG}

**Type:** Value | `std::string`

**Format:** `string in the form EPSG:<code> (projected coordinate reference system)`

**Description:** Set project EPSG coordinate reference system in projected coordinates. At the moment, no geographic coordinates are permitted.

**Default:** `"unknown" (no coordinate reference system is assigned by default).`

**Dependencies:** Optional parameter, used with [--new_project](#new-project) flag
Common projected EPSG codes:
- EPSG:3857 (Web Mercator)
- EPSG:32633 (UTM Zone 33N)

**Example:** `muse_project -N -p /path/to/project/dir -n 00_test --setEPSG EPSG:32633`

---

#### `--timestamp` {#timestamp}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Check project name and, if it exists, create a new folder as "project_YYYY-MM-DD_HH-MM-SS"

**Default:** `false (timestamp renaming is disabled by default).`

**Dependencies:** Optional parameter. Mutually exclusive with [--overwrite](#overwrite).

**Example:** `muse_project -N -p /path/to/project/dir -n 00_test --timestamp`

---

#### `--overwrite` {#overwrite}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Check project name and overwrite the existing project

**Default:** `false (existing projects are not overwritten by default).`

**Dependencies:** Optional parameter. Mutually exclusive with [--timestamp](#timestamp).

**Example:** `muse_project -N -p /path/to/project/dir -n 00_test --overwrite`

---

