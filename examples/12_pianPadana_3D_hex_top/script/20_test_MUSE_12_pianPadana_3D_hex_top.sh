#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export WORK=MUSE_test
export PROJECT=12_pianPadana_3D_hex_top

export PROJ_FOLDER=${SCRIPT_DIR}/../../${WORK}/${PROJECT}
echo ${PROJ_FOLDER}

export LINUXPV=ParaView-5.13.2-MPI-Linux-Python3.10-x86_64

if [[ $OSTYPE == 'darwin'* ]]; then
  PVPATH=/Applications/ParaView-5.10.1.app/Contents/bin
else
  PVPATH=${HOME}/${LINUXPV}/bin
fi


### Extract planar sections at different depths and profiles

## 1) Extract section
# Definizione del vettore di double
DEPTHS=(1000.0 2000.0 3000.0 4000.0 5000.0 6000.0 7000.0)

# Loop su ogni elemento
for val in "${DEPTHS[@]}"; do
    echo "Extract section for depth: $val"
    ${PVPATH}/pvpython ${SCRIPT_DIR}/20_visual_${PROJECT}_extract.py true false ${val}
done

### Extract vertical profiles
# Format vertical profiles to import in excel (for comparison)
${PVPATH}/pvpython ${SCRIPT_DIR}/20_visual_${PROJECT}_extract.py false true 0.0
./for_profile.sh







#########################################################################
### Visualization 
#${PVPATH}/pvpython ${SCRIPT_DIR}/20_visual_${PROJECT}.py

### Extract vertical profiles
#${PVPATH}/pvpython ${SCRIPT_DIR}/20_visual_${PROJECT}_ALLSEC.py
# Format vertical profiles to import in excel (for comparison)
#./for_profile.sh

