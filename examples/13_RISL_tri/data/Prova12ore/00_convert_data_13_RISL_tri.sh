	#!/bin/bash

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
  source 01_convert_data_13_RISL_tri.sh ${j%%.*} csv
done


#echo "SPLIT data" del file 
#sed '1 d ' ${file_rain}.${ext} > data.csv
#awk 'BEGIN {FS=";"} {print $1}' data.csv > labels.csv
#awk 'BEGIN {FS=";"} {print $4}' data.csv > values.csv
#
#echo "CONVERT coordinates"
#awk 'BEGIN {FS=";"} {print $3,$2}' data.csv | cs2cs EPSG:4326  EPSG:23032 | awk 'BEGIN{OFS=";"} {print $1,$2}' > data_UTM.csv
#
#
## la lettura 0 mm di pioggia è arbitraria, indica che la lettura reale è sotto 0.1 mm. Nel file i valori 0 vengono sostituiti con 0.1, mentre i valori NA rimangono tali, o sostituiti da -9999.0 per GRASS
## quello che segue introduce un white noie attorno alla lettura 
#
#echo "WHITE NOISE to data == 0.0"
#awk 'BEGIN {FS=";"} {if ($1 == 0) 
#{ 
#printf ("%7.4f\n", 0.096+rand()/100);
#}
#else
#{
#printf ("%s\n", $1);
#}
#}' values.csv > values2.csv
#
#echo "PATCH all column"
#paste -d ";" labels.csv data_UTM.csv values2.csv > temp_UTM.csv 
#
#echo "APPEND all files"
#cat header.csv temp_UTM.csv > ${file_rain}_UTM.${ext}