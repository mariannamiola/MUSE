#!/bin/bash

export HEADER=header

export FILENAME2=bisagno_log
export FILENAME3=bis_bedrock_DTM
export FILENAME4=bis_bedrock_soft
export FILENAME5=ds99_sondaggi

export FILENAME_OUT=log_samples

#cat ${FILENAME1_COORD}.ascii | sed -e 's/ /;/g' > ${FILENAME1_COORD}.csv

#awk 'BEGIN {FS=";"} {print $1,$2,$3}' cat ${FILENAME1_COORD}.ascii | sed -e 's/ /;/g' > coords.csv
#awk 'BEGIN {FS=";"} {print $4}' cat ${FILENAME1_COORD}.ascii | sed -e 's/ /;/g' > categ.csv
#paste -d ';' coords.csv ${FILENAME1_CAT}.csv > ${FILENAME1}.csv

cat ${FILENAME2}.dat | sed -e 's/^  //g' | sed -e 's/    /;/g' | sed -e 's/   /;/g' | sed -e 's/  /;/g' | sed -e 's/ /;/g' > ${FILENAME2}.csv

cat ${FILENAME3}.dat | sed -e 's/^   //g' | sed -e 's/^  //g' | sed -e 's/     /;/g' | sed -e 's/    /;/g' | sed -e 's/   /;/g' | sed -e 's/  /;/g' | sed -e 's/ /;/g' | sed -e 's/;6;/;5;/g' > ${FILENAME3}.csv

cat ${FILENAME4}.dat | sed -e 's/^   //g' | sed -e 's/^  //g' | sed -e 's/      /;/g' | sed -e 's/     /;/g' | sed -e 's/    /;/g' | sed -e 's/   /;/g' | sed -e 's/  /;/g' | sed -e 's/ /;/g' | sed -e 's/;6;/;5;/g' > ${FILENAME4}.csv

cat ${FILENAME5}.dat | sed -e 's/^  //g' | sed -e 's/      /;/g' | sed -e 's/     /;/g' | sed -e 's/    /;/g' | sed -e 's/   /;/g' | sed -e 's/  /;/g' | sed -e 's/ /;/g' > ${FILENAME5}.csv

#cat ${FILENAME5}.csv | sed '1d' | sed -e 's/|/;/g' > ${FILENAME5}_.csv

cat ${HEADER}.csv ${FILENAME2}.csv ${FILENAME3}.csv ${FILENAME4}.csv ${FILENAME5}.csv > ../${FILENAME_OUT}.csv
cat ${FILENAME2}.csv ${FILENAME3}.csv ${FILENAME4}.csv ${FILENAME5}.csv > ../samples.csv
