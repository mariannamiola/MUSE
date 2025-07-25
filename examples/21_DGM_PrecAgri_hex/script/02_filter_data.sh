#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export HEADER=header

export DATASOURCE=${SCRIPT_DIR}/../data
export DATASYNCRO=${DATASOURCE}/data_syncro

export OUTFOLDER=${DATASOURCE}/data_merge
export OUTFOLDER1=${DATASOURCE}/data_transpose
export OUTFOLDER2=${DATASOURCE}/data_split

mkdir ${OUTFOLDER}
mkdir ${OUTFOLDER1}
mkdir ${OUTFOLDER2}


declare -a VAR=("h" "T")
declare -a SENSOR=("21044" "21045" "21046" "21047" "21048" "21261" "21262" "21263" "21264" "21265")


export N_NODES=6
for i in "${SENSOR[@]}" 
do
    for j in "${VAR[@]}"
    do
        awk 'BEGIN {FS=";"} {print $1}' ${DATASYNCRO}/ID_${i}_date_${j}1.csv > ${OUTFOLDER}/ID_${i}_date_${j}.csv
        
        for ((n=1; n<= ${N_NODES}; n++));
        do
    	    export FILE=ID_${i}_date_${j}${n}
    	    echo "############################################## "
    	    echo "### SENSOR: " ${FILE}
    	
    	    awk 'BEGIN {FS=";"} {print $2}' ${DATASYNCRO}/${FILE}.csv | sed '1d' > ${DATASYNCRO}/var_tmp.csv

	        echo ${j}${n} > ${DATASYNCRO}/label.csv
	        cat ${DATASYNCRO}/label.csv ${DATASYNCRO}/var_tmp.csv > ${DATASYNCRO}/var.csv

    	    paste -d ";" ${OUTFOLDER}/ID_${i}_date_${j}.csv ${DATASYNCRO}/var.csv > ${OUTFOLDER}/ID_${i}_date_${j}_tmp.csv
    	    cp ${OUTFOLDER}/ID_${i}_date_${j}_tmp.csv ${OUTFOLDER}/ID_${i}_date_${j}.csv
    	
    	    rm ${OUTFOLDER}/ID_${i}_date_${j}_tmp.csv
    	    rm ${DATASYNCRO}/var_tmp.csv
    	done
    done
done
#
#
#
#
cd ${OUTFOLDER}
for file in *
do 
  echo $file
  ${DATASOURCE}/./00_transpose_data.py data_merge ${file%%.*} .csv data_transpose
done

cd ..
cd ${OUTFOLDER1}
for file in *
do   
  echo "SPLIT data"
  columns=$(cat ${file} | awk -v FS=';' '{print NF}' | tail -n 1)
    
  id=${file:3:5}
  #echo ${id}
    
  var=${file:14:1}
  #echo ${var}
  mkdir -p ${OUTFOLDER2}/${var}
  
  for ((c=2; c<= ${columns}; c++));
  do
    cat ${file} | sed '1d' | awk -v col="$c" 'BEGIN {FS=";"} {print $col}' > ${OUTFOLDER2}/${var}/ID_${id}_date_tmp.csv
    
    frame=$(cat ${OUTFOLDER2}/${var}/ID_${id}_date_tmp.csv |  sed '1q' | sed 's/-//g' | sed 's/://g' | sed 's/ //g')
    #echo ${frame}
    mv ${OUTFOLDER2}/${var}/ID_${id}_date_tmp.csv ${OUTFOLDER2}/${var}/ID_${id}_date_${frame}.csv
  done
done


cd ${OUTFOLDER2}/h
for file in *
do   
    frame="${file%.*}"
    frame="${frame##*_}"
    echo ${frame}
    
    id=${file:3:5}
    echo ${id}
    
    export filename=ID_${id}_date_${frame}
    paste -d ";" ${filename}.csv ../T/${filename}.csv > ../${filename}.csv 
done

