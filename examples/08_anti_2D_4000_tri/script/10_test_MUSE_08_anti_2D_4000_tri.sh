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

RUN_DIR=$(pwd)

######################################################
################ PROJECT NAME ########################

FILENAME=$(basename "$0")          # prende il nome dello script in esecuzione
NAME="${FILENAME%.*}"           # rimuove l'estensione .sh
export setPROJECT_NAME="${NAME#10_test_MUSE_}"
echo "Project name (automatically) set to: $setPROJECT_NAME"

######################################################
######################################################

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
	export DATA_SOURCE=${MUSE}/examples/${setPROJECT_NAME}/data
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
	export PROJ=${setPROJECT_NAME}
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=anti_4000_phi_class.csv
fi


# 1. Export variables
#######################################################################
export GEOM=anti ##geometry name (without extension) - boundary points describing the anticline geometry parallel to the vertical plane x-z

export VAR1=phi ##continuous variable to be simulated
export VAR2=class ##categorical variable to be simulated
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=a0.01

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
muse_project -N -p ${WORK} --name ${PROJ} --overwrite

##########  DATA  ###########
muse_data -N -p ${WP}
cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
muse_data -S -p ${WP} --setX 1 --setY 2 --setZ 3
muse_data -C -p ${WP}

#geometry:
##########  GEOMETRY  ###########
muse_geometry -N -p ${WP}
cp -R ${DATA_SOURCE}/${GEOM}.txt ${INGEOM}

##The triangulation of the geometry is performed in 2D (x-z plane). Points are in fact rotated by 270 degrees around the x-axis and then triangulated with the obj format.
muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GEOM}.txt --opt ${OPT} --setz 17.7119 --obj


#manip:
##########  MANIPULATE  ###########
##Data are rotated to operate on horizontal geometry (x-y plane) for manipulation
muse_manipulate -E -p ${WP} --geom ${OUTSURF}/${GEOM}.obj
exit

##Points (samples and vertex of the geometry) are projected on top and bottom of the geometry to define the stratigraphic coordinates
muse_manipulate -S -p ${WP} --mgeom ${OUTSURF}/${GEOM}.obj --type SAMPLES --sub ${GEOM}
muse_manipulate -S -p ${WP} --mgeom ${OUTSURF}/${GEOM}.obj --type GEOMETRY --geom ${OUTSURF}/${GEOM}.obj

#strat:
##Stratigraphic coordinate transformation is performed in the horizontal reference system (x-y plane) for both samples and geometry
muse_manipulate -T -p ${WP} --sttype PROPORTIONAL --type SAMPLES --sub ${GEOM} --top ${GEOM}_top --bot ${GEOM}_bot --name ${GEOM}
muse_manipulate -T -p ${WP} --sttype PROPORTIONAL --type GEOMETRY --top ${GEOM}_top --bot ${GEOM}_bot --geom ${OUTSURF}/${GEOM}.obj --obj --name ${GEOM}

##Straigraphic geometry is rotated back to the original reference system (x-z plane) using muse-geometry
muse_geometry -L -p ${WP} -m ${OUTMAN}/geom_${GEOM}.obj --rotaxis X --rotangle -270 --obj






read -p "VARIOGRAM - CONTINUOUS ... Press any key to continue ... " -n1 -s

############################## (CONTINUOUS) VARIOGRAM AND SGS COMPUTATION ##############################

##########  VARIO  ###########
##Variogram is computed on horizontal plane (x-y) and in stratigraphic coordinates, then the variogram model (anisotropy) is fitted and used for simulation
muse_vario -V -p ${WP} -v ${VAR1} --sttype PROPORTIONAL -f samples_${GEOM} --sub ${GEOM} --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --dirs 0,70,80,90,110 --degtol 15 --vclean 10 --eps 5.0

#computesgs:
##########  COMPUTE  ###########
muse_compute -C -p ${WP} -v ${VAR1} --sub ${GEOM} -m ${OUTMAN}/geom_${GEOM}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS} --octant --scaleradius 1.5 --input 10 --simulated 6
muse_compute -S -p ${WP} -v ${VAR1} --sub ${GEOM} -m ${OUTMAN}/geom_${GEOM}.obj --dir ${DIR} --dim ${DIM} --csv
muse_compute -B -p ${WP} -v ${VAR1} --sub ${GEOM} -m ${OUTMAN}/geom_${GEOM}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 1 --csv
muse_compute -S -p ${WP} -v ${VAR1} --sub ${GEOM} -m ${OUTMAN}/geom_${GEOM}.obj --dir ${DIR} --dim ${DIM} --space VAR --csv



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





############################## INDICATOR VARIOGRAM AND SIS COMPUTATION ##############################

read -p "VARIOGRAM - INDICATOR ... Press any key to continue ... " -n1 -s

#variosis:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR2} --sttype PROPORTIONAL -f samples_${GEOM} --sub ${GEOM} --vario MODEL --dir ${DIR} --dim ${DIM} --dirs 0,70,80,90,110 --degtol 15 --vclean 10 --itype SPHERICAL!6 --itype SPHERICAL!8 --inugget 0!1 --inugget 0!2 --inugget 0!3 --inugget 0!4 --inugget 0!5 --inugget 0!6 --inugget 0!7 --inugget 0!8

#computesis:
##########  COMPUTE  ###########
read -p "SIS ... Press any key to continue ... " -n1 -s

muse_compute -C -p ${WP} -v ${VAR2} --sub ${GEOM} --dir ${DIR} --dim ${DIM} -m ${OUTMAN}/geom_${GEOM}.obj --crit SISIM --nsim ${NSIM} --scaleradius 1.5 --octant --simulated 6 --input 10

################################
export OUTCOMP=${OUTWP}/compute/${VAR2}_${GEOM}_${DIR}${DIM}_geom_${GEOM}
export PDF_NAME=pdf_cat_
export NCAT=8
for ((i=1; i<=${NCAT}; i++))
do
    cp ${SCRIPT_DIR}/${PDF_NAME}${i}.txt ${OUTCOMP}/${VAR2}_${i}_pdf.csv
    rm ${SCRIPT_DIR}/${PDF_NAME}${i}.txt
done 






# #db:
# ##########  DATABASE  ###########
# if [[ $OUTSGS == 'MEAN'* ]]; then
#   echo "Database creation for MEAN SGS method ... NOT IMPLEMENTED!"
# else
#   muse_compute -D -p ${WP} -v ${VAR} --sub ${GEOM} -m ${OUTMAN}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --space VAR
# fi


#plot:
##########  PLOT  ###########
#-H -p ${WP} -v <FILE>

####################################################################### MUSE END

)
