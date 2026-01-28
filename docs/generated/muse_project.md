## muse_project

**Description:** MUSE = Modelling of Uncertainty as a Support of Environment; Muse Project tool

### Usage

```bash
muse_project -p <PDIR> -n <NAME> [-N] [OPTIONS]
```

### Options

---

#### `-N`, `--new_project` {#new-project}
**Type:** Switch (flag)

**Description:** Create a new project

Flag to enable new project creation

**Dependencies:** When using -N/[--new_project](#new-project), the following flags are typically used together:
- [--pdir](#pdir) (required): Specify project directory
- [--name](#name) (required): Specify project name
- [--setEPSG](#setEPSG) (optional): Set coordinate reference system

**Example:** `muse_project -N --pdir /path/to/projects --name MyProject --setEPSG EPSG:4326`

---

#### `-p`, `--pdir` {#pdir}
**Type:** Value (flag)

**Description:** Specify project directory path

Path where the project will be created

**Dependencies:** Required when using -N/[--new_project](#new-project) flag

**Example:** `--pdir /home/user/projects`

---

#### `-n`, `--name` {#name}
**Type:** Value (flag)

**Description:** Specify name of the new project

Project name to be created

**Dependencies:** Required when using -N/[--new_project](#new-project) flag. The project name will be used as:
- Directory name: {pdir}/{name}
- JSON config file: {pdir}/{name}/out/{name}.json

**Example:** `--name MyGeologyProject`

---

#### `--setEPSG` {#setEPSG}
**Type:** Value (flag)

**Description:** Set project EPSG coordinate reference system

EPSG authority code for the project coordinate system

**Dependencies:** Optional parameter, used with -N/[--new_project](#new-project) flag
Common EPSG codes:
- EPSG:4326 (WGS84 Geographic)
- EPSG:3857 (Web Mercator)
- EPSG:32633 (UTM Zone 33N)

**Example:** `--setEPSG EPSG:4326`

---

