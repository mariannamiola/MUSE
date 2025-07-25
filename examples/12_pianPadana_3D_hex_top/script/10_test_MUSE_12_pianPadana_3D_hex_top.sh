#!/bin/bash

help()
{
    echo "
    	Usage: 10_test_MUSE 
    			[ -d | --data 	    ] data filename
    			[ -D | --datasource ] data folder
    			[ -p | --proj       ] project name
        	        [ -s | --sim        ] number of simulations
               		[ -w | --work       ] working directory
               		[ -h | --help       ] 
               		
               		"
    exit 2
}

(
set -e	#exit if an error occours

SHORT=d:,D:,p:,s:,w:,h
LONG=data:,data_source:,proj:,sim:,work:,help
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
	export DATA_SOURCE=${MUSE}/examples/12_pianPadana_3D_hex_top/data
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
	export PROJ=12_pianPadana_3D_hex_top
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=samples_extended.csv
fi

### INSERIRE:
#export DATA=samples_originale.csv ###no dati nuovi di temperatura superficiale
#export DATA=samples_extended.csv ###CON dati nuovi di temperatura superficiale
#export DATA=samples_extended_120325.csv ###AGGIORNAMENTO DATI: 12/03/2025: Rimozione outliers temperature


# 1. Export variables
#######################################################################
export GEOM=top
export RESX=1000
export RESY=1000
export RESZ=500

export VAR=T
#######################################################################


# 2. Export flags
#######################################################################
#For geometry

#For vario
export DIR=DIR
export DIM=3Dxy

#For compute
if [ "$OPTSIM" ]	#if a number of simulations is provided
then
	export NSIM=$OPTSIM
else
	export NSIM=20
fi
#######################################################################

########## REPORT  ###########

echo "
    	Running: 10_test_MUSE 
    			[ -d | --data 	    ] $DATA
    			[ -D | --datasource ] ${DATA_SOURCE}
    			[ -p | --proj       ] $PROJ
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
    cmd=$($SED	 -n "/$label:/{:a;n;p;ba};" $0 | grep -v ':$')
    eval "$cmd"
    exit
}

start=${1:-"start"}

jumpto $start

#project:
##########  PROJECT  ###########
muse_project -N -p ${WORK} --name ${PROJ}

##########  DATA  ###########

#set +e	#do not exit if an error occours
muse_data -N -p ${WP}
cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
muse_data -S -p ${WP} --setX 1 --setY 2 --setZ 3
muse_data -C -p ${WP}
#set -e	#exit if an error occours

#geometry:
##########  GEOMETRY  ###########

muse_geometry -N -p ${WP}
cp -R ${DATA_SOURCE}/${GEOM}.xyz ${INGEOM}

muse_geometry -P -p ${WP} --tri --points ${INGEOM}/${GEOM}.xyz --convex --obj

muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM}.obj --abs -z -7164 --obj
mv ${OUTSURF}/${GEOM}_absz.obj ${OUTSURF}/bottom.obj

muse_geometry -T -p ${WP} -m ${OUTSURF}/top.obj -m ${OUTSURF}/bottom.obj --obj

muse_geometry -M -p ${WP} -m ${OUTSURF}/top-bottom.obj --hex --resx ${RESX} --resy ${RESY} --resz ${RESZ} --vtk


#manip:
##########  MANIPULATE  ###########
muse_manipulate -E -p ${WP} --geom ${OUTSURF}/top-bottom.obj


#vario:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR} --nscore YES --dir ${DIR} --dim ${DIM} --dirs 0,22.5,67.5,90,112.5,135,157.5 --degtol 22.5 --zdegtol 5 --vario MODEL --lagspac CONSTANT --eps 5.0
muse_vario -V -p ${WP} -v ${VAR} --nscore YES --dir ${DIR} --dim 3Dz --lagspac CONSTANT --zdegtol 5 --vario MODEL --eps 5.0


#compute:
##########  COMPUTE  ###########
export GMOD=top-bottom

export OUTCOM=${OUTWP}/compute/${VAR}_${DIR}${DIM}_${GMOD}
export OUTNORMS=${OUTCOM}/_normspace
export OUTVARS=${OUTCOM}/_varspace

if [[ $OUTSGS == 'MEAN'* ]]; then
  muse_compute -C -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 5 --maxextr 200 --dir ${DIR} --dim ${DIM} --zrange 3600 --octant --scaleradius 2.5 --input 6 --simulated 4
else
  muse_compute -C -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --nsim ${NSIM} --out ${OUTSGS} --dir ${DIR} --dim ${DIM} --zrange 3600 --octant --scaleradius 2.5 --input 6 --simulated 4
  muse_compute -S -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --csv --dir ${DIR} --dim ${DIM}

  #back:
  muse_compute -B -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --extr Extr --minextr 5 --maxextr 200 --dir ${DIR} --dim ${DIM} --csv

  #statsback:
  muse_compute -S -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --space VAR --csv --dir ${DIR} --dim ${DIM}
fi


#clean:
if [[ $OUTSGS == 'MEAN'* ]]; then
  for (( id=0; id<$NSIM; id++ ))
  do
    rm ${SCRIPT_DIR}/output${id}.dat
  done
  rm ${SCRIPT_DIR}/sgs_output_data
  rm ${SCRIPT_DIR}/back_transf_data
else
    for (( id=0; id<=$NSIM-1; id++ ))
  do
    rm ${SCRIPT_DIR}/sgs_output_${id}.dat
    rm ${SCRIPT_DIR}/output${id}.dat
  done
  rm ${SCRIPT_DIR}/sgs_output_data
fi
####################################################################### MUSE END

)
