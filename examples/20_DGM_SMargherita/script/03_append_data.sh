#!/bin/bash

### Script che richiama lo script python: 02_append_data.py per aggiungere nuovi dati

# USAGE: source 03_append_data.sh

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export DATASOURCE=${SCRIPT_DIR}/../data
export FOLDER=BHSLMelfi_frame
export FOLDER_TMP=BHSLMelfi_frame_tmp
export FOLDER_OUT=BHSLMelfi_frame_out

cd ${DATASOURCE}
mkdir ${FOLDER_OUT}
mkdir ${FOLDER_TMP}

cd ${FOLDER}
for file in *.csv
do
	echo ${file%%.*}
	${DATASOURCE}/./03_append_data.py ${FOLDER} ${file%%.*} .csv ${FOLDER_TMP}
done

cd ..
cd ${FOLDER_TMP}
for file in *.csv
do
	echo ${file%%.*}
	cat ${DATASOURCE}/header.csv ${file} > ${DATASOURCE}/${FOLDER_OUT}/${file%%.*}.csv
done
