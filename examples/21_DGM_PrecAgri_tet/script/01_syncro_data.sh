#!/bin/bash

### Script che richiama lo script python: 01_syncro_data.py per sincronizzare i dati

# USAGE: source 01_syncro_data.sh

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export DATASOURCE=${SCRIPT_DIR}/../data
export FOLDER=data_convert
export TIME='24h'
export OUTFOLDER=data_syncro

mkdir -p ${DATASOURCE}/${OUTFOLDER}

cd ${DATASOURCE}/${FOLDER}
for file in *
do
    export LABELH=$(cat ${file} | sed '1q' | awk 'BEGIN {FS=";"} {print $2}')
    #export LABELT=$(cat ${file} | sed '1q' | awk 'BEGIN {FS=";"} {print $3}')
    echo ${LABELH} ${LABELT}
    ${SCRIPT_DIR}/./01_syncro_data.py ${file} ${TIME} ${OUTFOLDER} ${LABELH} #${LABELT}
done
