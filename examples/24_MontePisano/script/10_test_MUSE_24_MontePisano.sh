#!/bin/bash

help()
{
    echo "
    	Usage: 10_test_MUSE 
    			[ -d | --data 	    ] data filename
    			[ -D | --datasource ] data folder
    			[ -p | --proj       ] project name
    			[ -r | --repl       ] replace project
        	        [ -s | --sim        ] number of simulations
               		[ -w | --work       ] working directory
               		[ -h | --help       ] 
               		
               		"
    exit 2
}

(
set -e	#exit if an error occours

######################################################
################ PROJECT NAME ########################

FILENAME=$(basename "$0")          # prende il nome dello script in esecuzione
NAME="${FILENAME%.*}"           # rimuove l'estensione .sh
export setPROJECT_NAME="${NAME#10_test_MUSE_}"
echo "Project name (automatically) set to: $setPROJECT_NAME"

######################################################
######################################################

SHORT=d:,D:,p:,r:,s:,w:,h
LONG=data:,data_source:,proj:,repl:,sim:,work:,help
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
    -r | --repl )
      OPTREPL="$2"
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
	export DATA=samples100x100.csv
fi


# 1. Export variables
#######################################################################
export VAR=cat_vege
export GEOM=dtm #dtmid_clip
export FORMAT=asc

#######################################################################


# 2. Export flags
#######################################################################
#For geometry
#export OPT=a0.1

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

#For export
export NCAT=9
#######################################################################

# Project settings
#######################################################################
if [ "$OPTREPL" ]	#if a boolean for replace data is provided
then
	export RMPROJ=${OPTREPL}
else
	export RMPROJ=true
fi

########## REPORT  ###########

echo "
    	Running: 10_test_MUSE 
    			[ -d | --data 	    ] $DATA
    			[ -D | --datasource ] ${DATA_SOURCE}
    			[ -p | --proj       ] $PROJ
    			[ -r | --repl       ] $RMPROJ
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
export OUTVARIO=${OUTWP}/vario
export OUTCOMP=${OUTWP}/compute


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
muse_data -S -p ${WP} --setX 2 --setY 3 --setZ 8 #--setDel COMMA ##--setZ 5
muse_data -C -p ${WP} #--setDel COMMA

#geometry:
##########  GEOMETRY  ###########
muse_geometry -N -p ${WP}
cp -R ${DATA_SOURCE}/${GEOM}.${FORMAT} ${INGEOM}
#mkdir -p ${OUTSURF}
#cp -R ${DATA_SOURCE}/${GEOM}* ${OUTSURF}

read -p "READING RASTER ... Press any key to continue ... " -n1 -s
muse_geometry -R -p ${WP} --grid --obj

#read -p "READING OBJ ... Press any key to continue ... " -n1 -s
#mkdir -p ${OUTSURF}
#cp -R ${DATA_SOURCE}/${GEOM}.obj ${OUTSURF}
#cp -R ${DATA_SOURCE}/${GEOM}.json ${OUTSURF}

muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM}.obj --abs -z 0.0 --obj
muse_geometry -L -p ${WP} -m ${OUTSURF}/${GEOM}_absz.obj --translate -5,5,0.0 --obj
export GEOMNAME=${GEOM}_absz_tr


#read -p "POINT IN POLYGON ... Press any key to continue ... " -n1 -s

#manip:
##########  MANIPULATE  ###########
#muse_manipulate -E -p ${WP} --geom ${OUTSURF}/${GEOM}_absz.obj

read -p "VARIOGRAM ... Press any key to continue ... " -n1 -s

#vario:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR} --vario MODEL --dir ${DIR} --dim ${DIM} #--sub ${GEOM}_absz #--dir ${DIR} --dim ${DIM} --dirs 0,70,80,90,110 --degtol 15
#muse_vario -V -p ${WP} -v ${VAR0} --sub ${GEOM} --vario MODEL --dir ${DIR} --dim ${DIM} --dirs 0,70,80,90,110 --degtol 15 --vclean 10 --itype SPHERICAL!6 --itype SPHERICAL!8 --inugget 0!1 --inugget 0!2 --inugget 0!3 --inugget 0!4 --inugget 0!5 --inugget 0!6 --inugget 0!7 --inugget 0!8

read -p "SISIM ... Press any key to continue ... " -n1 -s

#compute:
##########  COMPUTE  ###########
muse_compute -C -p ${WP} -v ${VAR} -m ${OUTSURF}/${GEOMNAME}.obj --crit SISIM --dir ${DIR} --dim ${DIM} --nsim ${NSIM} #--sub ${GEOM}_absz


#clean:
for (( id=1; id<=$NCAT; id++ ))
do
   cp ${SCRIPT_DIR}/pdf_cat_${id}.txt ${OUTCOMP}/${VAR}_${DIR}${DIM}_${GEOMNAME}/pdf_${VAR}_${id}.csv
   rm ${SCRIPT_DIR}/pdf_cat_${id}.txt
done

#export:
muse_export -p ${WP} -m ${GEOMNAME}.obj -v ${VAR} -N ${NCAT} --geopkg -o ${OUTCOMP}/${VAR}_${DIR}${DIM}_${GEOMNAME}/${GEOMNAME}_out.gpkg --type INDICATOR --dir ${DIR} --dim ${DIM}



####################################################################### MUSE END

)
