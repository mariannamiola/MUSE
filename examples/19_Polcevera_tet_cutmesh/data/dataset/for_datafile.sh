#!/bin/bash

export HEADER=header
export FILENAME1=log_polcevera
export FILENAME2=log_rock
export FILENAME3=log_substratum
export FILENAME4=vrtbrhl
#export FILENAME5=punti_polcevera_aggiuntivi_quotato

export FILENAME_OUT=log_samples

cat ${FILENAME1}.dat | sed -e 's/^  //g' | sed -e 's/^ //g' | sed -e 's/        /;/g' | sed -e 's/       /;/g' | sed -e 's/      /;/g' | sed -e 's/     /;/g' | sed -e 's/    /;/g' | sed -e 's/   /;/g' | sed -e 's/  /;/g' | sed -e 's/ /;/g' > ${FILENAME1}.csv

#cat ${FILENAME2}.dat | sed -e 's/^  //g' | sed -e 's/^ //g' | sed -e 's/        /;/g' | sed -e 's/       /;/g' | sed -e 's/      /;/g' | sed -e 's/     /;/g' | sed -e 's/    /;/g' | sed -e 's/   /;/g' | sed -e 's/  /;/g' | sed -e 's/ /;/g' > ${FILENAME2}.csv

#cat ${FILENAME3}.dat | sed -e 's/0-1/0 -1/g' | sed -e 's/^  //g' | sed -e 's/^ //g' | sed -e 's/        /;/g' | sed -e 's/       /;/g' | sed -e 's/      /;/g' | sed -e 's/     /;/g' | sed -e 's/    /;/g' | sed -e 's/   /;/g' | sed -e 's/  /;/g' | sed -e 's/ /;/g' > ${FILENAME3}.csv

cat ${FILENAME4}.log | sed -e 's/^  //g' | sed -e 's/^ //g' | sed -e 's/        /;/g' | sed -e 's/       /;/g' | sed -e 's/      /;/g' | sed -e 's/     /;/g' | sed -e 's/    /;/g' | sed -e 's/   /;/g' | sed -e 's/  /;/g' | sed -e 's/ /;/g' > ${FILENAME4}.csv

#cat ${FILENAME5}.csv | sed '1d' | sed -e 's/|/;/g' > ${FILENAME5}_.csv

#cat ${HEADER}.csv ${FILENAME1}.csv ${FILENAME2}.csv ${FILENAME3}.csv > ${FILENAME_OUT}.csv
cat ${HEADER}.csv ${FILENAME1}.csv ${FILENAME4}.csv > ${FILENAME_OUT}.csv
