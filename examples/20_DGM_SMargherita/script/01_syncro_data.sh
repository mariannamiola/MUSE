#!/bin/bash

### Script che richiama lo script python: 01_syncro_data.py per sincronizzare i dati

# USAGE: source 01_syncro_data.sh

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export DATASOURCE=${SCRIPT_DIR}/../data
export FOLDER=BHSLMelfi
export N_NODES=5
export TIME='12h'

for ((n=1; n<= ${N_NODES}; n++))
do
	echo 'BHSL_Melfi_TH0'${n}'_merge'
	${DATASOURCE}/./01_syncro_data.py ${FOLDER} 'BHSL_Melfi_TH0'${n}'_merge' .csv ${TIME}
done
