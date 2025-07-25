#!/bin/bash

export HEADER=header
export FILENAME1=log_polcevera
export FILENAME2=log_rock
export FILENAME3=log_substratum
export FILENAME_OUT=log_samples

cat ${FILENAME1}.dat | sed -e 's/^  //g' | sed -e 's/^ //g' | sed -e 's/        /;/g' | sed -e 's/       /;/g' | sed -e 's/      /;/g' | sed -e 's/     /;/g' | sed -e 's/    /;/g' | sed -e 's/   /;/g' | sed -e 's/  /;/g' | sed -e 's/ /;/g' > ${FILENAME1}.csv

cat ${FILENAME2}.dat | sed -e 's/^  //g' | sed -e 's/^ //g' | sed -e 's/        /;/g' | sed -e 's/       /;/g' | sed -e 's/      /;/g' | sed -e 's/     /;/g' | sed -e 's/    /;/g' | sed -e 's/   /;/g' | sed -e 's/  /;/g' | sed -e 's/ /;/g' > ${FILENAME2}.csv

cat ${FILENAME3}.dat | sed -e 's/0-1/0 -1/g' | sed -e 's/^  //g' | sed -e 's/^ //g' | sed -e 's/        /;/g' | sed -e 's/       /;/g' | sed -e 's/      /;/g' | sed -e 's/     /;/g' | sed -e 's/    /;/g' | sed -e 's/   /;/g' | sed -e 's/  /;/g' | sed -e 's/ /;/g' > ${FILENAME3}.csv

cat ${HEADER}.csv ${FILENAME1}.csv ${FILENAME2}.csv ${FILENAME3}.csv > ${FILENAME_OUT}.csv
