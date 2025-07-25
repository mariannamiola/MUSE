# MUSE - Stochastic-based tool for Modeling Uncertainty as a Support for Enviroments
MUSE is an open-source stochastic tool for modeling uncertainty as a support for multi-scenario applications in Environments.

MUSE is an embedded package (all useful functionalities are self-contained): for this reason, the process from input to output is automatic and implicit, without further coding complications.
Combining state-of-the-art techniques and advanced algorithms (e.g., parallel computing) guarantees robustness and, at the same time, high computing performances.
The code is developed in C++ programming language and is tested on different Operating Systems (MacOS and Linux).

MUSE is featured by flexible and concise language in a command-line interface.
The repository contains the entire MUSE package, such as source code for each computational module, a C++ library to collect all functionalities, a set of examples to test the main features, and the documentation. 
Referring to examples, execution script and data are related to synthetical scenarios or real case studies.
Results visualization is supported by [Paraview](https://www.paraview.org/download/). 

## Clone
We provide the commands to install MUSE. 
The repository includes some submodules necessary to make the code work. Please, clone it recursively:

- Clone recursively the repository into your local machine:
```
git clone --recursive git@bitbucket.org:marianna_miola/muse.git
```
- If some submodules are not clone/update, plese use git command in the root directory ROOT (where this README lies):
```
cd ${ROOT}
git submodule update --init --recursive
```

## Usage
MUSE code has some mandatory dependences (included as a submodule in _${ROOT}/external_):

- to manage command line arguments and options (tclap);
- to metadata the computational process (cereal);
- to geostatistics and stochastic computation (geostatslib);
- to manage geospatial data (GDAL, PROJ);
- to process polygonal/polyhedral meshes and offers geometric processing tools (cinolib, libigl, Triangle, Tetgen, fTetWild).

MUSE employs _Paraview_ python scripting and graphical interaface to visualize results.
Please, download it to use additional functionalities of results visualization and customization.

## Documentation
The complete set of options for each MUSE module is available [here](MUSE_Manual.pdf). 
MUSE can be easily built (see __Building__ section).

## Building
To build MUSE, use the `00_exe_creation.sh` script available, located in the root directory ROOT (where this README lies). 
It is set to build the code and some external dependencies (_Triangle, Tetgen, fTetWild_) that need building.

## Examples
To guarantee replicability, data of our examples are provided in the _example_ folder. 
If you wish to replicate them, please run bash script file (_10_test_MUSE_example_name.sh_), as follows:

```
cd ${ROOT}/examples/example_name/script
./10_test_MUSE_example_name.sh
```

Results will be available in the _${ROOT}/examples/MUSE_test/example_name_ folder.
To visualize results, please run bash script file (_20_test_MUSE_example_name.sh_) located in _script_ folder of the related example:

```
./20_test_MUSE_example_name.sh
```

## Authors contact
- Marianna Miola (DISTAV-UNIGE, Genova, Italy), email: marianna.miola@edu.unige.it
- Daniela Cabiddu (CNR-IMATI, Genova, Italy)
- Simone Pittaluga (CNR-IMATI, Genova, Italy)
- Marino Vetuschi Zuccolini (DISTAV-UNIGE, Genova, Italy)

## Citing us
If you use MUSE in your academic projects, please consider citing us using the following BibTeX entry:
```
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

## Acknowledgment
This work is supported by PON "Ricerca e Innovazione" 2014-2020, Asse IV "Istruzione e ricerca per il recupero", Azione IV.5 "Dottorati su tematiche green" DM 1061/2021.
