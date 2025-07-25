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
export START_FILTER="2023-05-01 00:00:00"
export END_FILTER="2023-12-27 00:00:00"
export offset=43200 ##numero di secondi per una ora
#export N_TIMEWINDOWS=3

echo "START_FILTER: " ${START_FILTER}
echo "END_FILTER  : " ${END_FILTER}
echo "OFFSET (sec): " ${offset}
echo

export N_NODES=5
#declare -a arr=(${NODE}1 ${NODE}2 ${NODE}3 ${NODE}4 ${NODE}5 ${NODE}6 ${NODE}7 ${NODE}8 ${NODE}9)
#for i in "${arr[@]}"
for ((i=1; i<= ${N_NODES}; i++));
do
    echo "SENSOR: " ${NODE}${i}
    echo "TIME WINDOWS: ---------------------"
    #export NODE_FOLDER=${DATASOURCE}/${NODE}${i}

    export NAME_FILE=${FILE}_${NODE}${i}_merge_syncro
    #cat ${DATASOURCE}/${NAME_FILE}.csv | sed ' 1d' > ${DATASOURCE}/${NAME_FILE}rev00.csv
    
    counter=0
    if [[ $OSTYPE == 'darwin'* ]]; then
    	e_start="$(date -jf '%Y-%m-%d %T' "$START_FILTER" '+%s')"
    	e_end="$(date -jf '%Y-%m-%d %T' "$END_FILTER" '+%s')"
    
    	dateTs=$e_start
    	while [ "$dateTs" -le "$e_end" ]
    	do
    		let counter++
		
        	date=`date -j -f "%s" $dateTs "+%Y-%m-%d-%T"`
        	dateTs=$(($dateTs+$offset))
        	date_next=`date -j -f "%s" $dateTs "+%Y-%m-%d-%T"`
        	
        	if [[ "$dateTs" -le "$e_end" ]]; then
        		echo "ID time window: " ${counter}
        		echo "START_DATE: " $date
        		echo "END_DATE  : " $date_next
        		#echo $date > ${DATASOURCE}/sdate.csv
        		#echo $date_next > ${DATASOURCE}/edate.csv
  
	        	cat ${DATASOURCE}/${NAME_FILE}.csv | sed ' 1d' | sed -e 's/ /-/g' | awk -v start=$date -v end=$date_next -v FS=';' '$1 >= start && $1 <end' > ${DATASOURCE}/${NAME_FILE}_filter_$counter.csv
	  
    	        #cat ${DATASOURCE}/${NAME_FILE}_filter_$counter.csv | awk -F '[;]' '{ total += $2 } END { print total/NR }' |  sed -e 's/,/./g' > ${DATASOURCE}/${NAME_FILE}_filter_${counter}_mean.csv
	        
	        	#paste -d ";" ${DATASOURCE}/sdate.csv ${DATASOURCE}/edate.csv ${DATASOURCE}/${NODE}${i}.csv > ${DATASOURCE}/tmp.csv
	        	#paste -d ";" ${DATASOURCE}/tmp.csv ${DATASOURCE}/${NAME_FILE}_filter_${counter}.csv > ${DATASOURCE}/${NAME_FILE}_${counter}.csv
                paste -d ";" ${DATASOURCE}/../${NODE}${i}.csv ${DATASOURCE}/${NAME_FILE}_filter_${counter}.csv > ${OUTFOLDER}/${NAME_FILE}_${counter}.csv
                rm ${DATASOURCE}/${NAME_FILE}_filter_${counter}.csv
        	fi
	done
    else
    	e_start=$(date -d "$START_FILTER" +%s)
    	e_end=$(date -d "$END_FILTER" +%s)
    	    	
	dateTs=$e_start
    	while [ "$dateTs" -le "$e_end" ]
    	do
		let counter++
		
        	date=$(date -d @$dateTs +"%Y-%m-%d-%T")	
        	dateTs=$(($dateTs+$offset))
        	date_next=$(date -d @$dateTs +"%Y-%m-%d-%T")
        	
        	if [[ "$dateTs" -le "$e_end" ]]; then
        		echo "ID time window: " ${counter}
        		echo "START_DATE: " $date
        		echo "END_DATE  : " $date_next
        		#echo $date > ${DATASOURCE}/sdate.csv
        		#echo $date_next > ${DATASOURCE}/edate.csv
        	
			cat ${DATASOURCE}/${NAME_FILE}.csv | sed ' 1d' | sed -e 's/ /-/g' | awk -v start=$date -v end=$date_next -v FS=';' '$1 >= start && $1 < end' > ${DATASOURCE}/${NAME_FILE}_filter_$counter.csv

			#cat ${DATASOURCE}/${NAME_FILE}_filter_$counter.csv | awk -F '[;]' '{ total += $2 } END { print total/NR }' |  sed -e 's/,/./g' > ${DATASOURCE}/${NAME_FILE}_filter_${counter}_mean.csv

			#paste -d ";" ${DATASOURCE}/sdate.csv ${DATASOURCE}/edate.csv ${DATASOURCE}/${NODE}${i}.csv > ${DATASOURCE}/tmp.csv
			#paste -d ";" ${DATASOURCE}/tmp.csv ${DATASOURCE}/${NAME_FILE}_filter_${counter}.csv > ${DATASOURCE}/${NAME_FILE}_${counter}.csv
			paste -d ";" ${DATASOURCE}/../${NODE}${i}.csv ${DATASOURCE}/${NAME_FILE}_filter_${counter}.csv > ${OUTFOLDER}/${NAME_FILE}_${counter}.csv
			rm ${DATASOURCE}/${NAME_FILE}_filter_${counter}.csv
		fi
	 done
    fi
    
    #rm ${DATASOURCE}/${NAME_FILE}rev00.csv
    #rm ${DATASOURCE}/tmp.csv
    #rm ${DATASOURCE}/sdate.csv
    #rm ${DATASOURCE}/edate.csv
    
    #echo "n. time windows: " ${counter}
    echo "-----------------------------------"
    echo
    
done

export OUTFRAME=${DATASOURCE}/../BHSLMelfi_frame
mkdir ${OUTFRAME}

for ((w=1; w< ${counter}; w++));
do
    cat ${OUTFOLDER}/${FILE}_${NODE}1_merge_syncro_${w}.csv ${OUTFOLDER}/${FILE}_${NODE}2_merge_syncro_${w}.csv ${OUTFOLDER}/${FILE}_${NODE}3_merge_syncro_${w}.csv ${OUTFOLDER}/${FILE}_${NODE}4_merge_syncro_${w}.csv ${OUTFOLDER}/${FILE}_${NODE}5_merge_syncro_${w}.csv > ${OUTFRAME}/${FILE}_merge_syncro_${w}.csv
done
#${DATASOURCE}/../${HEADER}.csv 

rm ${OUTFOLDER}/*.csv
