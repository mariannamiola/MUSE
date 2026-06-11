#!/bin/bash

help()
{
    echo "
    	Usage: 10_test_MUSE 
    			[ -d | --data 	    ] data filename
    			[ -D | --datasource ] data folder
    			[ -p | --proj       ] project name
    			[ -r | --repl       ] replace project
        	        [ -s | --sim        ] number of simulations
               		[ -w | --work       ] working directory
               		[ -h | --help       ] 
               		
               		"
    exit 2
}

(
set -e	#exit if an error occours

RUN_DIR=$(pwd)

######################################################
################ PROJECT NAME ########################

FILENAME=$(basename "$0")          # prende il nome dello script in esecuzione
NAME="${FILENAME%.*}"           # rimuove l'estensione .sh
export setPROJECT_NAME="${NAME#10_test_MUSE_}"
echo "Project name (automatically) set to: $setPROJECT_NAME"

######################################################
######################################################

SHORT=d:,D:,p:,r:,s:,w:,h
LONG=data:,data_source:,proj:,repl:,sim:,work:,help
OPTS=$(getopt -a --options $SHORT --longoptions $LONG -- "$@")
#echo $OPTS

eval set -- "$OPTS"

while :
do
  case "$1" in
    -d | --data )
      OPTDATA="$2"
      shift 2
      ;;
    -D | --datasource )
      OPTDATASOURCE="$2"
      shift 2
      ;;
    -p | --proj )
      OPTPROJ="$2"
      shift 2
      ;;
    -r | --repl )
      OPTREPL="$2"
      shift 2
      ;;
    -s | --sim )
      OPTSIM="$2"
      shift 2
      ;;
    -w | --work )
      OPTWORK="$2"
      shift 2
      ;;
    -h | --help)
      help
      exit 2
      ;;
    --)
      shift;
      break
      ;;
    *)
      echo "Unexpected option: $1"
      ;;
  esac
done


# 0. Export paths
#######################################################################

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BIN_DIR=${SCRIPT_DIR}/../../../bin
export PATH=${BIN_DIR}:$PATH
###echo $PATH

#######################################################################

export MUSE=${SCRIPT_DIR}/../../..

if [ "$OPTDATASOURCE" ]	#if a input data folder is provided
then
	export DATA_SOURCE=${OPTDATASOURCE}
else
	export DATA_SOURCE=${MUSE}/examples/${setPROJECT_NAME}/data
fi

if [ "$OPTWORK" ]	#if a working directory is provided
then
	export WORK=${OPTWORK}
else
	export WORK=${MUSE}/examples/MUSE_test
fi


# 1. Export variables
#######################################################################

if [ "$OPTPROJ" ]	#if a project name is provided
then
	export PROJ=${OPTPROJ}
else
	export PROJ=${setPROJECT_NAME}
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=data.csv
fi


# 1. Export variables
#######################################################################
export GEOM=prism
export FORMAT=xyz

export VAR=phi
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=a0.5
export TETOPT=Ya0.5

#For vario
export DIR=DIR
export DIM=3Dxy

#For compute
if [ "$OPTSIM" ]	#if a number of simulations is provided
then
	export NSIM=$OPTSIM
else
	export NSIM=100
fi
#######################################################################

# Project settings
#######################################################################
if [ "$OPTREPL" ]	#if a boolean for replace data is provided
then
	export RMPROJ=${OPTREPL}
else
	export RMPROJ=true
fi

########## REPORT  ###########

echo "
    	Running: 10_test_MUSE 
    			[ -d | --data 	    ] $DATA
    			[ -D | --datasource ] ${DATA_SOURCE}
    			[ -p | --proj       ] $PROJ
    			[ -r | --repl       ] $RMPROJ
        	        [ -s | --sim        ] $NSIM
               		[ -w | --work       ] $WORK
               		
               		"

####################################################################### MUSE START ...

# Export folders
export WP=${WORK}/${PROJ}

export INWP=${WP}/in

export INDATA=${INWP}/data
export INGEOM=${INWP}/geometry

export OUTWP=${WP}/out

export OUTSURF=${OUTWP}/geometry/surf
export OUTVOL=${OUTWP}/geometry/volume

export OUTMAN=${OUTWP}/manipulate
export OUTVARIO=${OUTWP}/vario
export OUTCOMP=${OUTWP}/compute

export OUTSGS=VECSIM

