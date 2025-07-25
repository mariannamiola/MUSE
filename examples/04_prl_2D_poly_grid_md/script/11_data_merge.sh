#!/bin/bash

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export WORK=MUSE_test
export PROJECT=04_prl_2D_poly_grid_md

export PROJ_FOLDER=${SCRIPT_DIR}/../../${WORK}/${PROJECT}
echo ${PROJ_FOLDER}
export OUTCOMP=${PROJ_FOLDER}/out/compute

export VAR=cr
export VARIO=OMNI3D
export STAT=mean ###mean


declare -a name_sub=("F5TERRE_tri" "FMAGRA_tri" "FPETRONIO_tri" "FPORTOFINO_tri" "FENTELLA_tri" "FAVETO_tri" "FPADANO_tri" "FBISAGNO_tri" "FPOLCEVERA_tri" "FARENZANO_tri" "FSASSELLO_tri" "FBORMIDE_tri" "FSAVONESE_tri" "FIMPERIESE_tri")

declare -a name_geom=("F5TERRE_grid" "FMAGRA_grid" "FPETRONIO_grid" "FPORTOFINO_grid" "FENTELLA_grid" "FAVETO_grid" "FPADANO_grid" "FBISAGNO_grid" "FPOLCEVERA_grid" "FARENZANO_grid" "FSASSELLO_grid" "FBORMIDE_grid" "FSAVONESE_grid" "FIMPERIESE_grid")


cat ${OUTCOMP}/${VAR}_${name_sub[0]}_${VARIO}_${name_geom[0]}/_varspace/_stats/${VAR}_${STAT}.csv ${OUTCOMP}/${VAR}_${name_sub[1]}_${VARIO}_${name_geom[1]}/_varspace/_stats/${VAR}_${STAT}.csv ${OUTCOMP}/${VAR}_${name_sub[2]}_${VARIO}_${name_geom[2]}/_varspace/_stats/${VAR}_${STAT}.csv ${OUTCOMP}/${VAR}_${name_sub[3]}_${VARIO}_${name_geom[3]}/_varspace/_stats/${VAR}_${STAT}.csv ${OUTCOMP}/${VAR}_${name_sub[4]}_${VARIO}_${name_geom[4]}/_varspace/_stats/${VAR}_${STAT}.csv ${OUTCOMP}/${VAR}_${name_sub[5]}_${VARIO}_${name_geom[5]}/_varspace/_stats/${VAR}_${STAT}.csv ${OUTCOMP}/${VAR}_${name_sub[6]}_${VARIO}_${name_geom[6]}/_varspace/_stats/${VAR}_${STAT}.csv ${OUTCOMP}/${VAR}_${name_sub[7]}_${VARIO}_${name_geom[7]}/_varspace/_stats/${VAR}_${STAT}.csv ${OUTCOMP}/${VAR}_${name_sub[8]}_${VARIO}_${name_geom[8]}/_varspace/_stats/${VAR}_${STAT}.csv ${OUTCOMP}/${VAR}_${name_sub[9]}_${VARIO}_${name_geom[9]}/_varspace/_stats/${VAR}_${STAT}.csv ${OUTCOMP}/${VAR}_${name_sub[10]}_${VARIO}_${name_geom[10]}/_varspace/_stats/${VAR}_${STAT}.csv ${OUTCOMP}/${VAR}_${name_sub[11]}_${VARIO}_${name_geom[11]}/_varspace/_stats/${VAR}_${STAT}.csv ${OUTCOMP}/${VAR}_${name_sub[12]}_${VARIO}_${name_geom[12]}/_varspace/_stats/${VAR}_${STAT}.csv ${OUTCOMP}/${VAR}_${name_sub[13]}_${VARIO}_${name_geom[13]}/_varspace/_stats/${VAR}_${STAT}.csv > ${OUTCOMP}/${VAR}_${STAT}_merge.csv

#wc -l ${OUTCOMP}/${VAR}_${name_geom[0]}_${VARIO}_${name_geom[0]}/_varspace/_stats/${VAR}_${STAT}.csv
#wc -l ${OUTCOMP}/${VAR}_${name_geom[1]}_${VARIO}_${name_geom[1]}/_varspace/_stats/${VAR}_${STAT}.csv
wc -l ${OUTCOMP}/${VAR}_${STAT}_merge.csv
