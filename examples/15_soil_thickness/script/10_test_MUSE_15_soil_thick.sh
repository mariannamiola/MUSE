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
set -x   #verbose

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
	export DATA_SOURCE=${MUSE}/examples/15_soil_thickness/data
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
	export PROJ=15_soil_thickness
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=data.csv
fi


# 1. Export variables
#######################################################################
export GEOM1=boundary.gpkg

export VAR0=D
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=a10000

#For vario
export DIR=OMNI

#For compute
if [ "$OPTSIM" ]	#if a number of simulations is provided
then
	export NSIM=$OPTSIM
else
	export NSIM=50
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
muse_data -N -p ${WP}
cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
muse_data -S -p ${WP}  --setX 1 --setY 2 --setZ 3
muse_data -C -p ${WP}

#geometry:
##########  GEOMETRY  ###########
#set +e	#do not exit if an error occours
muse_geometry -N -p ${WP}
cp -R ${DATA_SOURCE}/${GEOM1} ${INGEOM}
muse_geometry -V -p ${WP} --grid --resx 100 --resy 100 --obj
mv ${OUTSURF}/boundary.obj ${OUTSURF}/boundary_grid.obj
mv ${OUTSURF}/boundary.json ${OUTSURF}/boundary_grid.json

muse_geometry -V -p ${WP} --tri --opt ${OPT} --obj
mv ${OUTSURF}/boundary.obj ${OUTSURF}/boundary_tri.obj
mv ${OUTSURF}/boundary.json ${OUTSURF}/boundary_tri.json
#muse_geometry -O -p ${WP} -m ${OUTSURF}/FSASSELLO.obj --abs -z 100 --obj
#muse_geometry -T -p ${WP} -m ${OUTSURF}/FSASSELLO.obj -m ${OUTSURF}/FSASSELLO_absz.obj
#set -e	#exit if an error occours

#manip:
##########  MANIPULATE  ###########
#muse_manipulate -E -p ${WP} --geom ${OUTSURF}/FSASSELLO_tri.obj


#vario:
##########  VARIO  ###########
#muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --dir DIR --dim 2D --vario MODEL --dirs 0,45,80,110,135 --degtol 45 --vclean 10 --eps 5.0 --weight --lagspac CONSTANT --type SPHERICAL
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL

#compute:
##########  COMPUTE  ###########
declare -a arr=("boundary_tri" "boundary_grid")
for i in "${arr[@]}"
do

    export OUTCOM=${OUTWP}/compute/${VAR0}_${DIR}${DIM}_${GMOD}
    
#  if [[ $DIR == 'DIR'* ]]; then
#    export OUTCOM=${OUTWP}/compute/${VAR0}_dir_$i
#  else
#    export OUTCOM=${OUTWP}/compute/${VAR0}_omni_$i
#  fi

  export OUTNORMS=${OUTCOM}/_normspace
  export OUTVARS=${OUTCOM}/_varspace


  if [[ $OUTSGS == 'MEAN'* ]]; then
     muse_compute -C -p ${WP} -v ${VAR0} -m ${OUTSURF}/${i}.obj --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 100 --maxextr 4000
  else
     muse_compute -C -p ${WP} -v ${VAR0} -m ${OUTSURF}/${i}.obj --nsim ${NSIM} --out ${OUTSGS} --scaleradius 10 --input 8
     muse_compute -S -p ${WP} -v ${VAR0} -m ${OUTSURF}/${i}.obj --csv
     
     muse_compute -B -p ${WP} -v ${VAR0} -m ${OUTSURF}/${i}.obj --extr Extr --minextr 0 --maxextr 15 --csv
#     for (( id=0; id<$NSIM; id++ ))
#     do
#       muse_compute -B -p ${WP} -v ${VAR0} -m ${OUTSURF}/${i}.obj --extr Extr --minextr 0 --maxextr 15 -f ${OUTNORMS}/sgs_$id.csv --csv
#     done
     
     muse_compute -S -p ${WP} -v ${VAR0} -m ${OUTSURF}/${i}.obj --space VAR --csv
  fi
  
  #db:
  if [[ $OUTSGS == 'MEAN'* ]]; then
    echo "Dataset creation for MEAN SGS method ... NOT IMPLEMENTED!"
  else
     muse_compute -D -p ${WP} -v ${VAR0} -m ${OUTSURF}/$i.obj --space VAR
  fi
  #mv ${OUTCOM} ${OUTCOM}_$i
done


#clean:
#echo ${SCRIPT_DIR}
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





#plot:
##########  PLOT  ###########
#-H -p ${WP} -v <FILE>

####################################################################### MUSE END

)
