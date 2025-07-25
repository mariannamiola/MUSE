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
	export DATA_SOURCE=${MUSE}/examples/07_sec_anti_2D_4000_md_tri/data
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
	export PROJ=07_sec_anti_2D_4000_md_tri
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


export SURF=D_0

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

muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GEOM1} --rotaxis X --rotangle 270 --opt ${OPT} --setz -1.0 --obj
muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GEOM2} --rotaxis X --rotangle 270 --opt ${OPT} --setz -1.0 --obj
muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GEOM3} --rotaxis X --rotangle 270 --opt ${OPT} --setz -1.0 --obj
muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GEOM4} --rotaxis X --rotangle 270 --opt ${OPT} --setz -1.0 --obj

#merge:
muse_geometry -U -p ${WP} -m ${OUTSURF}/${SURF}1.obj -m ${OUTSURF}/${SURF}2.obj --obj
muse_geometry -U -p ${WP} -m ${OUTSURF}/${SURF}1_${SURF}2.obj -m ${OUTSURF}/${SURF}3.obj --obj
muse_geometry -U -p ${WP} -m ${OUTSURF}/${SURF}1_${SURF}2_${SURF}3.obj -m ${OUTSURF}/${SURF}4.obj --obj

mv -v ${OUTSURF}/${SURF}1_${SURF}2_${SURF}3_${SURF}4.obj ${OUTSURF}/D_merge.obj

muse_geometry -L -p ${WP} -m ${OUTSURF}/D_merge.obj --rotaxis X --rotangle -270 --obj


#manip:
##########  MANIPULATE  ###########
declare -a arr=("ONLAP" "PROPORTIONAL" "PROPORTIONAL" "TRUNCATION")

for ((n=1; n<=4; n++))
do
  muse_manipulate -E -p ${WP} --geom ${OUTSURF}/${SURF}${n}.obj --rotaxis X --rotangle 270
  
  muse_manipulate -S -p ${WP} --mgeom ${OUTSURF}/${SURF}${n}.obj --type SAMPLES --sub ${SURF}${n} 
  muse_manipulate -S -p ${WP} --mgeom ${OUTSURF}/${SURF}${n}.obj --type GEOMETRY --geom ${OUTSURF}/${SURF}${n}.obj
  
  #echo "STAMPAAAAAAA"
  #echo ${arr[${n}-1]}
  muse_manipulate -T -p ${WP} --sttype ${arr[${n}-1]} --type SAMPLES --sub ${SURF}${n} --top ${SURF}${n}_top --bot ${SURF}${n}_bot --name ${SURF}${n} 
  muse_manipulate -T -p ${WP} --sttype ${arr[${n}-1]} --type GEOMETRY --top ${SURF}${n}_top --bot ${SURF}${n}_bot --geom ${OUTSURF}/${SURF}${n}.obj --obj --name ${SURF}${n}
done


#vario:
#########  VARIO  ###########
for ((n=1; n<=4; n++))
do
  muse_vario -V -p ${WP} -v ${VAR} --sttype ${arr[${n}-1]} -f samples_${SURF}${n} --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --dirs 0,70,80,90,110 --degtol 15 --vclean 10 --eps 5.0 --nugget 0.18 --sub ${SURF}${n}
done



#compute:
##########  COMPUTE  ###########
for ((n=1; n<=4; n++))
do
  export GMOD=geom_${SURF}${n}
  export OUTCOM=${OUTWP}/compute/${VAR}_${SURF}${n}_${DIR}${DIM}_${GMOD}
  
  
  export OUTNORMS=${OUTCOM}/_normspace
  export OUTVARS=${OUTCOM}/_varspace
  
  if [[ $OUTSGS == 'MEAN'* ]]; then
    muse_compute -C -p ${WP} -v ${VAR} --sub ${SURF}${n} -m ${OUTMAN}/${GMOD}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 0 --maxextr 1 
  else
    muse_compute -C -p ${WP} -v ${VAR} --sub ${SURF}${n} -m ${OUTMAN}/${GMOD}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
    muse_compute -S -p ${WP} -v ${VAR} --sub ${SURF}${n} -m ${OUTMAN}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --csv
    
    muse_compute -B -p ${WP} -v ${VAR} --sub ${SURF}${n} -m ${OUTMAN}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 1 --csv
    
    muse_compute -S -p ${WP} -v ${VAR} --sub ${SURF}${n} -m ${OUTMAN}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --space VAR --csv
  fi
done



#mergesim:
export GMOD=geom_${SURF}

export OUTCOM1=${OUTWP}/compute/${VAR}_${SURF}1_${DIR}${DIM}_${GMOD}1
export OUTCOM2=${OUTWP}/compute/${VAR}_${SURF}2_${DIR}${DIM}_${GMOD}2
export OUTCOM3=${OUTWP}/compute/${VAR}_${SURF}3_${DIR}${DIM}_${GMOD}3
export OUTCOM4=${OUTWP}/compute/${VAR}_${SURF}4_${DIR}${DIM}_${GMOD}4
  
if [[ $OUTSGS == 'MEAN'* ]]; then
  cat ${OUTCOM1}/${VAR}_${SURF}1.csv ${OUTCOM2}/${VAR}_${SURF}2.csv ${OUTCOM3}/${VAR}_${SURF}3.csv ${OUTCOM4}/${VAR}_${SURF}4.csv > ${OUTWP}/compute/${VAR}_D_merge.csv
else
  export OUTSTAT1=${OUTCOM1}/_varspace/_stats
  export OUTSTAT2=${OUTCOM2}/_varspace/_stats
  export OUTSTAT3=${OUTCOM3}/_varspace/_stats
  export OUTSTAT4=${OUTCOM4}/_varspace/_stats

  cat ${OUTSTAT1}/${VAR}_mean.csv ${OUTSTAT2}/${VAR}_mean.csv ${OUTSTAT3}/${VAR}_mean.csv ${OUTSTAT4}/${VAR}_mean.csv > ${OUTWP}/compute/${VAR}_D_merge.csv
fi


#db:
if [[ $OUTSGS == 'MEAN'* ]]; then
  echo "Database creation for MEAN SGS method ... NOT IMPLEMENTED!"
else
  for ((n=1; n<=4; n++))
  do
    export GMOD=geom_${SURF}${n}
    muse_compute -D -p ${WP} -v ${VAR} --sub ${SURF}${n} -m ${OUTMAN}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --space VAR
  done
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

#plot:
##########  PLOT  ###########
#-H -p ${WP} -v <FILE>

####################################################################### MUSE END
)
