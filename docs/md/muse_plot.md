## muse_plot

**Description:** MUSE = Modelling of Uncertainty as a Support of Environment; Plot tool

### Usage

```bash
muse_plot [SWITCHES] [OPTIONS]
```

### Options

---

#### `-H`, `--histogram` {#histogram}
**Type:** Switch (flag)

**Description:** Creation histogram plot

Enable creation histogram plot

**Dependencies:** Plot type selection (mutually exclusive):
- -H/[--histogram](#histogram): Histogram plot (choose this OR others)
- -B/[--bivariate_plot](#bivariate-plot): Bivariate plot
- -E/[--error_plot](#error-plot): Error plot
- -T/[--triangular_plot](#triangular-plot): Triangular plot
When using histogram, configure with:
- [--nval](#nval): Minimum number of values (default: 20)
- [--nbin](#nbin): Number of bins (default: 1)

**Example:** `-H --nval 50 --nbin 25`

---

#### `-p`, `--pdir` {#pdir}
**Type:** Value (flag)

**Description:** Project directory

Path to project directory

---

#### `-v`, `--val` {#val}
**Type:** Value (flag)

**Description:** Set values

values

---

#### `--nval` {#nval}
**Type:** Value (flag)

**Description:** Set min number of values, sufficient for histogram plot

Number of set min number of values, sufficient for histogram plot

---

#### `--nbin` {#nbin}
**Type:** Value (flag)

**Description:** Set number of bins for histogram plot

Number of set number of bins for histogram plot

**Dependencies:** Used with -H/[--histogram](#histogram) flag
Controls histogram resolution and detail level

---

#### `-x`, `--x_variable` {#x-variable}
**Type:** Value (flag)

**Description:** Name variable1

Name of name variable1

**Dependencies:** Required for:
- -B/[--bivariate_plot](#bivariate-plot): X-axis variable (must use with [--y_variable](#y-variable))
- -T/[--triangular_plot](#triangular-plot): First variable (must use with [--y_variable](#y-variable) and [--z_variable](#z-variable))

---

#### `-B`, `--bivariate_plot` {#bivariate-plot}
**Type:** Switch (flag)

**Description:** Creation bivariate plot

Enable creation bivariate plot

**Dependencies:** Mutually exclusive with -H/[--histogram](#histogram), -E/[--error_plot](#error-plot), -T/[--triangular_plot](#triangular-plot)
Requires both:
- [--x_variable](#x-variable): X-axis variable (mandatory)
- [--y_variable](#y-variable): Y-axis variable (mandatory)

**Example:** `-B --x_variable temperature --y_variable pressure`

---

#### `-y`, `--y_variable` {#y-variable}
**Type:** Value (flag)

**Description:** Name variable2

Name of name variable2

**Dependencies:** Required for:
- -B/[--bivariate_plot](#bivariate-plot): Y-axis variable (must use with [--x_variable](#x-variable))
- -T/[--triangular_plot](#triangular-plot): Second variable (must use with [--x_variable](#x-variable) and [--z_variable](#z-variable))

---

#### `-E`, `--error_plot` {#error-plot}
**Type:** Switch (flag)

**Description:** Creation error plot

Enable creation error plot

**Dependencies:** Mutually exclusive with -H/[--histogram](#histogram), -B/[--bivariate_plot](#bivariate-plot), -T/[--triangular_plot](#triangular-plot)

---

#### `-T`, `--triangular_plot` {#triangular-plot}
**Type:** Switch (flag)

**Description:** Creation triangular plot

Enable creation triangular plot

**Dependencies:** Mutually exclusive with -H/[--histogram](#histogram), -B/[--bivariate_plot](#bivariate-plot), -E/[--error_plot](#error-plot)
Requires all three variables:
- [--x_variable](#x-variable): First variable (mandatory)
- [--y_variable](#y-variable): Second variable (mandatory)
- [--z_variable](#z-variable): Third variable (mandatory)

**Example:** `-T --x_variable var1 --y_variable var2 --z_variable var3`

---

#### `-z`, `--z_variable` {#z-variable}
**Type:** Value (flag)

**Description:** Name variable3

Name of name variable3

**Dependencies:** Required for -T/[--triangular_plot](#triangular-plot)
Must be used together with [--x_variable](#x-variable) and [--y_variable](#y-variable)

---

