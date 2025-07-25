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
	export DATA_SOURCE=${MUSE}/examples/07_sec_anti_2D_4000_tri/data
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
	export PROJ=07_sec_anti_2D_4000_tri
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=D_data.csv
fi


# 1. Export variables
#######################################################################
export GEOM1=D_01.txt
export GEOM2=D_02.txt
export GEOM3=D_03.txt
export GEOM4=D_04.txt
export GEOM0=D

export VAR=phi
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=a0.5

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

export OUTGEOM=${OUTWP}/geometry

export OUTSURF=${OUTWP}/geometry/surf
export OUTVOL=${OUTWP}/geometry/volume

export OUTMAN=${OUTWP}/manipulate

#export SURF1=D_01
#export SURF2=D_02
#export SURF3=D_03
#export SURF4=D_04

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
muse_data -S -p ${WP}  --setX 1 --setY 2 --setZ 3
muse_data -C -p ${WP}
#set -e	#exit if an error occours


#geometry:
##########  GEOMETRY  ###########
#set +e	#do not exit if an error occours
muse_geometry -N -p ${WP}
cp -R ${DATA_SOURCE}/${GEOM1} ${INGEOM}
cp -R ${DATA_SOURCE}/${GEOM2} ${INGEOM}
cp -R ${DATA_SOURCE}/${GEOM3} ${INGEOM}
cp -R ${DATA_SOURCE}/${GEOM4} ${INGEOM}

cp -R ${DATA_SOURCE}/${GEOM0}.xyz ${INGEOM}

#muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GEOM1} --rotaxis X --rotangle 270 --opt ${OPT} --setz -1.0 --obj
#muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GEOM2} --rotaxis X --rotangle 270 --opt ${OPT} --setz -1.0 --obj
#muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GEOM3} --rotaxis X --rotangle 270 --opt ${OPT} --setz -1.0 --obj
#muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GEOM4} --rotaxis X --rotangle 270 --opt ${OPT} --setz -1.0 --obj

muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GEOM0}.xyz --opt ${OPT} --setz -1.0 --obj

#merge:
#muse_geometry -U -p ${WP} -m ${OUTSURF}/${SURF1}.obj -m ${OUTSURF}/${SURF2}.obj --obj
#muse_geometry -U -p ${WP} -m ${OUTSURF}/${SURF1}_${SURF2}.obj -m ${OUTSURF}/${SURF3}.obj --obj
#muse_geometry -U -p ${WP} -m ${OUTSURF}/${SURF1}_${SURF2}_${SURF3}.obj -m ${OUTSURF}/${SURF4}.obj --obj

#mv -v ${OUTSURF}/${SURF1}_${SURF2}_${SURF3}_${SURF4}.obj ${OUTSURF}/D_merge.obj

muse_geometry -L -p ${WP} -m ${OUTSURF}/${GEOM0}.obj --rotaxis X --rotangle -270 --obj


#manip:
##########  MANIPULATE  ###########
muse_manipulate -E -p ${WP} --geom ${OUTSURF}/${GEOM0}.obj --rotaxis X --rotangle 270

#vario:
#########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR} --sub ${GEOM0} --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --dirs 0,70,80,90,110 --degtol 15 --vclean 10 --eps 5.0 --nugget 0.18



#compute:
##########  COMPUTE  ###########
export OUTCOM1=${OUTWP}/compute/${VAR}_${GEOM0}_${DIR}${DIM}_${GEOM0}

export OUTNORMS=${OUTCOM}/_normspace
export OUTVARS=${OUTCOM}/_varspace

if [[ $OUTSGS == 'MEAN'* ]]; then
  muse_compute -C -p ${WP} -v ${VAR} --sub D -m ${OUTSURF}/${GEOM0}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 0 --maxextr 1
else
  muse_compute -C -p ${WP} -v ${VAR} --sub D -m ${OUTSURF}/${GEOM0}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
    
  muse_compute -S -p ${WP} -v ${VAR} --sub D -m ${OUTSURF}/${GEOM0}.obj --dir ${DIR} --dim ${DIM} --csv
    
  muse_compute -B -p ${WP} -v ${VAR} --sub D -m ${OUTSURF}/${GEOM0}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 1 --csv
  #for (( id=0; id<=$NSIM-1; id++ ))
  #do
  #  muse_compute -B -p ${WP} -v ${VAR} -m ${OUTSURF}/${GEOM0}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 1 -f ${OUTNORMS}/${VAR}_sgs_$id.csv
  #done
    
  muse_compute -S -p ${WP} -v ${VAR} --sub D -m ${OUTSURF}/${GEOM0}.obj --dir ${DIR} --dim ${DIM} --space VAR --csv
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


#cat ${OUTCOM1}/${VAR}_${SURF1}.csv ${OUTCOM2}/${VAR}_${SURF2}.csv ${OUTCOM3}/${VAR}_${SURF3}.csv ${OUTCOM4}/${VAR}_${SURF4}.csv > ${OUTWP}/compute/${VAR}_D_merge.csv

)
