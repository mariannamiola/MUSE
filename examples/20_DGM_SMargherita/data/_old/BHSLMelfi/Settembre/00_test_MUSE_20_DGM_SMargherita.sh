#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export FILE=Temperatura_Ambiente
export NODE=TH0
export HEADER=header

export DATASOURCE=${SCRIPT_DIR}/../data/BHSLMelfi/Settembre



### FILTER DATA FOR DATE-TIME
###https://stackoverflow.com/questions/29446192/how-to-covert-the-result-bin-date-y-m-d-hms-into-seconds
export START_FILTER="2023-04-11 00:00:00"
export END_FILTER="2023-04-20 00:00:00"
export offset=21600 ##numero di secondi per un giorno
#export N_TIMEWINDOWS=3

echo "START_FILTER: " ${START_FILTER}
echo "END_FILTER  : " ${END_FILTER}
echo "OFFSET (sec): " ${offset}
echo

export N_NODES=5
for ((i=1; i<= ${N_NODES}; i++));
do
    echo "SENSOR: " ${NODE}${i}
    echo "TIME WINDOWS: ---------------------"
    export NODE_FOLDER=${DATASOURCE}/${NODE}${i}

    cat ${NODE_FOLDER}/${FILE}.csv | sed ' 1d' > ${NODE_FOLDER}/${FILE}rev00.csv
    
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
        		echo $date > ${NODE_FOLDER}/sdate.csv
        		echo $date_next > ${NODE_FOLDER}/edate.csv
  
	        	cat ${NODE_FOLDER}/${FILE}rev00.csv | sed -e 's/ /-/g' | awk -v start=$date -v end=$date_next -v FS=';' '$1 >= start && $1 <end' > ${NODE_FOLDER}/${FILE}_filter_$counter.csv
	  
	        	cat ${NODE_FOLDER}/${FILE}_filter_$counter.csv | awk -F '[;]' '{ total += $2 } END { print total/NR }' |  sed -e 's/,/./g' > ${NODE_FOLDER}/${FILE}_filter_${counter}_mean.csv
	        
	        	paste -d ";" ${NODE_FOLDER}/sdate.csv ${NODE_FOLDER}/edate.csv ${NODE_FOLDER}/${NODE}.csv > ${NODE_FOLDER}/tmp.csv
	        	paste -d ";" ${NODE_FOLDER}/tmp.csv ${NODE_FOLDER}/${FILE}_filter_${counter}_mean.csv > ${NODE_FOLDER}/${FILE}_${counter}.csv
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
        		echo $date > ${NODE_FOLDER}/sdate.csv
        		echo $date_next > ${NODE_FOLDER}/edate.csv
        	
			cat ${NODE_FOLDER}/${FILE}rev00.csv | sed -e 's/ /-/g' | awk -v start=$date -v end=$date_next -v FS=';' '$1 >= start && $1 < end' > ${NODE_FOLDER}/${FILE}_filter_$counter.csv

			cat ${NODE_FOLDER}/${FILE}_filter_$counter.csv | awk -F '[;]' '{ total += $2 } END { print total/NR }' |  sed -e 's/,/./g' > ${NODE_FOLDER}/${FILE}_filter_${counter}_mean.csv

			paste -d ";" ${NODE_FOLDER}/sdate.csv ${NODE_FOLDER}/edate.csv ${NODE_FOLDER}/${NODE}.csv > ${NODE_FOLDER}/tmp.csv
			paste -d ";" ${NODE_FOLDER}/tmp.csv ${NODE_FOLDER}/${FILE}_filter_${counter}_mean.csv > ${NODE_FOLDER}/${FILE}_${counter}.csv
		fi
	 done
    fi
    
    rm ${NODE_FOLDER}/${FILE}rev00.csv
    rm ${NODE_FOLDER}/tmp.csv
    rm ${NODE_FOLDER}/sdate.csv
    rm ${NODE_FOLDER}/edate.csv
    
    #echo "n. time windows: " ${counter}
    echo "-----------------------------------"
    echo
    
done


for ((w=1; w< ${counter}; w++));
do
    cat ${DATASOURCE}/${HEADER}.csv ${DATASOURCE}/${NODE}1/${FILE}_${w}.csv ${DATASOURCE}/${NODE}2/${FILE}_${w}.csv ${DATASOURCE}/${NODE}3/${FILE}_${w}.csv ${DATASOURCE}/${NODE}4/${FILE}_${w}.csv ${DATASOURCE}/${NODE}5/${FILE}_${w}.csv #${DATASOURCE}/${NODE}6/${FILE}_${w}.csv ${DATASOURCE}/${NODE}7/${FILE}_${w}.csv ${DATASOURCE}/${NODE}8/${FILE}_${w}.csv ${DATASOURCE}/${NODE}9/${FILE}_${w}.csv ${DATASOURCE}/${NODE}10/${FILE}_${w}.csv ${DATASOURCE}/${NODE}11/${FILE}_${w}.csv ${DATASOURCE}/${NODE}12/${FILE}_${w}.csv ${DATASOURCE}/${NODE}13/${FILE}_${w}.csv ${DATASOURCE}/${NODE}14/${FILE}_${w}.csv ${DATASOURCE}/${NODE}15/${FILE}_${w}.csv > ${DATASOURCE}/${FILE}_merge${w}.csv
done

