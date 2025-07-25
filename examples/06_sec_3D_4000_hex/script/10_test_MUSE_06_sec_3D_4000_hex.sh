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
	export DATA_SOURCE=${MUSE}/examples/06_sec_3D_4000_hex/data
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
	export PROJ=06_sec_3D_4000_hex
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=amga.csv
fi


# 1. Export variables
#######################################################################
export GEOM1=SEC.gpkg

export VAR=phi
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=a0.5

export RESX=0.5
export RESY=0.1
export RESZ=1.0

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

##########  DATA  ###########

#set +e	#do not exit if an error occours
muse_data -N -p ${WP}
cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
muse_data -S -p ${WP} --setX 1 --setY 2 --setZ 3
muse_data -C -p ${WP}
#set -e	#exit if an error occours

#geometry:
##########  GEOMETRY  ###########
#set +e	#do not exit if an error occours
muse_geometry -N -p ${WP}
cp -R ${DATA_SOURCE}/${GEOM1} ${INGEOM}

muse_geometry -G -p ${WP} --hex --bbp 0,-15,-0.5 --bbp 100,-15,-0.5 --bbp 100,0,-0.5 --bbp 0,0,-0.5 --bbp 0,-15,-1.5 --bbp 100,-15,-1.5 --bbp 100,0,-1.5 --bbp 0,0,-1.5 --resx ${RESX} --resy ${RESY} --resz ${RESZ} --vtk
muse_geometry -Z -p ${WP} -m ${OUTVOL}/grid.vtk --rotaxis X --rotangle -270 --vtk
#set -e	#exit if an error occours


#manip:
##########  MANIPULATE  ###########


#vario:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR} --rotaxis X --rotangle 270 --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --dirs 0,70,80,90,110 --degtol 15 --vclean 5 --weight --eps 5.0 --nugget 0.18

#compute:
##########  COMPUTE  ###########
export GMOD=grid
export OUTCOM=${OUTWP}/compute/${VAR}_${DIR}${DIM}_${GMOD}


if [[ $OUTSGS == 'MEAN'* ]]; then
  muse_compute -C -p ${WP} -v ${VAR} --rotaxis X --rotangle 270 -m ${OUTVOL}/${GMOD}.vtk --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 0 --maxextr 1 --octant --scaleradius 1.5 ##--input 10 --simulated 6
else
  muse_compute -C -p ${WP} -v ${VAR} --rotaxis X --rotangle 270 -m ${OUTVOL}/${GMOD}.vtk --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS} --octant --scaleradius 1.5 ##--input 10 --simulated 6
  
  #stats:
  muse_compute -S -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --dir ${DIR} --dim ${DIM} --csv

  #back:
  muse_compute -B -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 1 --csv

  #statsback:
  muse_compute -S -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --dir ${DIR} --dim ${DIM} --space VAR --csv
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
