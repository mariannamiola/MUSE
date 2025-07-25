#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export FILE=BHSL_Melfi
export NODE=TH0
#export VAR=T
export HEADER=header

export DATASOURCE=${SCRIPT_DIR}/../data/BHSLMelfi

export OUTFOLDER=${DATASOURCE}/_syncro
mkdir ${OUTFOLDER}

### FILTER DATA FOR DATE-TIME
###https://stackoverflow.com/questions/29446192/how-to-covert-the-result-bin-date-y-m-d-hms-into-seconds
#export START_FILTER="2023-05-01 00:00:00"
#export END_FILTER="2023-12-27 00:00:00"
#export offset=43200 ##numero di secondi per una ora

#echo "START_FILTER: " ${START_FILTER}
#echo "END_FILTER  : " ${END_FILTER}
#echo "OFFSET (sec): " ${offset}
echo

export counter=0
export N_NODES=5
for ((i=1; i<= ${N_NODES}; i++));
do
    echo "############################################## "
    echo "### SENSOR: " ${NODE}${i}
    
    export NAME_FILE=${FILE}_${NODE}${i}_merge_syncro
    
    cat ${DATASOURCE}/${NAME_FILE}.csv | sed ' 1d' | while read line 
    do
        let counter++
        
    	echo $line > ${DATASOURCE}/line.csv
    	
    	#filename=$(cat ${DATASOURCE}/line.csv | awk -v FS=';' '{print $1}' | sed 's/-//g' | sed 's/://g' | sed 's/ //g')
    	#echo $filename
   	
   	#echo "SAVE FILE: "
   	echo "### Processing row n°: " ${counter}
   	paste -d ";" ${DATASOURCE}/../${NODE}${i}.csv ${DATASOURCE}/line.csv > ${OUTFOLDER}/${NODE}${i}_merge_syncro_${counter}.csv
    done
    echo "############################################## "
    echo
done 



export nrow=$(cat ${DATASOURCE}/${FILE}_${NODE}1_merge_syncro.csv | sed ' 1d' | wc -l)

export OUTFRAME=${DATASOURCE}/../BHSLMelfi_frame
mkdir -p ${OUTFRAME}

echo "############################################## "
echo "### MERGING: "     
for ((w=1; w<= ${nrow}; w++));
do
    echo "### Merging n°: " ${w}
    filename=$(cat ${OUTFOLDER}/${NODE}1_merge_syncro_${w}.csv | awk -v FS=';' '{print $5}' | sed 's/-//g' | sed 's/://g' | sed 's/ //g')
    
    cat ${OUTFOLDER}/${NODE}1_merge_syncro_${w}.csv ${OUTFOLDER}/${NODE}2_merge_syncro_${w}.csv ${OUTFOLDER}/${NODE}3_merge_syncro_${w}.csv ${OUTFOLDER}/${NODE}4_merge_syncro_${w}.csv ${OUTFOLDER}/${NODE}5_merge_syncro_${w}.csv > ${OUTFRAME}/${filename}.csv
done
echo "############################################## "


rm ${OUTFOLDER}/*.csv

