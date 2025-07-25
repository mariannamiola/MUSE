#!/bin/bash

export NAME_FOLDER=Liguria_2018
mkdir ${NAME_FOLDER}_format

cd ${NAME_FOLDER}

for oldname in * 
do 
  newname=`echo $oldname | sed -e 's/ //g'` 
  mv "$oldname" "$newname" 
done


# USAGE: source convert_RISL_data.sh 190520202200 csv

# nome del file: mandatory
#file_rain=$1

# estensione csv, può essere anche eliminato come secondo argomento se sempre dello stesso tipo
#ext=$2

# NOTA il file header.csv richiamato nell'ultimo comando, deve essere presente nella cartella e formattato con le nostre solite 6 righe

# NOTA il file dei dati deve essere scritto in formato fisso e senza spazi es. 190520202200.csv ** 12 CIFRE** + estensione

# NOTA un loop per più files in console

for j in `ls | grep "[0-9]\{12\}".csv`
do
  source ../00_convert_data.sh ${j%%.*} csv ../${NAME_FOLDER}_format
done

rm data.csv
#rm data_UTM.csv
rm labels.csv
rm temp.csv
rm values.csv
rm values2.csv
