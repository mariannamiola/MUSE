## muse_compute

**Description:** MUSE - Modelling Uncertainty as a Support of Environment. MUSE-compute application

### Usage

```bash
muse_compute [OPTIONS]
```

### Options Index

- [`-C`, `--compute`](#compute) — Enable computation mode
- [`-p`, `--pdir`](#pdir) — Set project directory
- [`--debug`](#debug) — Set debug mode to save additional support files
- [`--mode`](#mode) — Set computation mode
- [`-v`, `--var`](#var) — Set variable name to perform computations
- [`-m`, `--geom`](#geom) — Set geometry model name to perform computations
- [`--sub`](#sub) — Set extracted sub-dataset referring to specified geometry domain
- [`--rotaxis`](#rotaxis) — Set rotation axis for data rotation (X, Y, Z)
- [`--rotangle`](#rotangle) — Set rotation angle (clockwise) for data rotation
- [`--rotcx`](#rotcx) — Set rotation center x coordinate for data rotation
- [`--rotcy`](#rotcy) — Set rotation center y coordinate for data rotation
- [`--rotcz`](#rotcz) — Set rotation center z coordinate for data rotation
- [`--vario`](#vario) — Set loading of variogram configuration from file
- [`--dir`](#dir) — Set variogram direction type
- [`--dim`](#dim) — Set variogram dimension type
- [`--zrange`](#zrange) — Set range in Z direction
- [`--crit`](#crit) — Set interpolation algorithm for geostatistical computations
- [`--bnscore`](#bnscore) — Enable back normal score transformation (integrated into SGS algorithm)
- [`--extr`](#extr) — Set extrapolation type in back normal score transformation
- [`--minextr`](#minextr) — Set minimum value for extrapolation in back normal score transformation
- [`--maxextr`](#maxextr) — Set maximum value for extrapolation in back normal score transformation
- [`--nsim`](#nsim) — Set number of iterations of simulation process
- [`--csize`](#csize) — Set cell size for 2D declustering
- [`--nstep`](#nstep) — Set number of grid translation steps for 2D declustering
- [`-S`, `--stats`](#stats) — Compute statistical analysis on simulation results
- [`--space`](#space) — Set space type for statistical analysis
- [`--cpdf`](#cpdf) — Plot the cpdf of the simulation results (use with -B and --ncell)
- [`-B`, `--back-normalscore`](#back-normalscore) — Enable back normal score transformation
- [`-f`, `--file`](#file) — Set input file path to apply back normal score transformation
- [`-D`, `--db`](#db) — Create database from simulations
- [`--input`](#input) — Set number of input samples
- [`--simulated`](#simulated) — Set number of simulated points
- [`--scaleradius`](#scaleradius) — Set scale factor of search radius
- [`--octant`](#octant) — Set octant search in simulation algorithm
- [`--csv`](#csv) — Set CSV format for output files
- [`--out`](#out) — Set type of SGS output (due to dual version of SGS algorithm in geostatslib)
- [`--format`](#format) — Set for encoding output by data format

### Options

---

#### `-C`, `--compute` {#compute}

**Type:** Switch

**Description:** Enable computation mode for MUSE-compute application. This flag activates the computation mode of the MUSE-compute application, allowing you to perform geostatistical computations and analyses based on the specified parameters and configurations. When this flag is set, the application will execute the computational workflow defined by the provided options and parameters, enabling you to generate results such as simulations, and statistical analyses based on your project data, variogram models and settings.

**Dependencies:** When using this flag, the following parameters become important for defining the computation workflow:
- [--var](#var): Specify the variable name to analyze (mandatory)
- [--geom](#geom): Specify the geometry model name (mandatory)

**Example:** `muse_compute -C -p /path/to/project_directory --var temperature --geom mesh_model`

---

#### `-p`, `--pdir` {#pdir}

<div class="required"><strong>⚠ Required:</strong> true (this parameter is mandatory for the computation process).</div>

**Type:** Value | `std::string`

**Format:** `string (path to the project directory)`

**Description:** Set project directory for MUSE-compute application. This option allows you to specify the path to the project directory for the MUSE-compute application, which is where the application will look for input data, variogram models, geometry models, and where it will save output results and logs. The project directory should contain the necessary files and subdirectories for the computation process, and specifying this path correctly is important for ensuring that the application can access the required resources and save results in the appropriate location.

**Default:** `"path/to/project_directory" (placeholder value, should be replaced with an actual path to the project directory).`

**Example:** `muse_compute -C -p /path/to/project_directory --var temperature --geom mesh_model`

---

#### `--debug` {#debug}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set debug mode for MUSE-compute application. This flag enables debug mode in the MUSE-compute application, which allows for saving additional support files during the computation process for troubleshooting and analysis purposes. When this flag is set, the application will generate and save intermediate results, logs, and diagnostic information that can be useful for investigating the computation process in more detail or when encountering issues. This can help users to understand the internal workings of the computation, identify potential problems, and analyze the results more effectively.

**Default:** `false (debug mode is disabled by default).`

**Example:** `muse_compute -C -p /path/to/project_directory --var temperature --geom mesh_model --debug`

---

#### `--mode` {#mode}

**Type:** Value | `std::string`

**Format:** `string` (`AUTO`, `MANUAL`)

**Description:** Set computation mode for MUSE-compute application. This option allows you to specify the computation mode for the MUSE-compute application, which can influence how the computations are performed and how parameters are selected.

**Default:** `"AUTO" (automatic parameter selection is applied by default).`

**Dependencies:** ⚠ Partially implemented: in the current code only the PRESENCE of [--mode](#mode) is used (it switches the input loading path); the AUTO/MANUAL value is not read yet, so the two values behave identically for now.
When fully implemented, the following modes are foreseen:
- AUTO: Automatic parameter selection
- MANUAL: Manual parameter configuration required

**Example:** `muse_compute -C -p /path/to/project_directory --var temperature --geom mesh_model --mode AUTO`

---

#### `-v`, `--var` {#var}

<div class="required"><strong>⚠ Required:</strong> true (this parameter is mandatory when using the -C/--compute flag).</div>

**Type:** Value | `std::string`

**Format:** `string (name of the variable)`

**Description:** Set variable name for MUSE-compute application. This option allows you to specify the name of the variable to be processed in the MUSE-compute application. The variable name should correspond to a variable that exists in the project data, and it is mandatory when using the -C/--compute flag to ensure that the application knows which variable to analyze and perform computations on. Specifying the correct variable name is crucial for the computation process, as it determines which data will be used for variogram modeling, simulations, and other geostatistical analyses.

**Default:** `"name_var" (placeholder value, should be replaced with the actual variable name from the project data).`

**Example:** `muse_compute -C -p /path/to/project_directory --var temperature --geom mesh_model`

---

#### `-m`, `--geom` {#geom}

<div class="required"><strong>⚠ Required:</strong> true (this parameter is mandatory when using the -C/--compute flag).</div>

**Type:** Value | `std::string`

**Format:** `string (name of the geometry model)`

**Description:** Set geometry model for MUSE-compute application. This option allows you to specify the name of the geometry model to be used in the MUSE-compute application. The geometry model should be defined in the project and can be a surface mesh, volume mesh, or any other type of geometric representation that is compatible with the application. Specifying the geometry model is mandatory when using the -C/--compute flag, as it defines the spatial framework for the computations and analyses that will be performed on the specified variable. The geometry model must be available in the project for the application to access and utilize it during the computation process.

**Default:** `"name_geometry" (placeholder value, should be replaced with the actual name of the geometry model from the project).`

**Example:** `muse_compute -C -p /path/to/project_directory --var temperature --geom mesh_model`

---

#### `--sub` {#sub}

**Type:** Value | `std::string`

**Format:** `string value (name of the sub-dataset)`

**Description:** Set extracted sub-dataset referring to specified geometry domain from project data. This option allows you to specify a sub-dataset that corresponds to a particular geometry domain within the project data (derived from muse-manipulate).

**Default:** `false`

**Dependencies:** When using this flag, ensure that the specified sub-dataset is properly extracted (by muse-manipulate) and corresponds to the geometry domain you want to analyze. This allows for more targeted variogram computation based on specific spatial domains within the project data.

**Example:** `--sub subdataset-name`

---

#### `--rotaxis` {#rotaxis}

**Type:** Value | `std::string`

**Format:** `string value (X, Y, Z)`

**Description:** Set rotation axis for data rotation. This option allows you to specify the axis around which the data will be rotated. The rotation can be applied to the spatial coordinates of the data, which may be useful for aligning the data with a particular orientation or for performing certain types of analyses that require a specific coordinate system.

**Default:** `NO (no rotation is applied)`

**Dependencies:** When using this flag, you typically need to specify the rotation angle (with [--rotangle](#rotangle)) and the rotation center coordinates (with [--rotcx](#rotcx), [--rotcy](#rotcy), [--rotcz](#rotcz)) to fully define the rotation transformation. The rotation axis can be set to X, Y, or Z depending on the desired rotation direction.

**Example:** `--rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0`

---

#### `--rotangle` {#rotangle}

**Type:** Value | `double`

**Format:** `double value (rotation angle in degrees)`

**Description:** Set rotation angle (clockwise) for data rotation. This option allows you to specify the angle by which the data will be rotated in a clockwise direction. The rotation is applied around the axis specified with --rotaxis and centered at the coordinates specified with --rotcx, --rotcy, and --rotcz.

**Default:** `0.0 (no rotation)`

**Dependencies:** When using this flag, you typically need to specify the rotation axis (with [--rotaxis](#rotaxis)) and the rotation center coordinates (with [--rotcx](#rotcx), [--rotcy](#rotcy), [--rotcz](#rotcz)) to fully define the rotation transformation. The rotation angle should be provided in degrees, and the rotation will be applied in a clockwise direction based on the specified axis and center.

**Example:** `--rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0`

---

#### `--rotcx` {#rotcx}

**Type:** Value | `double`

**Format:** `double value (x coordinate of rotation center)`

**Description:** Set rotation center x coordinate for data rotation. This option allows you to specify the x-coordinate of the center point around which the data will be rotated. The rotation is applied based on the axis specified with --rotaxis and the angle specified with --rotangle.

**Default:** `0.0 (rotation around the origin)`

**Dependencies:** Default is 0.0 (rotation around the origin). When using this flag, you typically need to specify the rotation axis (with [--rotaxis](#rotaxis)) and the rotation angle (with [--rotangle](#rotangle)) to fully define the rotation transformation. The rotation center coordinates (rotcx, rotcy, rotcz) define the point in space around which the rotation will occur. The x-coordinate (rotcx) is used in conjunction with the y and z coordinates (rotcy, rotcz) to specify the full rotation center.

**Example:** `--rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0`

---

#### `--rotcy` {#rotcy}

**Type:** Value | `double`

**Format:** `double value (y coordinate of rotation center)`

**Description:** Set rotation center y coordinate for data rotation. This option allows you to specify the y-coordinate of the center point around which the data will be rotated. The rotation is applied based on the axis specified with --rotaxis and the angle specified with --rotangle.

**Default:** `0.0 (rotation around the origin)`

**Dependencies:** Default is 0.0 (rotation around the origin). When using this flag, you typically need to specify the rotation axis (with [--rotaxis](#rotaxis)) and the rotation angle (with [--rotangle](#rotangle)) to fully define the rotation transformation. The rotation center coordinates (rotcx, rotcy, rotcz) define the point in space around which the rotation will occur. The y-coordinate (rotcy) is used in conjunction with the x and z coordinates (rotcx, rotcz) to specify the full rotation center.

**Example:** `--rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0`

---

#### `--rotcz` {#rotcz}

**Type:** Value | `double`

**Format:** `double value (z coordinate of rotation center)`

**Description:** Set rotation center z coordinate for data rotation. This option allows you to specify the z-coordinate of the center point around which the data will be rotated. The rotation is applied based on the axis specified with --rotaxis and the angle specified with --rotangle.

**Default:** `0.0 (rotation around the origin)`

**Dependencies:** Default is 0.0 (rotation around the origin). When using this flag, you typically need to specify the rotation axis (with [--rotaxis](#rotaxis)) and the rotation angle (with [--rotangle](#rotangle)) to fully define the rotation transformation. The rotation center coordinates (rotcx, rotcy, rotcz) define the point in space around which the rotation will occur. The z-coordinate (rotcz) is used in conjunction with the x and y coordinates (rotcx, rotcy) to specify the full rotation center.

**Example:** `--rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0`

---

#### `--vario` {#vario}

**Type:** Value | `std::string`

**Format:** `string value (path to the variogram configuration file)`

**Description:** Set loading of variogram configuration from file. This option allows you to specify a path to a variogram configuration file that contains predefined settings for variogram modeling and computation. The configuration file can include parameters such as variogram type, direction, dimension, range, and other relevant settings that can be loaded into the application to streamline the variogram setup process. Using a configuration file can help to ensure consistency in variogram settings across different projects or analyses and can save time by allowing you to reuse predefined configurations.

**Default:** `"none" (no configuration file is loaded)`

**Example:** `--vario path/to/variogram_config.txt`

---

#### `--dir` {#dir}

**Type:** Value | `std::string`

**Format:** `string value (type of variogram direction)` (`OMNI`, `DIR`)

**Description:** Set variogram direction type related to variogram configuration. This option allows you to specify the type of variogram direction to be used in the variogram modeling and computation process. The direction type can influence how the variogram is computed and interpreted, especially in cases where anisotropy is present in the data.

**Default:** `"OMNI" (omnidirectional variogram)`

**Dependencies:** When using this flag, the following direction types are available for selection:
- OMNI: An omnidirectional variogram is computed, which does not consider any specific directionality in the spatial data. This type of variogram is useful when the spatial continuity is assumed to be the same in all directions.
- DIR: A directional variogram is computed, which considers specific directions in the spatial data. This type of variogram is useful when anisotropy is present, meaning that the spatial continuity varies with direction. When using DIR, you typically need to specify the direction angles and tolerances to define the directional variogram computation.

**Example:** `--dir DIR`

---

#### `--dim` {#dim}

**Type:** Value | `std::string`

**Format:** `string value (type of variogram dimension)` (`3D`, `3Dxy`, `3Dz`, `2D`, `1Dz`, `1D`)

**Description:** Set variogram dimension type related to variogram configuration. This option allows you to specify the type of variogram dimension to be used in the variogram modeling and computation process. The dimension type can influence how the variogram is computed and interpreted, especially in cases where the spatial data has different characteristics in different dimensions.

**Default:** `"3D" (three-dimensional variogram)`

**Dependencies:** Variogram configuration flags work together:
- [--dir](#dir): Direction type (OMNI, DIR)
- [--dim](#dim): Dimension type
- [--zrange](#zrange): Z direction range (used with 3D dimensions)

**Example:** `--dir OMNI --dim 3D --zrange 50.0`

---

#### `--zrange` {#zrange}

**Type:** Value | `double`

**Format:** `double value (range in Z direction)`

**Description:** Set range in Z direction. This option allows you to specify the range value in the Z direction for variogram computation when using three-dimensional variogram dimensions (3Dxy). The Z range can influence how the variogram is computed and interpreted in cases where there is significant variability in the vertical direction. Setting an appropriate Z range can help to capture the spatial continuity and variability in the Z direction more effectively, especially when the data exhibits different characteristics in the vertical dimension compared to the horizontal dimensions.

**Default:** `1.0 (default Z range value)`

**Dependencies:** This flag is used in conjunction with the variogram configuration flags [--dir](#dir) and [--dim](#dim). When using three-dimensional variogram dimensions (3Dxy), this flag becomes important for defining the Z direction range, which can help to manage the influence of vertical variability in the variogram computation and ensure that the variogram captures the spatial continuity in the Z direction appropriately.

**Example:** `--dir OMNI --dim 3Dxy --zrange 50.0`

---

#### `--crit` {#crit}

**Type:** Value | `std::string`

**Format:** `string value (type of interpolation algorithm)` (`SGS`, `IK`, `SISIM`)

**Description:** Set interpolation algorithm for geostatistical computations. This option allows you to specify the interpolation algorithm to be used in the geostatistical computations and analyses performed by the MUSE-compute application. The choice of interpolation algorithm can influence the results of simulations, predictions, and other analyses based on the spatial data and variogram models.

**Default:** `"SGS" (sequential Gaussian simulation)`

**Dependencies:** When using this flag, the following interpolation algorithms are available for selection:
- SGS: Sequential Gaussian Simulation, a geostatistical simulation method that generates realizations of spatial variables based on a Gaussian distribution and the variogram model.
- IK: Indicator Kriging, a geostatistical interpolation method that estimates the probability of a variable exceeding a certain threshold based on indicator variables and the variogram model.
- SISIM: Sequential Indicator Simulation, a geostatistical simulation method that generates realizations of spatial variables based on indicator variables and the variogram model, often used for categorical or non-Gaussian data.

**Example:** `--crit IK`

---

#### `--bnscore` {#bnscore}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Enable back normal score transformation integrated into SGS. This flag allows you to enable the back normal score transformation as part of the sequential Gaussian simulation (SGS) process. When this flag is set, the application will perform a back normal score transformation on the data after the SGS computations. The back normal score transformation can be particularly useful when dealing with skewed or non-normal data distributions, as it can help to normalize the data and make it more suitable for geostatistical modeling and simulation.

**Default:** `false (back normal score transformation is disabled by default).`

**Dependencies:** When using this flag, it is typically applied in conjunction with the [--crit](#crit) SGS option, as it is specifically designed to work with the sequential Gaussian simulation method. Enabling this flag can help to improve the quality of simulations and predictions when the data exhibits non-normal characteristics, by transforming the data back to a normal distribution after the SGS computations.

**Example:** `--crit SGS --bnscore`

---

#### `--extr` {#extr}

**Type:** Value | `std::string`

**Format:** `string value (type of extrapolation)` (`none`, `Extr`)

**Description:** Set extrapolation type in back normal score transformation. This option allows you to specify the type of extrapolation to be applied in the back normal score transformation process. Extrapolation can be used to manage values that fall outside the range of the original data distribution, especially when performing transformations on skewed or non-normal data. The choice of extrapolation type can influence how the back normal score transformation handles extreme values and can help to ensure that the transformed data remains within a reasonable range, which can be important for maintaining the integrity of the geostatistical modeling and simulation process.

**Default:** `"none" (no extrapolation is applied)`

**Dependencies:** When using this flag, the following extrapolation types are available for selection:
- none: No extrapolation is applied in the back normal score transformation.
- Extr: Extrapolation is applied in the back normal score transformation, and the specific method of extrapolation can be defined based on the requirements of the data and analysis. When using extrapolation, you typically need to specify the minimum and maximum extrapolation values (with [--minextr](#minextr) and [--maxextr](#maxextr)) to define the range of extrapolation for values that fall outside the original data distribution.

**Example:** `--extr Extr --minextr 10.0 --maxextr 70.0`

---

#### `--minextr` {#minextr}

<div class="required"><strong>⚠ Required:</strong> false (this parameter is optional and only relevant when extrapolation is enabled)</div>

**Type:** Value | `double`

**Format:** `double value (minimum extrapolation value)`

**Description:** Set minimum value for extrapolation in back normal score transformation. This option allows you to specify the minimum value to be used for extrapolation in the back normal score transformation process. When extrapolation is enabled (with --extr Extr), this minimum value defines the lower bound for extrapolating values that fall outside the original data distribution. Setting an appropriate minimum extrapolation value can help to manage extreme values and ensure that the transformed data remains within a reasonable range, which can be important for maintaining the integrity of geostatistical modeling and simulation.

**Default:** `0.0 (default minimum extrapolation value)`

**Dependencies:** This flag is used in conjunction with the [--extr](#extr) flag when extrapolation is enabled. When using extrapolation, you typically need to specify both the minimum and maximum extrapolation values (with [--minextr](#minextr) and [--maxextr](#maxextr)) to define the range of extrapolation for values that fall outside the original data distribution. Setting appropriate minimum and maximum extrapolation values can help to ensure that the back normal score transformation handles extreme values effectively and maintains the quality of the geostatistical modeling and simulation process.

**Example:** `--extr Extr --minextr 10.0`

---

#### `--maxextr` {#maxextr}

<div class="required"><strong>⚠ Required:</strong> false (this parameter is optional and only relevant when extrapolation is enabled)</div>

**Type:** Value | `double`

**Format:** `double value (maximum extrapolation value)`

**Description:** Set maximum value for extrapolation in back normal score transformation. This option allows you to specify the maximum value to be used for extrapolation in the back normal score transformation process. When extrapolation is enabled (with --extr Extr), this maximum value defines the upper bound for extrapolating values that fall outside the original data distribution. Setting an appropriate maximum extrapolation value can help to manage extreme values and ensure that the transformed data remains within a reasonable range, which can be important for maintaining the integrity of geostatistical modeling and simulation.

**Default:** `100000.0 (default maximum extrapolation value)`

**Dependencies:** This flag is used in conjunction with the [--extr](#extr) flag when extrapolation is enabled. When using extrapolation, you typically need to specify both the minimum and maximum extrapolation values (with [--minextr](#minextr) and [--maxextr](#maxextr)) to define the range of extrapolation for values that fall outside the original data distribution. Setting appropriate minimum and maximum extrapolation values can help to ensure that the back normal score transformation handles extreme values effectively and maintains the quality of the geostatistical modeling and simulation process.

**Example:** `--extr Extr --maxextr 70.0`

---

#### `--nsim` {#nsim}

<div class="required"><strong>⚠ Required:</strong> false (this parameter is optional and can be adjusted based on the desired balance between simulation quality and computational time)</div>

**Type:** Value | `uint`

**Format:** `unsigned integer value (number of simulation iterations)`

**Description:** Set number of simulation iterations. This option allows you to specify the number of iterations to be performed during the simulation process. The number of iterations can influence the quality and stability of the simulation results, as well as the computational time required to complete the simulations. Setting an appropriate number of iterations can help to ensure that the simulations converge to a stable solution and that the results are reliable for analysis and decision-making.

**Default:** `10 (default number of simulation iterations)`

**Dependencies:** When setting the number of simulation iterations, consider the complexity of the spatial data, the variogram model, and the computational resources available. In general, a higher number of iterations can lead to more stable and reliable simulation results, but it may also increase the computational time required to complete the simulations. It is often recommended to start with a moderate number of iterations (e.g., 10) and adjust based on the observed results and computational performance.

**Example:** `--nsim 20`

---

#### `--csize` {#csize}

<div class="required"><strong>⚠ Required:</strong> Must be specified when using --decl flag for 2D declustering.</div>

**Type:** Value | `double`

**Format:** `double value (cell size)`

**Description:** Set cell size for 2D declustering. This option allows you to specify the cell size to be used for 2D declustering when computing the variogram. The cell size defines the spatial resolution of the grid used for declustering, where data points within the same cell are considered part of the same cluster. Choosing an appropriate cell size is important for effective declustering, as it can influence the weights assigned to data points and ultimately affect the variogram results.

**Default:** `0.0 (no cell size specified).`

**Dependencies:** The specified cell size should be chosen based on the spatial characteristics of the data and the desired level of declustering.
It is often recommended to experiment with different cell sizes to find the optimal value for your specific dataset and analysis goals.
When using declustering, the cell size should be provided in the same units as the spatial coordinates of the data (e.g., meters) to ensure proper declustering based on the spatial distribution of the data points.

**Example:** `--decl --csize 100 --nstep 5`

---

#### `--nstep` {#nstep}

<div class="required"><strong>⚠ Required:</strong> Must be specified when using --decl flag for 2D declustering.</div>

**Type:** Value | `int`

**Format:** `positive integer`

**Description:** Set number of grid translation steps for 2D declustering. This option allows you to specify the number of grid translation steps to be used for 2D declustering when computing the variogram. Grid translation is a technique used in declustering to reduce the influence of clustered data points by translating the grid multiple times and averaging the results. The number of steps determines how many times the grid will be translated, which can help to further mitigate the effects of clustering in the data.

**Default:** `0 (no grid translation).`

**Dependencies:** The specified number of steps should be chosen based on the level of declustering desired and the computational resources available, as increasing the number of steps can lead to more effective declustering but also increases the computational time required for variogram computation.
When using declustering, the number of grid translation steps should be a positive integer, and it is often recommended to experiment with different values to find the optimal number of steps for your specific dataset and analysis goals.

**Example:** `--decl --csize 100 --nstep 5`

---

#### `-S`, `--stats` {#stats}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Enable statistical analysis on simulation results. This flag allows you to enable the computation of statistical analysis on the results generated from the simulations. When this flag is set, the application will perform various statistical analyses on the simulated data, which can include mean, variance, quantiles, and other relevant statistics that can help to summarize and interpret the simulation results. Enabling statistical analysis can provide valuable insights into the characteristics of the simulated data and can assist in making informed decisions based on the simulation outcomes.

**Default:** `false (statistical analysis is disabled by default).`

**Dependencies:** When using this flag, the application will automatically compute and save the statistical analysis results based on the simulations performed on the _stats folder within the project directory.
It requires the following parameters to be specified for the computation process:
- [--var](#var): Specify the variable name to analyze (mandatory)
- [--geom](#geom): Specify the geometry model name (mandatory)
- [--dir](#dir): Specify the directory for the simulation results (mandatory)
- [--dim](#dim): Specify the dimensionality of the analysis (mandatory)
- [--space](#space): Specify the space type for analysis (optional, default: NORMAL)

**Example:** `-S -p /path/to/project_directory --var temperature --geom mesh_model --dir DIR --dim 2D --space VAR`

---

#### `--space` {#space}

**Type:** Value | `std::string`

**Format:** `string value (type of space)` (`NORMAL`, `VAR`)

**Description:** Set space type for statistical analysis on simulation results. This option allows you to specify the type of space to be used for the statistical analysis of simulation results. The space type can influence how the statistical analysis is performed and interpreted, especially in cases where the spatial characteristics of the data play a significant role in the analysis.

**Default:** `"NORMAL" (normal space is used by default).`

**Dependencies:** When using this flag, the following space types are available for selection:
- NORMAL: The statistical analysis is performed in the normal data space (after normal score transformation).
- VAR: The statistical analysis is performed in the original variable space (after back normal score transformation).

**Example:** `-S -p /path/to/project_directory --var temperature --geom mesh_model --dir DIR --dim 2D --space VAR`

---

#### `--cpdf` {#cpdf}

**Type:** Switch

**Description:** Plot the cumulative probability distribution function (cpdf) reconstructed from the

**Dependencies:** Used together with -B (back normal score), which provides the loaded realizations and the
normal-score transform. Combine with [--ncell](#ncell) to select the target: an integer plots the cpdf of
that single cell, "all" overlays the cpdf of every cell in one plot.

**Example:** `-B --cpdf --ncell 42  (single cell)     -B --cpdf --ncell all  (all cells)`

---

#### `-B`, `--back-normalscore` {#back-normalscore}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Enable back normal score transformation after SGS computations. This flag allows you to enable the back normal score transformation after saving the results of the sequential Gaussian simulation (SGS) process. When this flag is set, the application will perform a back normal score transformation on the data after the SGS computations.

**Default:** `false (back normal score transformation is disabled by default).`

**Dependencies:** When using this flag, it is typically applied in conjunction with the [--crit](#crit) SGS option, as it is specifically designed to work with the sequential Gaussian simulation method. Enabling this flag can help to improve the quality of simulations and predictions when the data exhibits non-normal characteristics, by transforming the data back to a normal distribution after the SGS computations. The back normal score transformation can be particularly useful for managing skewed or non-normal data distributions, ensuring that the transformed data is more suitable for geostatistical modeling and simulation.

**Example:** `-B -p /path/to/project_directory --var temperature --geom mesh_model --crit SGS --extr Extr --minextr 10.0 --maxextr 70.0`

---

#### `-f`, `--file` {#file}

**Type:** Value | `std::string`

**Format:** `string value (path to the input file)`

**Description:** Set input file path. This option allows you to specify the path to an input file that contains data to be used in the MUSE-compute application. The input file can include various types of data relevant to the computations and analyses performed by the application, such as back normal score transformation. The file should be formatted according to the requirements of the application: a column of estimates in normal space is accepted.

**Default:** `"/path/to/file" (placeholder value, should be replaced with the actual path to the input file).`

**Example:** `--file /path/to/input_file.txt`

---

#### `-D`, `--db` {#db}

**Type:** Switch

**Description:** Enable database creation from simulations (preliminary version of database implementation). This flag allows you to enable the creation of a database to store the results generated from the simulations. When this flag is set, the application will create a database structure to organize and manage the simulation results, which can facilitate data retrieval, and analysis. The database can be designed to store various types of information related to the simulations, such as input parameters, variogram models, simulation outputs, and statistical analyses.

---

#### `--input` {#input}

<div class="required"><strong>⚠ Required:</strong> false (this parameter is optional and can be adjusted based on the desired balance between simulation quality and computational time)</div>

**Type:** Value | `uint`

**Format:** `unsigned integer value (number of input samples)`

**Description:** Set number of input samples. This option allows you to specify the number of input samples to be used in the nearest neighbor search for the SGS algorithm. The number of input samples can influence the quality and accuracy of the simulations, as it determines how many neighboring data points are considered when performing the simulation process. Setting an appropriate number of input samples can help to ensure that the simulations capture the spatial variability of the data effectively while also managing computational resources.

**Default:** `4 (default number of input samples)`

**Dependencies:** When setting the number of input samples, consider the spatial density of the data and the variogram model, as well as the computational resources available. In general, a higher number of input samples can lead to more accurate simulations by capturing more spatial variability, but it may also increase the computational time required to complete the simulations. It is often recommended to start with a moderate number of input samples (e.g., 4) and adjust based on the observed results and computational performance.

**Example:** `--input 6`

---

#### `--simulated` {#simulated}

<div class="required"><strong>⚠ Required:</strong> false (this parameter is optional and can be adjusted based on the desired balance between simulation quality and computational time)</div>

**Type:** Value | `uint`

**Format:** `unsigned integer value (number of simulated points)`

**Description:** Set number of simulated points. This option allows you to specify the number of points to be simulated during the simulation process. The number of simulated points can influence the quality and stability of the simulation results, as well as the computational time required to complete the simulations. Setting an appropriate number of simulated points can help to ensure that the simulations capture the spatial variability of the data effectively while also managing computational resources.

**Default:** `3 (default number of simulated points)`

**Dependencies:** When setting the number of simulated points, consider the spatial density of the data and the variogram model, as well as the computational resources available. In general, a higher number of simulated points can lead to more accurate simulations by capturing more spatial variability, but it may also increase the computational time required to complete the simulations. It is often recommended to start with a moderate number of simulated points (e.g., 3) and adjust based on the observed results and computational performance.

**Example:** `--simulated 5`

---

#### `--scaleradius` {#scaleradius}

<div class="required"><strong>⚠ Required:</strong> false (this parameter is optional and can be adjusted based on the desired balance between simulation quality and computational time)</div>

**Type:** Value | `double`

**Format:** `double value (scale factor for search radius)`

**Description:** Set scale factor of search radius. This option allows you to specify a scale factor for the search radius used in the nearest neighbor search during the simulation algorithm. The search radius determines how far the algorithm looks for neighboring data points when performing simulations, and the scale factor can be used to adjust the size of this search radius based on the spatial characteristics of the data and the variogram model. Setting an appropriate scale factor can help to ensure that the simulations capture the spatial variability of the data effectively while also managing computational resources.

**Default:** `1.0 (default scale factor for search radius)`

**Dependencies:** When setting the scale factor for the search radius, consider the spatial density of the data and the variogram model, as well as the computational resources available. In general, a higher scale factor can lead to more accurate simulations by capturing more spatial variability, but it may also increase the computational time required to complete the simulations. It is often recommended to start with a moderate scale factor (e.g., 1.0) and adjust based on the observed results and computational performance.

**Example:** `--scaleradius 1.5`

---

#### `--octant` {#octant}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set octant search in simulation algorithm. This flag allows you to enable the octant search method in the simulation algorithm, which can be used to optimize the nearest neighbor search process. When this flag is set, the algorithm will divide the search space into octants (in 3D) or quadrants (in 2D) and perform the search within these subdivisions, which can help to reduce the computational time required for finding neighboring data points during simulations. Enabling octant search can be particularly beneficial when dealing with large datasets or complex variogram models, as it can improve the efficiency of the simulation process while still capturing the spatial variability of the data effectively.

**Default:** `false (octant search is disabled by default).`

**Example:** `--octant`

---

#### `--csv` {#csv}

**Type:** Switch

**Format:** `boolean flag`

**Description:** Set CSV format for output files. This flag allows you to specify that the output files generated by the application should be saved in CSV (Comma-Separated Values) format. When this flag is set, the application will format the output data as CSV, which can be easily opened and analyzed using spreadsheet software or other data analysis tools. Saving output files in CSV format can facilitate data sharing and further analysis, as CSV is a widely supported format for tabular data.

**Default:** `false (CSV format is disabled by default)`

**Example:** `--csv`

---

#### `--out` {#out}

**Type:** Value | `std::string`

**Format:** `string value (type of SGS output)` (`MEAN`, `VECSIM`)

**Description:** Set type of SGS output. This option allows you to specify the type of output to be generated from the sequential Gaussian simulation (SGS) process (due to dual version of SGS algorithm in geostatslib).

**Default:** `"MEAN" (mean of simulations is used as default SGS output)`

**Example:** `--crit SGS --out VECSIM`

---

#### `--format` {#format}

**Type:** Value | `std::string`

**Format:** `string value (YES or NO for JSON format encoding)`

**Description:** Set flag to json output for data format encoding. This flag allows you to specify whether the output data should be encoded in JSON format, which can be useful for structured data representation and interoperability with other applications. When this flag is set, the application will format the output data as JSON, which can facilitate data sharing and further analysis, as JSON is a widely supported format for structured data.

**Default:** `YES (JSON format encoding is enabled by default)`

**Example:** `--format NO`

---

