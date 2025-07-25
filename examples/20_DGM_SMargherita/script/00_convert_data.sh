#!/bin/bash

### Script che converte i dati da xls a csv

# USAGE: source 00_convert_data.sh


#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export DATASOURCE=${SCRIPT_DIR}/../data
export FOLDER=BHSLMelfi
export name_file=BHSL_Melfi

cd ${DATASOURCE}/${FOLDER}
for mese in *
do
    echo "MESE: " ${mese}
    cd ${mese}
   
    for node in *
    do
        echo "NODE: " ${node}
        cd ${node}
   
   	### CONVERT EXCEL FILE IN TXT
        for file in *.xls
        do
            echo $file
            ssconvert -O 'separator=; format=automatic quoting-mode=never' $file ${file%%.*}.txt
            rm $file
        done

	### CONVERT EXCEL FILE IN TXT
        for file in *.txt
        do
            #echo $file
            cat $file | sed 's/,/./g' > ${file%%.*}.csv #| | sed 's/ /-/g sed ' 1d'
            rm $file
        done
        
        #find . -maxdepth 1 -iname '*.csv' -not -name 'Melfi_'${mese}'_01.csv' -exec cat {} +>merge
        awk 'NR==1||FNR>1' ${name_file}_*.csv > ${name_file}_${mese}_${node}_merge.csv
        cd ..
    done
    cd ..
done


cd ${DATASOURCE}/${FOLDER}
export -a declare node_name=("TH01" "TH02" "TH03" "TH04" "TH05")
for node in "${node_name[@]}"
do
    awk 'NR==1||FNR>1' Agosto/${node}/${name_file}_Agosto_${node}_merge.csv Dicembre/${node}/${name_file}_Dicembre_${node}_merge.csv Giugno/${node}/${name_file}_Giugno_${node}_merge.csv Luglio/${node}/${name_file}_Luglio_${node}_merge.csv Maggio/${node}/${name_file}_Maggio_${node}_merge.csv Novembre/${node}/${name_file}_Novembre_${node}_merge.csv Ottobre/${node}/${name_file}_Ottobre_${node}_merge.csv Settembre/${node}/${name_file}_Settembre_${node}_merge.csv > ${name_file}_${node}_merge.csv
done
