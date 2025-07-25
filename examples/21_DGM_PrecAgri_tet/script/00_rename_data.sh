#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export DATASOURCE=${SCRIPT_DIR}/../data
export FOLDER=data
export FOLDER1=data_convert
export FOLDER2=data_transpose


cd ${DATASOURCE}
mkdir -p ${FOLDER1}

cd ${FOLDER}
for oldname in *
do 
  newname=`echo $oldname | sed -e 's/ //g'` 
  mv "$oldname" "${newname:0:8}.csv"
  
  ##ssconvert -O 'separator=; format=automatic quoting-mode=never' $newname ../${FOLDER1}/${newname%%.*}.txt
done

for file in *
do 
  echo $file
  ##export FILENAME=../${FOLDER1}/${file:0:8}.csv
  export FILENAME=../${FOLDER1}/${file%%.*}
  
  #if [[ $OSTYPE != 'darwin'* ]]; then
  dos2unix -f $file
  #fi

  cat $file | sed '1 d ' | sed 's/ °C//g' | sed -e 's/ /_/g' > ${FILENAME}.csv

  echo "SPLIT data"
  awk 'BEGIN {FS=";"} {print $1}' ${FILENAME}.csv | sed -e 's/_/ /g' > ${FILENAME}_date.csv
  ##cat ${FILENAME}_date_tmp.csv | sed -e 's/_/;/g' | awk 'BEGIN {FS=";"} {print $1}' > ${FILENAME}_date.csv
  ##cat ${FILENAME}_date_tmp.csv | sed -e 's/_/;/g' | awk 'BEGIN {FS=";"} {print $2}' > ${FILENAME}_time.csv
  
  awk 'BEGIN {FS=";"} {print $2}' ${FILENAME}.csv > ${FILENAME}_h1.csv
  awk 'BEGIN {FS=";"} {print $3}' ${FILENAME}.csv > ${FILENAME}_h2.csv
  awk 'BEGIN {FS=";"} {print $4}' ${FILENAME}.csv > ${FILENAME}_h3.csv
  awk 'BEGIN {FS=";"} {print $5}' ${FILENAME}.csv > ${FILENAME}_h4.csv
  awk 'BEGIN {FS=";"} {print $6}' ${FILENAME}.csv > ${FILENAME}_h5.csv
  awk 'BEGIN {FS=";"} {print $7}' ${FILENAME}.csv > ${FILENAME}_h6.csv
  
  awk 'BEGIN {FS=";"} {print $8}' ${FILENAME}.csv > ${FILENAME}_T1.csv
  awk 'BEGIN {FS=";"} {print $9}' ${FILENAME}.csv > ${FILENAME}_T2.csv
  awk 'BEGIN {FS=";"} {print $10}' ${FILENAME}.csv > ${FILENAME}_T3.csv
  awk 'BEGIN {FS=";"} {print $11}' ${FILENAME}.csv > ${FILENAME}_T4.csv
  awk 'BEGIN {FS=";"} {print $12}' ${FILENAME}.csv > ${FILENAME}_T5.csv
  awk 'BEGIN {FS=";"} {print $13}' ${FILENAME}.csv > ${FILENAME}_T6.csv
  
  echo "PATCH all column"
  paste -d ";" ${FILENAME}_date.csv ${FILENAME}_h1.csv | sed 's/; /;/g' > ${FILENAME}_date_h1.csv
  paste -d ";" ${FILENAME}_date.csv ${FILENAME}_h2.csv | sed 's/; /;/g' > ${FILENAME}_date_h2.csv
  paste -d ";" ${FILENAME}_date.csv ${FILENAME}_h3.csv | sed 's/; /;/g' > ${FILENAME}_date_h3.csv
  paste -d ";" ${FILENAME}_date.csv ${FILENAME}_h4.csv | sed 's/; /;/g' > ${FILENAME}_date_h4.csv
  paste -d ";" ${FILENAME}_date.csv ${FILENAME}_h5.csv | sed 's/; /;/g' > ${FILENAME}_date_h5.csv 
  paste -d ";" ${FILENAME}_date.csv ${FILENAME}_h6.csv | sed 's/; /;/g' > ${FILENAME}_date_h6.csv
  
  paste -d ";" ${FILENAME}_date.csv ${FILENAME}_T1.csv | sed 's/; /;/g' > ${FILENAME}_date_T1.csv
  paste -d ";" ${FILENAME}_date.csv ${FILENAME}_T2.csv | sed 's/; /;/g' > ${FILENAME}_date_T2.csv
  paste -d ";" ${FILENAME}_date.csv ${FILENAME}_T3.csv | sed 's/; /;/g' > ${FILENAME}_date_T3.csv
  paste -d ";" ${FILENAME}_date.csv ${FILENAME}_T4.csv | sed 's/; /;/g' > ${FILENAME}_date_T4.csv
  paste -d ";" ${FILENAME}_date.csv ${FILENAME}_T5.csv | sed 's/; /;/g' > ${FILENAME}_date_T5.csv 
  paste -d ";" ${FILENAME}_date.csv ${FILENAME}_T6.csv | sed 's/; /;/g' > ${FILENAME}_date_T6.csv
  
  rm ${FILENAME}.csv
  
  rm ${FILENAME}_date.csv
  
  rm ${FILENAME}_h1.csv
  rm ${FILENAME}_h2.csv
  rm ${FILENAME}_h3.csv
  rm ${FILENAME}_h4.csv
  rm ${FILENAME}_h5.csv
  rm ${FILENAME}_h6.csv
  
  rm ${FILENAME}_T1.csv
  rm ${FILENAME}_T2.csv
  rm ${FILENAME}_T3.csv
  rm ${FILENAME}_T4.csv
  rm ${FILENAME}_T5.csv
  rm ${FILENAME}_T6.csv
  
#  rm ${FILENAME}_date_h1_tmp.csv
#  rm ${FILENAME}_date_h2_tmp.csv
#  rm ${FILENAME}_date_h3_tmp.csv
#  rm ${FILENAME}_date_h4_tmp.csv
#  rm ${FILENAME}_date_h5_tmp.csv
#  rm ${FILENAME}_date_h6_tmp.csv
  
done
