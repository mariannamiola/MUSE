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
	export DATA_SOURCE=${MUSE}/examples/00_Rtest/data
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
	export PROJ=00_Rtest
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=dataset.csv
fi


# 1. Export variables
#######################################################################
#export GEOM=domain

export VAR=Porosity
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
#export OPT=a10000000

#For vario
export DIR=OMNI

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
muse_data -S -p ${WP}  --setX 3 --setY 4 --setZ 5
muse_data -C -p ${WP}
#set -e	#exit if an error occours

#geometry:
##########  GEOMETRY  ###########
###routine for gridding and voxel or hexmesh

#muse_geometry -N -p ${WP}
#cp -R ${DATA_SOURCE}/${GEOM}.xyz ${INGEOM}

#muse_geometry -P -p ${WP} --tri --points ${INGEOM}/${GEOM}.xyz --concave --opt ${OPT} --obj
#--points /Users/mariannamiola/muse/examples/MUSE_test/12_pianPadana_3D_tet/in/geometry/samples_bound.xyz --obj --concave

#muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM}.obj --abs -z -165 --obj
#mv ${OUTSURF}/${GEOM}_absz.obj ${OUTSURF}/top.obj 

#muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM}.obj --abs -z -7200 --obj
#mv ${OUTSURF}/${GEOM}_absz.obj ${OUTSURF}/bottom.obj 

#muse_geometry -T -p ${WP} -m ${OUTSURF}/top.obj -m ${OUTSURF}/bottom.obj --obj

#muse_geometry -M -p ${WP} -m ${OUTSURF}/top-bottom.obj --tet --opt Yq --vtk

#mv ${OUTVOL}/cube-cube_absz.vtk ${OUTVOL}/cube.vtk
#mv ${OUTVOL}/cube-cube_absz.json ${OUTVOL}/cube.json


#manip:
##########  MANIPULATE  ###########

#strat:

#vario:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR} --nscore YES --vario MODEL --eps 5.0
#muse_vario -V -p ${WP} -v ${VAR} --nscore YES --dir DIR --dim 2D --vario MODEL --deg 22.5 --degtol 22.5 --eps 5.0


#compute:

####################################################################### MUSE END

)
