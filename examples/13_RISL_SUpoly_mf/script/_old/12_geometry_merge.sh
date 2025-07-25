#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BIN_DIR=${SCRIPT_DIR}/../../../bin
export PATH=${BIN_DIR}:$PATH
echo $PATH
export MUSE=${SCRIPT_DIR}/../../..
export WORK=${MUSE}/examples/MUSE_test

export PROJECT=13_RISL_tri_mf_Liguria
export WP=${WORK}/${PROJECT}

export OUTSURF=${WP}/out/geometry/surf

export NFEATURES=7024
export NAME=su_liguria


echo

muse_geometry -U -p ${WP} -m ${OUTSURF}/${NAME}_1.obj -m ${OUTSURF}/${NAME}_2.obj --obj
mv ${OUTSURF}/${NAME}_1_${NAME}_2.obj ${OUTSURF}/${NAME}.obj

for (( id=3; id<=${NFEATURES}; id++ ))
do
   muse_geometry -U -p ${WP} -m ${OUTSURF}/${NAME}.obj -m ${OUTSURF}/${NAME}_${id}.obj --obj
   mv ${OUTSURF}/${NAME}_${NAME}_${id}.obj ${OUTSURF}/${NAME}.obj
done



#declare -a name_geom2=("F5TERRE_tri" "FMAGRA_tri" "FPETRONIO_tri" "FPORTOFINO_tri" "FENTELLA_tri" "FAVETO_tri" "FPADANO_tri" "FBISAGNO_tri" "FPOLCEVERA_tri" "FARENZANO_tri" "FSASSELLO_tri" "FBORMIDE_tri" "FSAVONESE_tri" "FIMPERIESE_tri")
#
#echo
#
#muse_geometry -U -p ${WP} -m ${OUTSURF}/${name_geom2[0]}.obj -m ${OUTSURF}/${name_geom2[1]}.obj --obj
#mv ${OUTSURF}/${name_geom2[0]}_${name_geom2[1]}.obj ${OUTSURF}/Liguria.obj
#
#for (( id=2; id<=13; id++ ))
#do
#   muse_geometry -U -p ${WP} -m ${OUTSURF}/Liguria.obj -m ${OUTSURF}/${name_geom2[${id}]}.obj --obj
#   mv ${OUTSURF}/Liguria_${name_geom2[${id}]}.obj ${OUTSURF}/Liguria.obj
#done
##wc -l ${OUTCOMP}/${VAR}_${STAT}_merge.csv
