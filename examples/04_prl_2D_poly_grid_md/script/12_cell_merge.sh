#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BIN_DIR=${SCRIPT_DIR}/../../../bin
export PATH=${BIN_DIR}:$PATH
echo $PATH
export MUSE=${SCRIPT_DIR}/../../..
export WORK=${MUSE}/examples/MUSE_test

export PROJECT=04_prl_2D_poly_grid_md
export WP=${WORK}/${PROJECT}

export OUTSURF=${WP}/out/geometry/surf

export NAME=MNG_CELLE_


muse_geometry -U -p ${WP} -m ${OUTSURF}/${NAME}1.obj -m ${OUTSURF}/${NAME}2.obj --obj
mv ${OUTSURF}/${NAME}1_${NAME}2.obj ${OUTSURF}/Celle.obj

for (( id=3; id<=135305; id++ ))
do
   muse_geometry -U -p ${WP} -m ${OUTSURF}/Celle.obj -m ${OUTSURF}/${NAME}${id}.obj --obj
   mv ${OUTSURF}/Celle_${NAME}${id}.obj ${OUTSURF}/Celle.obj
done

