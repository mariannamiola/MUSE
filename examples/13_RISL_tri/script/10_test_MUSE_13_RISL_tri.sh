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
	export DATA_SOURCE=${MUSE}/examples/13_RISL_tri/data
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
	export PROJ=13_RISL_tri
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	#export DATA=080220141900_format_UTM.csv
	export DATA=190520202200_UTM.csv
fi


# 1. Export variables
#######################################################################
export GEOM0=italy_3
export GEOM1=_t2000
export GEOM=${GEOM0}${GEOM1}

export VAR0=EH
#export VAR1=CO2
#export VAR2=lCO2
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=qa10000000

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
muse_data -S -p ${WP} --setX 2 --setY 3
muse_data -C -p ${WP}
#muse_data -R -p ${WP}

##########  GEOMETRY  ###########
#geometry:
muse_geometry -N -p ${WP}

###routine for triangulation and tets
cp -R ${DATA_SOURCE}/../${GEOM}.gpkg ${INGEOM}
muse_geometry -V -p ${WP} --tri --opt ${OPT} --obj

#for ((n=1; n<=3; n++))
#do
#    muse_geometry -L -p ${WP} -m ${OUTSURF}/${GEOM}_${n}.obj --splmet EDGE --obj
#done

#for ((n=1; n<=3; n++))
#do
#    muse_geometry -L -p ${WP} -m ${OUTSURF}/${GEOM}_${n}.obj --remesh --mark --obj
#done


#manip:
#########  MANIPULATE  ###########
for ((n=1; n<=3; n++))
do
    muse_manipulate -E -p ${WP} --geom ${OUTSURF}/${GEOM}_${n}.obj
done


#vario:
#########  VARIO  ###########
for ((n=1; n<=2; n++))
do
    muse_vario -V -p ${WP} -v ${VAR0} --sub ${GEOM}_${n} --nscore YES --vario MODEL --eps 5.0 --dim ${DIM} --dir ${DIR} --maxdist 200000 --decl YES --csize 100000 --nstep 10
done
muse_vario -V -p ${WP} -v ${VAR0} --sub ${GEOM}_3 --nscore YES --vario MODEL --eps 5.0 --dim ${DIM} --dir ${DIR} --dirs 0,45,90 --maxdist 200000 --decl YES --csize 100000 --nstep 10


#compute:
#########  COMPUTE  ###########
export ped=
if [[ $OUTSGS == 'MEAN'* ]]; then
	muse_compute -C -p ${WP} -v ${VAR0} --sub ${GEOM}_1$ -m ${OUTSURF}/${GEOM}_1${ped}.obj --nsim ${NSIM} --bnscore --extr Extr --minextr 0 --maxextr 35 --dir ${DIR} --dim ${DIM}
else
	muse_compute -C -p ${WP} -v ${VAR0} --sub ${GEOM}_1 -m ${OUTSURF}/${GEOM}_1${ped}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
	muse_compute -S -p ${WP} -v ${VAR0} --sub ${GEOM}_1 -m ${OUTSURF}/${GEOM}_1${ped}.obj --dir ${DIR} --dim ${DIM} --csv

	muse_compute -B -p ${WP} -v ${VAR0} --sub ${GEOM}_1 -m ${OUTSURF}/${GEOM}_1${ped}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 35 --csv

	muse_compute -S -p ${WP} -v ${VAR0} --sub ${GEOM}_1 -m ${OUTSURF}/${GEOM}_1${ped}.obj --dir ${DIR} --dim ${DIM} --space VAR --csv
fi

if [[ $OUTSGS == 'MEAN'* ]]; then
	muse_compute -C -p ${WP} -v ${VAR0} --sub ${GEOM}_2 -m ${OUTSURF}/${GEOM}_2${ped}.obj --nsim ${NSIM} --bnscore --extr Extr --minextr 0 --maxextr 13 --dir ${DIR} --dim ${DIM}
else
	muse_compute -C -p ${WP} -v ${VAR0} --sub ${GEOM}_2 -m ${OUTSURF}/${GEOM}_2${ped}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
	muse_compute -S -p ${WP} -v ${VAR0} --sub ${GEOM}_2 -m ${OUTSURF}/${GEOM}_2${ped}.obj --dir ${DIR} --dim ${DIM} --csv

	muse_compute -B -p ${WP} -v ${VAR0} --sub ${GEOM}_2 -m ${OUTSURF}/${GEOM}_2${ped}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 13 --csv

	muse_compute -S -p ${WP} -v ${VAR0} --sub ${GEOM}_2 -m ${OUTSURF}/${GEOM}_2${ped}.obj --dir ${DIR} --dim ${DIM} --space VAR --csv
fi

if [[ $OUTSGS == 'MEAN'* ]]; then
	muse_compute -C -p ${WP} -v ${VAR0} --sub ${GEOM}_3 -m ${OUTSURF}/${GEOM}_3${ped}.obj --nsim ${NSIM} --bnscore --extr Extr --minextr 0 --maxextr 5 --dir ${DIR} --dim ${DIM}
else
	muse_compute -C -p ${WP} -v ${VAR0} --sub ${GEOM}_3 -m ${OUTSURF}/${GEOM}_3${ped}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
	muse_compute -S -p ${WP} -v ${VAR0} --sub ${GEOM}_3 -m ${OUTSURF}/${GEOM}_3${ped}.obj --dir ${DIR} --dim ${DIM} --csv

	muse_compute -B -p ${WP} -v ${VAR0} --sub ${GEOM}_3 -m ${OUTSURF}/${GEOM}_3${ped}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 5 --csv

	muse_compute -S -p ${WP} -v ${VAR0} --sub ${GEOM}_3 -m ${OUTSURF}/${GEOM}_3${ped}.obj --dir ${DIR} --dim ${DIM} --space VAR --csv
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
