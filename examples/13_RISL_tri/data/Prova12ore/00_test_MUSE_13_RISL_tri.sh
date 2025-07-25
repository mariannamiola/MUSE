#!/bin/bash

export SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
#export DATA_DIR=${SCRIPT_DIR}/data/Prova12ore

echo ${SCRIPT_DIR}
echo ${DATA_DIR}

#cd ${DATA_DIR}

for j in `ls | grep "[0-9]\{12\}_UTM".csv`
do
    echo ${j}
    ./../../script/10_test_MUSE_13_RISL_tri.sh project -D ${SCRIPT_DIR} -d ${j} -p 13_RISL_tri_${j%%.*} -s 2
done