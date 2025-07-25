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
	export DATA_SOURCE=${MUSE}/examples/08_anti_3D_4000_tet/data
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
	export PROJ=08_anti_3D_4000_tet
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=samples_anti_1.csv
fi


# 1. Export variables
#######################################################################
export GEOM1=anti.txt

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

export OUTSURF=${OUTWP}/geometry/surf
export OUTVOL=${OUTWP}/geometry/volume

export OUTMAN=${OUTWP}/manipulate

export OUTSGS=VECSIM

export SUB=anti_0.5-anti_absz

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

##########  GEOMETRY  ###########
set +e	#do not exit if an error occours
muse_geometry -N -p ${WP}
cp -R ${DATA_SOURCE}/${GEOM1} ${INGEOM}
muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/anti.txt --rotaxis X --rotangle 270 --opt ${OPT} --setz -1.0 --obj

muse_geometry -O -p ${WP} -m ${OUTSURF}/anti.obj --abs -z -0.5 --obj
mv -v ${OUTSURF}/anti_absz.obj ${OUTSURF}/anti_0.5.obj

muse_geometry -O -p ${WP} -m ${OUTSURF}/anti.obj --abs -z -1.5 --obj

muse_geometry -T -p ${WP} -m ${OUTSURF}/anti_0.5.obj -m ${OUTSURF}/anti_absz.obj --obj

muse_geometry -M -p ${WP} -m ${OUTSURF}/anti_0.5-anti_absz.obj --tet --vtk
mv -v ${OUTVOL}/anti_0.5-anti_absz.vtk ${OUTVOL}/anti.vtk

muse_geometry -Z -p ${WP} -m ${OUTVOL}/anti.vtk --rotaxis X --rotangle -270 --vtk
set -e	#exit if an error occours

#manip:
##########  MANIPULATE  ###########
muse_manipulate -E -p ${WP} --geom ${OUTSURF}/anti_0.5-anti_absz.obj --rotaxis X --rotangle 270

#muse_manipulate -S -p ${WP} --type SAMPLES --sub anti_0.5-anti_absz --mgeom ${OUTSURF}/anti.obj
muse_manipulate -V -p ${WP} --type SAMPLES --sub anti_0.5-anti_absz --mgeom ${OUTVOL}/anti.vtk

#muse_manipulate -S -p ${WP} --type GEOMETRY --geom ${OUTSURF}/anti_0.5.obj --mgeom ${OUTSURF}/anti_0.5.obj 
#muse_manipulate -S -p ${WP} --type GEOMETRY --geom ${OUTSURF}/anti_absz.obj --mgeom ${OUTSURF}/anti_absz.obj
muse_manipulate -V -p ${WP} --type GEOMETRY --geom ${OUTVOL}/anti.vtk --mgeom ${OUTVOL}/anti.vtk


#strat:
muse_manipulate -T -p ${WP} --sttype PROPORTIONAL --type SAMPLES --sub anti_0.5-anti_absz --top anti_top --bot anti_bot --name anti

#muse_manipulate -T -p ${WP} --sttype PROPORTIONAL --type GEOMETRY --top anti_0.5_top --bot anti_0.5_bot --geom ${OUTSURF}/anti_0.5.obj --obj --name anti_0.5
#muse_manipulate -T -p ${WP} --sttype PROPORTIONAL --type GEOMETRY --top anti_absz_top --bot anti_absz_bot --geom ${OUTSURF}/anti_absz.obj --obj --name anti_absz
muse_manipulate -T -p ${WP} --sttype PROPORTIONAL --type VOLUME --top anti_top --bot anti_bot --geom ${OUTVOL}/anti.vtk --vtk --name anti


#geomstrat:
##########  GEOMETRY  ###########
#muse_geometry -T -p ${WP} -m ${OUTMAN}/geom_anti_0.5.obj -m ${OUTMAN}/geom_anti_absz.obj --obj
#mv -v ${OUTMAN}/anti_0.5-anti_absz.vtk ${OUTVOL}/anti.vtk

#muse_geometry -M -p ${WP} -m ${OUTSURF}/geom_anti_0.5-geom_anti_absz.obj --tet --vtk
#mv -v ${OUTVOL}/geom_anti_0.5-geom_anti_absz.vtk ${OUTVOL}/anti_strat.vtk



#vario:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR} --sttype PROPORTIONAL -f samples_anti --sub ${SUB} --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --dirs 0,70,80,90,110 --degtol 15 --vclean 10 --eps 5.0 #--weight #--lagspac CONSTANT


#compute:
##########  COMPUTE  ###########
export GMOD=geom_anti
export OUTCOM=${OUTWP}/compute/${VAR}_${SUB}_${DIR}${DIM}_${GMOD}

export OUTNORMS=${OUTCOM}/_normspace
export OUTVARS=${OUTCOM}/_varspace


if [[ $OUTSGS == 'MEAN'* ]]; then
  muse_compute -C -p ${WP} -v ${VAR} --sub ${SUB} -m ${OUTMAN}/${GMOD}.vtk --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 0 --maxextr 1
else
  muse_compute -C -p ${WP} -v ${VAR} --sub ${SUB} -m ${OUTMAN}/${GMOD}.vtk --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
  muse_compute -S -p ${WP} -v ${VAR} -m ${OUTMAN}/${GMOD}.vtk --sub ${SUB} --dim ${DIM} --dir ${DIR} --csv

  #back:
  muse_compute -B -p ${WP} -v ${VAR} -m ${OUTMAN}/${GMOD}.vtk --sub ${SUB} --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 1 --csv
  #for (( id=0; id<=$NSIM-1; id++ ))
  #do
  #  muse_compute -B -p ${WP} -v ${VAR} -m ${OUTMAN}/${GMOD}.vtk --sub ${SUB} --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 1 -f ${OUTNORMS}/${VAR}_sgs_$id.csv
  #done

  #statsback:
  muse_compute -S -p ${WP} -v ${VAR} -m ${OUTMAN}/${GMOD}.vtk --sub ${SUB} --dir ${DIR} --dim ${DIM} --space VAR --csv
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
