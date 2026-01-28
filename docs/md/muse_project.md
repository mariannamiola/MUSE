## muse_project

**Description:** MUSE = Modelling of Uncertainty as a Support of Environment; Muse Project tool

### Usage

```bash
muse_project [OPTIONS]
```

### Options

#### `-N`, `--new_project`
**Type:** Switch (flag)
**Description:** Create a new project

Switch to enable new project creation
- Usage: muse_project -N --pdir /path/to/projects --name MyProject

#### `-p`, `--pdir`
**Type:** Value (flag)
**Description:** Directory path for the new project

Path where the project will be created
- Usage: muse_project --pdir /path/to/projects

#### `-n`, `--name`
**Type:** Value (flag)
**Description:** Name of the new project

Project name to be created
- Usage: muse_project --name MyProject

#### `--setEPSG`
**Type:** Value (flag)
**Description:** Set project EPSG code (coordinate reference system authority)

EPSG code for the project coordinate system
- Format: EPSG:####
- Example: muse_project --setEPSG EPSG:4326
- Common codes: EPSG:4326 (WGS84), EPSG:3857 (Web Mercator)

---

