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
	export DATA_SOURCE=${MUSE}/examples/00_poster_STAG24/data
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
	export PROJ=00_poster_STAG24
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=amga.csv
fi


# 1. Export variables
#######################################################################
export GEOM1=unito1_2_Portovecchio

export VAR=phi
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=a0.5
#export OPT=a0.1

#For vario
export DIR=DIR
export DIM=2D

#For compute
if [ "$OPTSIM" ]	#if a number of simulations is provided
then
	export NSIM=$OPTSIM
else
	export NSIM=100
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

export RESX=50
export RESY=50
export RESZ=50


export OUTSGS=VECSIM


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

#geometry:
##########  GEOMETRY  ###########
#set +e	#do not exit if an error occours
muse_geometry -N -p ${WP}
cp -R ${DATA_SOURCE}/${GEOM1} ${INGEOM}
muse_geometry -V -p ${WP} --tri --opt ${OPT} --obj
muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM1}.obj --abs -z -50 --obj
muse_geometry -T -p ${WP} -m ${OUTSURF}/${GEOM1}.obj -m ${OUTSURF}/${GEOM1}_absz.obj --obj

mv -v ${OUTSURF}/${GEOM1}-${GEOM1}_absz.obj ${OUTSURF}/porto.obj
mv -v ${OUTSURF}/${GEOM1}-${GEOM1}_absz.json ${OUTSURF}/porto.json

muse_geometry -M -p ${WP} -m ${OUTSURF}/porto.obj --tet --opt Ya500 --vtk

export XMIN=1492841.66
export XMAX=1496176.866
export YMIN=4914737.228
export YMAX=4917919
export ZMIN=-50
export ZMAX=0

muse_geometry -G -p ${WP} --hex --bbp ${XMIN},${YMIN},${ZMIN} --bbp ${XMAX},${YMIN},${ZMIN} --bbp ${XMAX},${YMAX},${ZMIN} --bbp ${XMIN},${YMAX},${ZMIN} --bbp ${XMIN},${YMIN},${ZMAX} --bbp ${XMAX},${YMIN},${ZMAX} --bbp ${XMAX},${YMAX},${ZMAX} --bbp ${XMIN},${YMAX},${ZMAX} --resx ${RESX} --resy ${RESY} --resz ${RESZ} --vtk

muse_geometry -S -p ${WP} --hex -m ${OUTVOL}/grid.vtk --boundary ${OUTSURF}/porto.obj --vtk



####################################################################### MUSE END

)
