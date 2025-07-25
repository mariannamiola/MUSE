#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export DATA=ProvaLunga

export DATAREPL=true ##set on false to skip data/geometry unpackaging-copy-processing

if $DATAREPL; then
   cd ${SCRIPT_DIR}/../data/${DATA}
   #unzip ${DATA}.zip

   ### 1) DATA CONVERT
   ./01_convert_data_13_RISL_tri_mf.sh
fi

### 2) MUSE RUN
cd ${SCRIPT_DIR}
./00_test_MUSE_13_RISL_tri_mf.sh project -D ${SCRIPT_DIR}/../data/${DATA} -s 10 -r ${DATAREPL}
