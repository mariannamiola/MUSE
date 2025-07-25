#!/bin/bash

#export BASENAME='MOR_BATprofile_T'
export VAR=T

#export MORTARA_X=477550.0
#export MORTARA_Y=5014200.0

export INFOLDER='_1Dprofile'
export OUTFOLDER='_1Dprofile_filtered'
mkdir -p ${OUTFOLDER}

WELLS=("BATTUDA1" "BRIGNANO1" "MALOSSA3" "MORTARA1" "SALI_VERCELLESE1" "VALLI_SALIMBENE1")  # Modify with actual filenames
FILES=("mean" "mean_m_stdev" "mean_p_stdev")  # Modify with actual filenames

# Loop through each file
for WELL in "${WELLS[@]}"; do
	for FILE in "${FILES[@]}"; do
	   FILENAME=${WELL}profile_${VAR}_${FILE}.csv
	   ##FILENAME=${BASENAME}${FILE}.csv
	   echo "Processing file: ${FILENAME}"
	   
	   #cat ${FILENAME} | sed -e 's/,/;/g' > ${OUTFOLDER}/__${FILENAME}
	   
	   ## Filter points with a tolerance distance (< 20)
	   #python3 filter_profile.py ${OUTFOLDER}/__${FILENAME} ${OUTFOLDER}/${FILENAME} ${MORTARA_X} ${MORTARA_Y}
	   python3 filter_nan.py ${INFOLDER}/${FILENAME} ${OUTFOLDER}/${FILENAME}
	done
done


### Plotting vertical profile: Petromod vs muse model
for WELL in "${WELLS[@]}"; do
	python3 plot_profile.py ${WELL}model.csv ${OUTFOLDER}/${WELL}profile_${VAR}_${FILES[0]}.csv ${OUTFOLDER}/${WELL}profile_${VAR}_${FILES[1]}.csv ${OUTFOLDER}/${WELL}profile_${VAR}_${FILES[2]}.csv ${WELL}
	echo "Plotting profile for well: ${WELL}"
done
