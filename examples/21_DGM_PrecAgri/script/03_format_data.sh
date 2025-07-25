#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export DATASOURCE=${SCRIPT_DIR}/../data
export DATASPLIT=${DATASOURCE}/data_split

export OUTFOLDER=${DATASOURCE}/data_format
export OUTFOLDER1=${DATASOURCE}/data_frame
mkdir ${OUTFOLDER}
mkdir ${OUTFOLDER1}


declare -a VAR=("h" "T")
declare -a SENSOR=("21044" "21045" "21046" "21047" "21048" "21261" "21262" "21263" "21264" "21265")

cd ${DATASPLIT}
for file in *.csv
do
    frame="${file%.*}"
    frame="${frame##*_}"
    echo ${frame}
    
    id=${file:3:5}
    echo ${id}
    
    #var=${file:14:1}
    #echo ${var}
    
    export FILE=ID_${id}_date_${frame}
    cat ${FILE}.csv |  sed '1d' > ${FILE}_tmp.csv
    
    echo "CONVERT coordinates"
    awk 'BEGIN {FS=";"} {print $1}' ../ID_${id}.csv > ../ID_labels.csv
    awk 'BEGIN {FS=";"} {print $3,$2}' ../ID_${id}.csv | cs2cs EPSG:4326  EPSG:32633 | awk 'BEGIN{OFS=";"} {print $1,$2}' > ../ID_${id}_UTM.csv
    awk 'BEGIN {FS=";"} {print $4}' ../ID_${id}.csv > ../ID_z.csv

    mkdir -p ${OUTFOLDER}/${frame}
    paste -d ";" ../ID_labels.csv ../ID_${id}_UTM.csv ../ID_z.csv ${FILE}_tmp.csv > ${OUTFOLDER}/${frame}/ID_${id}_${frame}.csv
    rm ${FILE}_tmp.csv
    rm ../ID_labels.csv
    rm ../ID_z.csv
done

cd ${OUTFOLDER}
for f in *
do
    echo $f
    cat ../header.csv ${f}/ID_${SENSOR[0]}_$f.csv ${f}/ID_${SENSOR[1]}_$f.csv ${f}/ID_${SENSOR[2]}_$f.csv ${f}/ID_${SENSOR[3]}_$f.csv ${f}/ID_${SENSOR[4]}_$f.csv ${f}/ID_${SENSOR[5]}_$f.csv ${f}/ID_${SENSOR[6]}_$f.csv ${f}/ID_${SENSOR[7]}_$f.csv ${f}/ID_${SENSOR[8]}_$f.csv ${f}/ID_${SENSOR[9]}_$f.csv > ${OUTFOLDER1}/$f.csv
done




