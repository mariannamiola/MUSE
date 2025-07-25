#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

####################################################################
### 0) CONVERT XLS FILE IN CSV (rimuovere commento per usarlo)
###./00_convert_data.sh
####################################################################

export DATA=BHSLMelfi
#unzip ${SCRIPT_DIR}/../data/${DATA}.zip -d ${SCRIPT_DIR}/../data

### 1) DATA SYNCRONIZATION (TIME = 12h)
#./01_syncro_data.sh

### 2) DATA FILTER (TIME WINDOW = 12h) AND FILE COMPOSITION
#./02_filter_data.sh

### 3) DATA APPEND
#./03_append_data.sh

### 4) MUSE RUN
./04_test_MUSE_20_DGM_SMargherita.sh project -D ${SCRIPT_DIR}/../data/${DATA}_frame_out

rm -r ${SCRIPT_DIR}/../data/${DATA}_frame_tmp
rm -r ${SCRIPT_DIR}/../data/${DATA}_frame
