#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export WORK=MUSE_test
export PROJECT=13_RISL_tri_mf

export PROJ_FOLDER=${SCRIPT_DIR}/../../${WORK}/${PROJECT}
echo ${PROJ_FOLDER}

export COMPUTE_FOLDER=${PROJ_FOLDER}/out/compute
echo ${COMPUTE_FOLDER}
cd ${COMPUTE_FOLDER}
#ls | wc -l

#export frame='190520200000_UTM'
#cd ${COMPUTE_FOLDER}/${frame}

if [[ $OSTYPE == 'darwin'* ]]; then
  PVPATH=/Applications/ParaView-5.10.1.app/Contents/bin
else
  PVPATH=${HOME}/ParaView-5.12.0-MPI-Linux-Python3.10-x86_64/bin
fi

for frame in *
do
	echo 'FRAME: ' ${frame}
   	${PVPATH}/pvpython ${SCRIPT_DIR}/20_visual_13_RISL_tri_mf.py ${frame}
done

##/Applications/ParaView-5.10.1.app/Contents/MacOS/paraview ; exit; --script=20_visual_13_RISL_tri_mf.py
##paraview --script=20_visual_13_RISL_tri_mf.py

######## Command for merging figures into a GIF
### brew install imagemagick
### cd ${PROJ_FOLDER}/_fig
### convert -delay 20 -loop 0 *.png EH_mean.gif