mkdir -p ${WORK}

if [[ $OSTYPE == 'darwin'* ]]; then
  SED=gsed
else
  SED=sed
fi


# https://bobcopeland.com/blog/2012/10/goto-in-bash/
function jumpto
{
    label=$1
  cmd=$($SED -n "/$label:/{:a;n;p;ba};" "$0" | grep -v ':$' | grep -v '^)$')
    eval "$cmd"
    exit
}

start=${1:-"start"}

jumpto $start

#project:
##########  PROJECT  ###########
muse_project -N -p ${WORK} --name ${PROJ} --overwrite


##########  DATA  ###########
muse_data -N -p ${WP}
cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
muse_data -S -p ${WP} --setX 1 --setY 2 --setZ 3
muse_data -C -p ${WP}


#geometry:
##########  GEOMETRY  ###########
muse_geometry -N -p ${WP}
cp -R ${DATA_SOURCE}/${GEOM}.${FORMAT} ${INGEOM}
muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GEOM}.${FORMAT} --opt ${OPT} --obj
muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM}.obj --abs -z -32 --obj
muse_geometry -T -p ${WP} -m ${OUTSURF}/${GEOM}.obj -m ${OUTSURF}/${GEOM}_absz.obj --obj

muse_geometry -M -p ${WP} -m ${OUTSURF}/${GEOM}-${GEOM}_absz.obj --tet --opt ${TETOPT} --vtk
mv ${OUTVOL}/${GEOM}-${GEOM}_absz.vtk ${OUTVOL}/${GEOM}.vtk
mv ${OUTVOL}/${GEOM}-${GEOM}_absz.json ${OUTVOL}/${GEOM}.json


#vario:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR} --nscore YES --vario MODEL --eps 5.0 --dim ${DIM} --dir ${DIR} --maxdist 70 --dirs 0,45,67.5,90 --degtol 45 --zdegtol 5
muse_vario -V -p ${WP} -v ${VAR} --nscore YES --dir ${DIR} --dim 3Dz --lagspac CONSTANT --zdegtol 5 --vario MODEL --eps 5.0


#compute:
##########  COMPUTE  ###########
export ZRANGE=7.9

if [[ $OUTSGS == 'MEAN'* ]]; then
  muse_compute -C -p ${WP} -v ${VAR} -m ${OUTVOL}/${GEOM}.vtk --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 0 --maxextr 1 --dir ${DIR} --dim ${DIM} --zrange ${ZRANGE} --octant
else
  muse_compute -C -p ${WP} -v ${VAR} -m ${OUTVOL}/${GEOM}.vtk --nsim ${NSIM} --out ${OUTSGS} --dir ${DIR} --dim ${DIM} --zrange ${ZRANGE} --octant

  muse_compute -S -p ${WP} -v ${VAR} -m ${OUTVOL}/${GEOM}.vtk --csv --dir ${DIR} --dim ${DIM}

  #back:
  muse_compute -B -p ${WP} -v ${VAR} -m ${OUTVOL}/${GEOM}.vtk --extr Extr --minextr 0 --maxextr 1 --csv --dir ${DIR} --dim ${DIM}

  #statsback:
  muse_compute -S -p ${WP} -v ${VAR} -m ${OUTVOL}/${GEOM}.vtk --space VAR --csv --dir ${DIR} --dim ${DIM}
fi


#clean:
if [[ $OUTSGS == 'MEAN'* ]]; then
  for (( id=0; id<$NSIM; id++ ))
  do
    rm -f "${SCRIPT_DIR}/output${id}.dat" "${RUN_DIR}/output${id}.dat"
  done
  rm -f "${SCRIPT_DIR}/sgs_output_data" "${RUN_DIR}/sgs_output_data"
  rm -f "${SCRIPT_DIR}/back_transf_data" "${RUN_DIR}/back_transf_data"
else
    for (( id=0; id<=$NSIM-1; id++ ))
  do
    rm -f "${SCRIPT_DIR}/sgs_output_${id}.dat" "${RUN_DIR}/sgs_output_${id}.dat"
    rm -f "${SCRIPT_DIR}/output${id}.dat" "${RUN_DIR}/output${id}.dat"
  done
  rm -f "${SCRIPT_DIR}/sgs_output_data" "${RUN_DIR}/sgs_output_data"
fi
####################################################################### MUSE END

)
