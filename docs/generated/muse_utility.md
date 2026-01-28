## muse_utility

**Description:** MUSE = Modelling of Uncertainty as a Support of Environment; Muse Utility tool

### Usage

```bash
muse_utility [SWITCHES] [OPTIONS]
```

### Options

---

#### `-L`, `--list` {#list}
**Type:** Switch (flag)

**Description:** File List

Enable file list

**Dependencies:** Operation mode selection (mutually exclusive):
- -L/[--list](#list): File listing operation
- -Q/[--query](#query): Query path creation
- -H/[--history](#history): JSON history analysis
When using file listing:
- [--pdir](#pdir): Directory path (optional)
- [--type](#type): File type filter (optional)

**Example:** `-L --pdir /project --type JSON`

---

#### `-p`, `--pdir` {#pdir}
**Type:** Value (flag)

**Description:** Directory

Path to directory

**Dependencies:** Used with -L/[--list](#list) flag for file listing operations
Specifies the directory to search for files

---

#### `--type` {#type}
**Type:** Value (flag)

**Description:** Type of file

Path to type of file

**Dependencies:** Used with -L/[--list](#list) flag for file filtering
Available types: JSON, MESH

---

#### `-Q`, `--query` {#query}
**Type:** Switch (flag)

**Description:** Query for creation path

Enable query for creation path

**Dependencies:** Mutually exclusive with -L/[--list](#list) and -H/[--history](#history)
Operation mode for path creation queries

---

#### `-H`, `--history` {#history}
**Type:** Switch (flag)

**Description:** Set JSON history

Enable set json history

**Dependencies:** Mutually exclusive with -L/[--list](#list) and -Q/[--query](#query)
History analysis operation requires:
- [--json](#json): JSON file path (mandatory)
OPTIONAL history navigation flags:
- [--back](#back): Recursive backward navigation
- [--forward](#forward): Recursive forward navigation
- [--more](#more): Forward navigation with commands

**Example:** `-H --json /path/to/history.json --more`

---

#### `--json` {#json}
**Type:** Value (flag)

**Description:** Set json file

Path to set json file

**Dependencies:** Required when using -H/[--history](#history) flag
Specifies the JSON file containing processing history

---

#### `--back` {#back}
**Type:** Switch (flag)

**Description:** Set JSON history (recursively - back)

Enable set json history (recursively - back)

**Dependencies:** Optional modifier for -H/[--history](#history) operations
Enables recursive backward navigation through history

---

#### `--forward` {#forward}
**Type:** Switch (flag)

**Description:** Set JSON history (recursively - forward)

Enable set json history (recursively - forward)

**Dependencies:** Optional modifier for -H/[--history](#history) operations
Enables recursive forward navigation through history

---

#### `--more` {#more}
**Type:** Switch (flag)

**Description:** Set JSON history (recursively - forward) and commands

Enable set json history (recursively - forward) and commands

**Dependencies:** Optional modifier for -H/[--history](#history) operations
Provides detailed forward navigation with command information

---

