#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export DATA=data_frame

export DATAREPL=true ##set on false to skip data/geometry unpackaging-copy-processing


### 2) MUSE RUN
cd ${SCRIPT_DIR}
./01_test_MUSE_21_DGM_PrecAgri.sh project -D ${SCRIPT_DIR}/../data/${DATA} -s 10 -r ${DATAREPL}
