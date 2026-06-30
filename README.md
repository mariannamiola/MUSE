# MUSE - Modeling Uncertainty as a Support for Environment

MUSE is an open-source stochastic tool for modeling uncertainty in multi-scenario environmental applications.
It provides a self-contained C++ package in which the workflow from input data to output generation is managed through command-line applications and reproducible scripts.

MUSE combines geostatistical modeling, geometric processing, workflow metadata management, and parallel-computing strategies to support robust and efficient environmental simulations.
The software has been tested on macOS and Linux systems.

The repository includes the full MUSE package: source code for the computational modules, a C++ library collecting the core functionalities, examples for testing the main features, and user documentation.
Example datasets and scripts refer to synthetic scenarios and real case studies.
Result visualization is supported through [ParaView](https://www.paraview.org/download/).

## Repository cloning

The repository includes external submodules required by MUSE. Clone it recursively with:

```bash
git clone --recursive git@bitbucket.org:marianna_miola/muse.git
cd MUSE
```

If the repository was cloned without the `--recursive` option, initialize and update the submodules from the repository root:

```bash
git submodule update --init --recursive
```

Throughout this README, `${ROOT}` denotes the root directory of the MUSE repository.

## Dependencies

MUSE relies on system dependencies and external libraries. The required system dependencies can be installed through the build scripts provided in the repository.

Some third-party libraries are included as submodules under `${ROOT}/external`, including tools for:

- command-line argument parsing and option management, through `tclap`;
- workflow metadata and persistence, through `cereal`;
- geostatistical and stochastic computation, through `geostatslib`;
- geospatial data management, through GDAL and PROJ;
- polygonal and polyhedral mesh processing, through `cinolib`, `libigl`, Triangle, and TetGen.

MUSE also uses ParaView Python scripting and the ParaView graphical interface for result visualization.
ParaView is not required to compile the core applications, but it is recommended for post-processing, visualization, and customization of the outputs.

## Build and executable generation

From the MUSE root directory, run the following commands to install the required system dependencies, build the external dependencies, and generate the MUSE executables:

```bash
cd MUSE
./00_installDeps.sh
./00_buildDeps.sh all
./00_exe_creation.sh
```

The build workflow is organized as follows:

1. `00_installDeps.sh` installs the required system-level dependencies.
2. `00_buildDeps.sh all` compiles the external dependencies needed by the MUSE applications.
3. `00_exe_creation.sh` configures and builds the MUSE applications, creating the executable files.

Triangle and TetGen are managed through `cinolib` during CMake configuration; therefore, no standalone top-level Triangle or TetGen build is required.

If the scripts are not executable on your system, enable execution permissions before running them:

```bash
chmod +x 00_installDeps.sh 00_buildDeps.sh 00_exe_creation.sh
```

## Documentation

The complete set of options for the MUSE modules is available in the [MUSE manual](MUSE_Manual.pdf).

The compilation process also generates the documentation for the available applications. After building MUSE, the generated documentation can be found in one of the following directories, depending on the selected output format:

```text
MUSE/docs/html
MUSE/docs/md
```

The HTML documentation can be opened with a web browser, while the Markdown documentation can be read directly from the generated `.md` files.

## Examples

Example data and scripts are provided under the `${ROOT}/examples` directory.
Each example contains scripts to run the corresponding MUSE workflow and, when available, to visualize the results.

A complete example can be run as follows:

```bash
cd MUSE/examples/05_prism_3D_tet/script
./10_test_MUSE_05_prism_3D_tet.sh project
```

The results of this example are saved in:

```text
MUSE/examples/MUSE_test/05_prism_3D_tet
```

More generally, examples can be executed from the corresponding `script` directory using the `10_test_*` script:

```bash
cd ${ROOT}/examples/<example_name>/script
./10_test_<example_name>.sh
```

When a visualization script is provided, results can be visualized by running the corresponding `20_test_*` script from the same `script` directory:

```bash
./20_test_<example_name>.sh
```

## Authors and contacts

- Marianna Miola, CNR-IMATI, Genova, Italy, marianna.miola@cnr.it
- Daniela Cabiddu, CNR-IMATI, Genova, Italy, daniela.cabiddu@cnr.it
- Simone Pittaluga, CNR-IMATI, Genova, Italy
- Marino Vetuschi Zuccolini, DISTAV-UNIGE, Genova, Italy

## Citation

If you use MUSE in academic work, please cite the following references.

```bibtex
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

@phdthesis{MIOLA2025PHD,
title = {MUSE: Modeling Uncertainty as a Support for Environment},
author = {Marianna Miola},
year = {2025},
school = {Università degli Studi di Genova (UNIGE)},
address = {Genova, Italy}
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
title = {EWoPe: A light Embeddable WOrkflow PErsistence tool for geoscientific pipeline reproducibility},
journal = {Computers & Geosciences},
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
