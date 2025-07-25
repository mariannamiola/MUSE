#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export DATASOURCE=${SCRIPT_DIR}/../data
export FILENAME=Sant_Ana_all

ssconvert -O 'separator=; format=automatic quoting-mode=never' ${DATASOURCE}/${FILENAME}.xlsx ${DATASOURCE}/${FILENAME}.txt

cat ${DATASOURCE}/${FILENAME}.txt | sed '1 d ' | sed -e 's/ /_/g' | sed -e 's/,/./g' > ${DATASOURCE}/${FILENAME}.csv
cat ${DATASOURCE}/header.csv ${DATASOURCE}/${FILENAME}.csv > ${DATASOURCE}/${FILENAME}_format.csv
