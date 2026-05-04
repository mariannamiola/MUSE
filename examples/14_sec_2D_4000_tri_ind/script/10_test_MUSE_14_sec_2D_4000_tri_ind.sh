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
echo $PATH

#######################################################################

export MUSE=${SCRIPT_DIR}/../../..

if [ "$OPTDATASOURCE" ]	#if a input data folder is provided
then
	export DATA_SOURCE=${OPTDATASOURCE}
else
	export DATA_SOURCE=${MUSE}/examples/14_sec_2D_4000_tri_ind/data
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
	export PROJ=14_sec_2D_4000_tri_ind
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=amga_class_v_4000.csv
fi


# 1. Export variables
#######################################################################
export GEOM=sec

export VAR0=phi
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=a0.1

#For vario
export DIR=DIR
export DIM=2D

#For compute
if [ "$OPTSIM" ]	#if a number of simulations is provided
then
	export NSIM=$OPTSIM
else
	export NSIM=10
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
muse_data -N -p ${WP}
cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
muse_data -S -p ${WP}  --setX 1 --setY 2 --setZ 3
muse_data -C -p ${WP}

#geometry:
##########  GEOMETRY  ###########
muse_geometry -N -p ${WP}
cp -R ${DATA_SOURCE}/${GEOM}.xyz ${INGEOM}
muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GEOM}.xyz --opt ${OPT} --setz -1.0 --obj
muse_geometry -L -p ${WP} -m ${OUTSURF}/${GEOM}.obj --rotaxis X --rotangle -270 --obj

#manip:
##########  MANIPULATE  ###########
muse_manipulate -E -p ${WP} --geom ${OUTSURF}/${GEOM}.obj --rotaxis X --rotangle 270

#vario:
##########  VARIO  ###########
#muse_vario -V -p ${WP} -v ${VAR0} --sub ${GEOM} --vario MODEL --vclean 10
muse_vario -V -p ${WP} -v ${VAR0} --sub ${GEOM} --vario MODEL --dir ${DIR} --dim ${DIM} --dirs 0,70,80,90,110 --degtol 15 --vclean 10 --itype SPHERICAL!6 --itype SPHERICAL!8 --inugget 0!1 --inugget 0!2 --inugget 0!3 --inugget 0!4 --inugget 0!5 --inugget 0!6 --inugget 0!7 --inugget 0!8

#compute:
##########  COMPUTE  ###########
muse_compute -C -p ${WP} -v ${VAR0} --sub ${GEOM} --dir ${DIR} --dim ${DIM} -m ${OUTSURF}/${GEOM}.obj --crit SISIM --nsim ${NSIM} --simulated 4 --input 2 --scaleradius 1.2 --octant


################################
export OUTCOMP=${OUTWP}/compute/${VAR0}_${GEOM}_${DIR}${DIM}_${GEOM}
export PDF_NAME=pdf_cat_
export NCAT=8
for ((i=1; i<=${NCAT}; i++))
do
    cp ${SCRIPT_DIR}/${PDF_NAME}${i}.txt ${OUTCOMP}/${PDF_NAME}${i}.csv
    rm ${SCRIPT_DIR}/${PDF_NAME}${i}.txt
done 


#stats:

#back:

#statsback:


####################################################################### MUSE END

)
