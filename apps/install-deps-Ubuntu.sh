#!/bin/bash

git clone https://github.com/alandefreitas/matplotplusplus.git
cd matplotplusplus/
mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O2"
cmake --build . --parallel 20 --config Release
sudo cmake --install .

sudo apt-get install pkg-config
sudo apt-get install liblz4-dev 
sudo apt-get install libarmadillo-dev 
sudo apt-get install libgdal-dev
sudo apt-get install libeigen3-dev
sudo apt-get install libvtk9-dev 
sudo apt-get install libflann-dev
sudo apt-get install sqlite3

