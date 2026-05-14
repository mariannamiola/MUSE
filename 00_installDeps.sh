#!/usr/bin/env bash
set -euo pipefail

# Detect OS
OS="$(uname)"
if [[ "$OS" == "Darwin" ]]; then
    LIB_EXT="dylib"
else
    LIB_EXT="so"
fi


if [[ "$LIB_EXT" == "so" ]]; then
    # Linux
    sudo apt-get update
    sudo apt-get install build-essential cmake -y
    sudo apt-get install liblz4-dev -y
    sudo apt-get install libarmadillo-dev -y 
    sudo apt-get install libeigen3-dev -y
    sudo apt-get install sqlite3 libsqlite3-dev -y
else
    # macOS
    brew update
    brew install cmake lz4 armadillo eigen sqlite3
fi