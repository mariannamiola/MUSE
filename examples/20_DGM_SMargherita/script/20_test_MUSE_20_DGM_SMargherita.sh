#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export WORK=MUSE_test
export PROJECT=20_DGM_SMargherita

export PROJ_FOLDER=${SCRIPT_DIR}/../../${WORK}/${PROJECT}
echo ${PROJ_FOLDER}

export COMPUTE_FOLDER=${PROJ_FOLDER}/out/compute
echo ${COMPUTE_FOLDER}
cd ${COMPUTE_FOLDER}
#ls | wc -l

#export frame='20230501000000'
#cd ${COMPUTE_FOLDER}/${frame}

if [[ $OSTYPE == 'darwin'* ]]; then
  PVPATH=/Applications/ParaView-5.10.1.app/Contents/bin
else
  PVPATH=${HOME}/ParaView-5.12.0-MPI-Linux-Python3.10-x86_64/bin
fi

for frame in *
do
	echo 'FRAME: ' ${frame}
	${PVPATH}/pvpython ${SCRIPT_DIR}/20_visual_${PROJECT}.py ${frame}
done

######## Command for merging figures into a GIF
### brew install imagemagick
### convert -delay 20 -loop 0 *.png GIF.gif


#/Applications/ParaView-5.10.1.app/Contents/bin/pvpython ${SCRIPT_DIR}/20_visual_20_DGM_SMargherita.py ${frame}
##/Applications/ParaView-5.10.1.app/Contents/MacOS/paraview ; exit; --script=20_visual_13_RISL_tri_mf.py
##paraview --script=20_visual_13_RISL_tri_mf.py
