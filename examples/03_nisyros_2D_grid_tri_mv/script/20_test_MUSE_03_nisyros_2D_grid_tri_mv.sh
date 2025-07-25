#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export WORK=MUSE_test
export PROJECT=03_nisyros_2D_grid_tri_mv

export PROJ_FOLDER=${SCRIPT_DIR}/../../${WORK}/${PROJECT}
echo ${PROJ_FOLDER}

export LINUXPV=ParaView-5.13.2-MPI-Linux-Python3.10-x86_64

if [[ $OSTYPE == 'darwin'* ]]; then
  PVPATH=/Applications/ParaView-5.10.1.app/Contents/bin
else
  PVPATH=${HOME}/${LINUXPV}/bin
fi

${PVPATH}/pvpython ${SCRIPT_DIR}/20_visual_03_nisyros_2D_grid_CO2.py
${PVPATH}/pvpython ${SCRIPT_DIR}/20_visual_03_nisyros_2D_grid_lCO2.py
${PVPATH}/pvpython ${SCRIPT_DIR}/20_visual_03_nisyros_2D_grid_T.py

${PVPATH}/pvpython ${SCRIPT_DIR}/20_visual_03_nisyros_2D_tri_CO2.py
${PVPATH}/pvpython ${SCRIPT_DIR}/20_visual_03_nisyros_2D_tri_lCO2.py
${PVPATH}/pvpython ${SCRIPT_DIR}/20_visual_03_nisyros_2D_tri_T.py
