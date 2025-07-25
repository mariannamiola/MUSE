#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

cd ${SCRIPT_DIR}
##./00_convert_data.sh
./01_syncro_data.sh
./02_filter_data.sh
./03_append_data.sh
