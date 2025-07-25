#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BIN_DIR=${SCRIPT_DIR}/../../../bin
export PATH=${BIN_DIR}:$PATH
echo $PATH
export MUSE=${SCRIPT_DIR}/../../..
export WORK=${MUSE}/examples/MUSE_test

export PROJECT=04_prl_2D_poly_tri_md
export WP=${WORK}/${PROJECT}

export OUTSURF=${WP}/out/geometry/surf




declare -a name_geom=("F5TERRE_tri" "FMAGRA_tri" "FPETRONIO_tri" "FPORTOFINO_tri" "FENTELLA_tri" "FAVETO_tri" "FPADANO_tri" "FBISAGNO_tri" "FPOLCEVERA_tri" "FARENZANO_tri" "FSASSELLO_tri" "FBORMIDE_tri" "FSAVONESE_tri" "FIMPERIESE_tri")

echo

muse_geometry -U -p ${WP} -m ${OUTSURF}/${name_geom[0]}.obj -m ${OUTSURF}/${name_geom[1]}.obj --obj
mv ${OUTSURF}/${name_geom[0]}_${name_geom[1]}.obj ${OUTSURF}/Liguria.obj

for (( id=2; id<=13; id++ ))
do
   muse_geometry -U -p ${WP} -m ${OUTSURF}/Liguria.obj -m ${OUTSURF}/${name_geom[${id}]}.obj --obj
   mv ${OUTSURF}/Liguria_${name_geom[${id}]}.obj ${OUTSURF}/Liguria.obj
done


