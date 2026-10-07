# MUSE - Modeling Uncertainty as a Support for Environment

MUSE is an open-source stochastic tool for modeling uncertainty in multi-scenario environmental applications.
It provides a self-contained C++ package in which the workflow from input data to output generation is managed through command-line applications and reproducible scripts.

MUSE combines geostatistical modeling, geometric processing, workflow metadata management, and parallel-computing strategies to support robust and efficient environmental simulations.
The software has been tested on macOS and Linux systems.

The repository includes the full MUSE package: source code for the computational modules, a C++ library collecting the core functionalities, examples for testing the main features, and user documentation.
Example datasets and scripts refer to synthetic scenarios and real case studies.
Result visualization is supported through [ParaView](https://www.paraview.org/download/).

## Clone

The repository includes external submodules required by MUSE. Please, clone it recursively with:

```bash
git clone --recursive https://github.com/mariannamiola/MUSE.git
```

If the repository was cloned without the `--recursive` option, initialize and update the submodules from the repository root:

```bash
git submodule update --init --recursive
```

Throughout this README, `${ROOT}` denotes the root directory of the MUSE repository.


## Content of the repository

- `00_installDeps.sh`: bash script to install systems dependencies
- `00_buildDeps.sh`: bash script to build and install external libraries
- `00_exe_creation.sh`: script to build and install MUSE applications, creating the executable files
- `apps`: including source code for all applications and CMakeLists.txt file for apps building
- `docs`: software documentation (html, md)
- `examples`: test cases (input data and execution/visualization scripts)
- `external`: external libraries
- `include`: functionalities library to support source codes (muselib)
- `scripts`: to auto-generate the software documentation from the source code


## Dependencies

MUSE relies on system dependencies and external libraries. The required system dependencies can be installed through the build scripts provided in the repository.

Some mandatory libraries are included as submodules under `${ROOT}/external`:

- to manage command line arguments and options: [tclap](https://tclap.sourceforge.net/);
- to metadata the computational process: `cereal`;
- to support mesh-based stochastic computation: [GeoStatsLib](https://github.com/DanielaCabiddu/GeoStatsLib);
- to manage geospatial data: [GDAL](https://github.com/OSGeo/gdal) and [PROJ](https://github.com/OSGeo/PROJ);
- to process polygonal and polyhedral mesh: [Triangle](https://www.cs.cmu.edu/~quake/triangle.html), [TetGen](https://wias-berlin.de/software), [libigl](https://libigl.github.io), [cinolib](https://github.com/mlivesu/cinolib), [concaveman](https://github.com/sadaszewski/concaveman-cpp);
- to support ellipse fitting: the C++ impelmentation of [ellipse-fitting](https://scipython.com/blog/direct-linear-least-squares-fitting-of-an-ellipse/).

MUSE also uses ParaView Python scripting and its GUI for results visualization.


## Build and executable generation

To install all required dependencies and generate the MUSE executables, use the following pipeline:

```bash
cd MUSE
./00_installDeps.sh
./00_buildDeps.sh all
./00_exe_creation.sh
```

If the scripts are not executable on your system, enable execution permissions before running them:

```bash
chmod +x 00_installDeps.sh 00_buildDeps.sh 00_exe_creation.sh
```

Apps executables will be made available in _${ROOT}/bin_ folder.

`muse_export` (and `muse_export_raster`) is optional and not required by the core pipeline. If it fails to build on your system, skip it with:

```bash
./00_exe_creation.sh --no-export
```

(equivalent to passing `-DMUSE_BUILD_EXPORT=OFF` to CMake).

### Tested platforms

MUSE has been built successfully on the following configurations:

| OS | Compiler | CMake | System libraries |
|----|----------|-------|------------------|
| macOS 26.6 (Apple Silicon) | Apple Clang 17.0 | 4.1.1 | Homebrew: armadillo 15.0.3, libomp 21.1.1, boost 1.90.0, lz4 1.10.0, sqlite 3.51.1, libtiff 4.7, eigen 3.4 / 5.0, gnuplot 6.0.3 |
| Ubuntu (version TBD) | GCC (version TBD) | TBD | apt packages installed by `00_installDeps.sh` |

System dependencies are installed by `00_installDeps.sh` (Homebrew on macOS, apt on Debian/Ubuntu); packages already present are skipped:

| Package (macOS / Ubuntu) | Used by |
|--------------------------|---------|
| `cmake` | build system |
| `pkgconf` / `pkg-config` | apps CMake configuration |
| `lz4` / `liblz4-dev` | FLANN, GeoStatsLib |
| `armadillo` / `libarmadillo-dev` | GeoStatsLib |
| `eigen` / `libeigen3-dev` | linear algebra |
| `sqlite` / `sqlite3`, `libsqlite3-dev` | PROJ, muse_compute |
| `libtiff` / `libtiff-dev` | PROJ |
| `libomp` (macOS only; GCC ships OpenMP) | OpenMP |
| `boost` / `libboost-dev` | cinolib |
| `gnuplot` | Matplot++ (needed at runtime to draw plots) |

The external libraries are pinned through git submodules and built by `00_buildDeps.sh`: PROJ (9.x), GDAL (3.11), Matplot++ (1.2.2), cinolib, libigl (2.6), cereal (1.3.2), GeoStatsLib. Clone the repository with `git clone --recursive` (or run `git submodule update --init --recursive`) to get the exact tested versions.

Notes:

- On Linux with glibc older than 2.34 (e.g. Ubuntu 20.04) `pthread` must be linked explicitly; this is handled in the CMake files (`Threads::Threads`).
- On macOS, `libomp` is searched in the Homebrew prefix (`/opt/homebrew/opt/libomp` or `/usr/local/opt/libomp`).

## Documentation

The documentation can be found in one of the following directories, depending on the selected output format:

```text
${ROOT}/docs/html
${ROOT}/docs/md
```

## Input CSV format

The CSV files imported by `muse_data` must start with **6 header rows**, followed by the data rows (one sample per row, one variable per column). The default delimiter is `;` (use `--setDel COMMA` for `,`).

| Row | Content | Notes |
|-----|---------|-------|
| 1 | Variable name | Used to name the output files (e.g. `X`, `Y`, `Z`, `phi`) |
| 2 | Unit of measure | May be empty; `%` is used to rescale compositional data |
| 3 | Flags | Data type and checks applied to the values (see below) |
| 4 | Parents | Optional, may be empty |
| 5 | Description | Optional, may be empty |
| 6 | Comments | Optional, may be empty |

Example (`examples/06_sec_2D_400_tri/data/samples_sec400.csv`):

```text
X;Y;Z;phi
m;m;m;
R+;R+;D;R+
;;;
Longitude;Latitude;Depth;
;;;
72.9;1;-1.30;0.43
14.3;1;-10.83;0.35
...
```

A flag is a combination of the following characters:

| Flag | Meaning | Flag | Meaning |
|------|---------|------|---------|
| `R` | Real | `K` | Categorical |
| `N` | Integer | `V` | String |
| `C` | Compositional | `T` | Time |
| `P` | Probability | `Y` | Date |
| `B` | Bounded | `S` | Soft data (`a`, `b`, `c`: soft A/B/C) |
| `L` | Logarithmic | `H` | Hard data |
| `E` | Error | `G` | Geodesic |
| `D` | Depth | `A` | Absolute height |
| `+` | Positive values (e.g. `R+`) | `-` | Negative values (e.g. `R-`) |

`+`/`-` are not checked when combined with `D` (depth).

If row 3 contains characters that are not valid flags (e.g. because the file has a single header row and row 3 holds data values), `muse_data` stops with an error.

Note: the `samples.csv` files with a single header row found in some example `data` folders are only used to visualize the samples in ParaView and are not `muse_data` input.

## Examples

Example data and scripts are provided under the `${ROOT}/examples` directory.
Each example contains input data and scripts to run the corresponding MUSE workflow and, when available, python script to visualize the results.

Each `10_test_*` script automatically adds `${ROOT}/bin` (where the executables are
generated) to the `PATH`, so **make sure MUSE has been built first** (see
[Build and executable generation](#build-and-executable-generation)).

A quick, self-contained example can be run as follows:

```bash
cd ${ROOT}/examples/06_sec_2D_400_tri/script
./10_test_MUSE_06_sec_2D_400_tri.sh project
```

By default the results are written under the shared working directory:

```text
${ROOT}/examples/MUSE_test/06_sec_2D_400_tri
```

More generally, examples are executed from the corresponding `script` directory using
the `10_test_*` script:

```bash
cd ${ROOT}/examples/<example_name>/script
./10_test_MUSE_<example_name>.sh project
```

Each `10_test_*` script accepts the following optional arguments:

```text
-w, --work <dir>    working directory for the output project (default: ${ROOT}/examples/MUSE_test)
-s, --sim  <n>      number of stochastic simulations
-p, --proj <name>   output project name
-d, --data <file>   input data file name
-h, --help          show all options
```

When a visualization script is provided, the results can be plotted with Python from
the same `script` directory, e.g.:

```bash
python 20_visual_2D.py      # 2D examples
python 20_visual_3D.py      # 3D examples (e.g. 22_Tomography)
```

The full list of examples, with a short description of each workflow, is given in
[`examples/README.md`](examples/README.md).

## Computational histories (EWoPe)

MUSE relies on a native version of [EWoPe](https://github.com/DanielaCabiddu/EWOPE) (*Embeddable WOrkflow PErsistence*) for metadata and workflow tracker. 
Every MUSE application writes a customized JSON metadata next to its outputs, so the computational history of a result can be reconstructed afterwards with the
standalone `EWOPE_history` executable.

EWoPe must be cloned and built separately, following the instructions in its repository:

```bash
git clone --recursive https://github.com/DanielaCabiddu/EWOPE.git
```

Once built, `EWOPE_history` is available in `${EWOPE_ROOT}/bin`. For example, to reconstruct the
history of the `phi` variable produced by the `06_sec_2D_400_tri` example, run from
`${EWOPE_ROOT}/bin`:

```bash
./EWOPE_history -w ${ROOT}/06_sec_2D_400_tri \
                -j ${ROOT}/06_sec_2D_400_tri/out/compute/phi_DIR2D_sec_xz/_varspace/phi.json \
                -f
```

## Authors and contacts

- Marianna Miola, CNR-IMATI, Genova, Italy, marianna.miola@cnr.it
- Daniela Cabiddu, CNR-IMATI, Genova, Italy, daniela.cabiddu@cnr.it
- Simone Pittaluga, CNR-IMATI, Genova, Italy, simone.pittaluga@cnr.it
- Marino Vetuschi Zuccolini, DISTAV-UNIGE, Genova, Italy, marino.zuccolini@unige.it

## License

MUSE is distributed under a dual-license model.

- **Open-source use:** GNU GPL v3.0 or later.
- **Commercial use:** available under a separate commercial license.

See the `LICENSE` and `COPYING` files for details.

## Citation

If you use MUSE in academic work, please cite the following references.

```bibtex
@article{miola2026muse,
  author  = {Miola, Marianna and Cabiddu, Daniela and Pittaluga, Simone and Vetuschi Zuccolini, Marino},
  title   = {{MUSE}: A Mesh-Based Geostatistics Framework for Modeling Spatial Uncertainty},
  journal = {SSRN Electronic Journal},
  year    = {2026},
  doi     = {10.2139/ssrn.7422348},
  note    = {Available at SSRN}
}

@phdthesis{MIOLA2025PHD,
title = {{Increase the knowledge of Natural Systems through the evaluation of the uncertainty of environmental data: operational theory and application}},
author = {Miola, Marianna},
year = {2025},
school = {Università degli Studi di Genova (UNIGE)},
address = {Genova, Italy}
}

@inproceedings {10.2312:stag.20221265,
booktitle = {Smart Tools and Applications in Graphics - Eurographics Italian Chapter Conference},
editor = {Cabiddu, Daniela and Schneider, Teseo and Allegra, Dario and Catalano, Chiara Eva and Cherchi, Gianmarco and Scateni, Riccardo},
title = {{MUSE: Modeling Uncertainty as a Support for Environment}},
author = {Miola, Marianna and Cabiddu, Daniela and Pittaluga, Simone and Vetuschi Zuccolini, Marino},
year = {2022},
publisher = {The Eurographics Association},
ISSN = {2617-4855},
ISBN = {978-3-03868-191-5},
DOI = {10.2312/stag.20221265}
}

```

### Additional citations for specific components

#### `muse_geometry`

If you use the `muse_geometry` component, please also cite:

```bibtex
@inproceedings{10.2312:stag.20251318,
booktitle = {Smart Tools and Applications in Graphics - Eurographics Italian Chapter Conference},
editor = {Comino Trinidad, Marc and Mancinelli, Claudio and Maggioli, Filippo and Romanengo, Chiara and Cabiddu, Daniela and Giorgi, Daniela},
title = {{A lightweight open-source tool for Meshing within Geosciences}},
author = {Miola, Marianna and Cabiddu, Daniela and Pittaluga, Simone and Raviola, Micaela and Zuccolini, Marino Vetuschi},
year = {2025},
publisher = {The Eurographics Association},
ISSN = {2617-4855},
ISBN = {978-3-03868-296-7},
DOI = {10.2312/stag.20251318}
}
```

#### Metadata handling in MUSE

If you use the metadata and workflow-persistence functionalities in MUSE, please also cite the EWoPe paper:

```bibtex
@article{MIOLA2026106099,
title = {{EWoPe: A light Embeddable WOrkflow PErsistence tool for geoscientific pipeline reproducibility}},
journal = {Computers \& Geosciences},
volume = {209},
pages = {106099},
year = {2026},
issn = {0098-3004},
doi = {https://doi.org/10.1016/j.cageo.2025.106099},
url = {https://www.sciencedirect.com/science/article/pii/S0098300425002493},
author = {Marianna Miola and Daniela Cabiddu and Simone Pittaluga and Micaela Raviola and Marino {Vetuschi Zuccolini}},
keywords = {Scientific workflows, Workflow persistence, Provenance tracking, Metadata, Geoscientific modeling, Computational reproducibility},
abstract = {Scientific workflows are essential in modern geoscientific research, where complex, multi-stage computational pipelines are used to analyze heterogeneous environmental data. Ensuring the reproducibility and traceability of these workflows is critical but often challenging due to their intricacy and evolving structure. We introduce EWoPe (Embeddable Workflow Persistence), a lightweight and embeddable methodology and C++ library designed to persist and reconstruct scientific workflows over time. Unlike existing workflow systems and provenance tools, our method adds minimal overhead to workflow execution: it does not automate or optimize processes, but instead ensures persistence through lightweight and structured metadata. EWoPe models workflows as directed acyclic graphs (DAGs), in which each data node is linked to computational tasks through metadata. The latter captures input-output dependencies, algorithm parameters, execution commands, and intermediate results, supporting full traceability and reproducibility of computational histories. EWoPe offers dual usability: as a standalone command-line tool or as an embeddable component within larger applications. We show its flexibility and applicability through a case study involving a complex workflow leading to subsurface reaction-transport modeling, starting from boreholes data. The EWoPe library is publicly available and designed to be extensible, making it suitable for a wide range of scientific domains, including geochemistry, geophysics, environmental engineering, and any other fields where transparency and data integrity are critical.}
}
```

## Acknowledgment

This work was developed within the Ph.D. Research Project in Science and Technologies for Earth and Environment, supported by the Italian Ministry of University and Research under the National Operational Programme NOP "Research and Innovation" 2014-2020, "Education and research for recovery - REACT-EU", Axis IV - Action IV.5 "PhD programmes on green topics", Ministerial Decree no. 1061, 10 August 2021.
