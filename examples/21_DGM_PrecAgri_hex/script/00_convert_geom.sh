#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export DATASOURCE=${SCRIPT_DIR}/../data
export FILENAME=area

cat ${DATASOURCE}/${FILENAME}.csv | sed -e 's/,/;/g' | awk 'BEGIN {FS=";"} {print $2,$1}' | cs2cs EPSG:4326  EPSG:23033 | awk 'BEGIN{OFS=";"} {print $1,$2}' > ${DATASOURCE}/area_UTM.xyz ####EPSG:23033
cat ${DATASOURCE}/${FILENAME}.csv | sed -e 's/,/;/g' | awk 'BEGIN {FS=";"} {print $3}' > ${DATASOURCE}/z.xyz

paste -d ";" ${DATASOURCE}/area_UTM.xyz ${DATASOURCE}/z.xyz > ${DATASOURCE}/area_tmp.xyz
cat ${DATASOURCE}/area_tmp.xyz | sed -e 's/;/ /g' > ${DATASOURCE}/area.xyz

rm ${DATASOURCE}/area_tmp.xyz
rm ${DATASOURCE}/area_UTM.xyz
rm ${DATASOURCE}/z.xyz
