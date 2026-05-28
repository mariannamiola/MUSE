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
	export DATA=AMGA_full_may_2026.csv
fi


# 1. Export variables
#######################################################################
#export GEOM1=sec
export GMOD=sec_xz

export VAR1=phi
export VAR2=class
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
# Check if the current user is root
if [[ -d ${WP} ]]; then
    echo "DIRECTORY: " ${WP} " EXISTS."
    if $RMPROJ; then
        echo "REMOVING EXISTING PROJECT " ${WP} 
        rm -r ${WP}
    
        ##########  PROJECT  ###########
        muse_project -N -p ${WORK} --name ${PROJ}
   
       ##########  DATA  ###########
        muse_data -N -p ${WP}
        cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
        muse_data -S -p ${WP} --setX 1 --setY 2 --setZ 3
        muse_data -C -p ${WP}
    
        ##########  GEOMETRY  ###########
        muse_geometry -N -p ${WP}
        cp -R ${DATA_SOURCE}/${GMOD}.xyz ${INGEOM}
        muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GMOD}.xyz --opt ${OPT} --setz -1.0 --obj --rotaxis X --rotangle 270
        muse_geometry -L -p ${WP} -m ${OUTSURF}/${GMOD}.obj --rotaxis X --rotangle -270 --obj
    else
        if [[ -d ${OUTMAN} ]]; then
           rm -r ${OUTMAN}
        fi
        if [[ -d ${OUTVARIO} ]]; then
           rm -r ${OUTVARIO}
        fi
        if [[ -d ${OUTCOMP} ]]; then
           rm -r ${OUTCOMP}
        fi
    fi
else
    echo "DIRECTORY: " ${WP} " NOT EXISTS."
    ##########  PROJECT  ###########
    muse_project -N -p ${WORK} --name ${PROJ}
    
    ##########  DATA  ###########
    muse_data -N -p ${WP}
    cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
    muse_data -S -p ${WP} --setX 1 --setY 2 --setZ 3
    muse_data -C -p ${WP}
    
    ##########  GEOMETRY  ###########
    muse_geometry -N -p ${WP}
    cp -R ${DATA_SOURCE}/${GMOD}.xyz ${INGEOM}
    muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GMOD}.xyz --opt ${OPT} --setz -1.0 --obj --rotaxis X --rotangle 270
    muse_geometry -L -p ${WP} -m ${OUTSURF}/${GMOD}.obj --rotaxis X --rotangle -270 --obj
fi



#manip:
##########  MANIPULATE  ###########
#muse_manipulate -E -p ${WP} --geom ${OUTSURF}/${GMOD}.obj --rotaxis X --rotangle 270


#vario:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR1} --rotaxis X --rotangle 270 --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --dirs 0,70,80,90,110 --degtol 15 --vclean 5 --weight --eps 5.0 --nugget 0.18


#compute:
##########  COMPUTE  ###########
if [[ $OUTSGS == 'MEAN'* ]]; then
  muse_compute -C -p ${WP} -v ${VAR1} --rotaxis X --rotangle 270 -m ${OUTSURF}/${GMOD}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 0 --maxextr 1 --octant --scaleradius 1.5 --input 10 --simulated 6
else
  muse_compute -C -p ${WP} -v ${VAR1} --rotaxis X --rotangle 270 -m ${OUTSURF}/${GMOD}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS} --octant --scaleradius 1.5 --input 10 --simulated 6

  #stats:
  muse_compute -S -p ${WP} -v ${VAR1} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --csv

  #back:
  muse_compute -B -p ${WP} -v ${VAR1} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 1 --csv
  
  #statsback:
  muse_compute -S -p ${WP} -v ${VAR1} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --space VAR --csv
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


#variosis:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR2} --vario MODEL --dir ${DIR} --dim ${DIM} --dirs 0,70,80,90,110 --degtol 15 --vclean 10 --itype SPHERICAL!6 --itype SPHERICAL!8 --inugget 0!1 --inugget 0!2 --inugget 0!3 --inugget 0!4 --inugget 0!5 --inugget 0!6 --inugget 0!7 --inugget 0!8

#computesis:
##########  COMPUTE  ###########
muse_compute -C -p ${WP} -v ${VAR2} --dir ${DIR} --dim ${DIM} -m ${OUTSURF}/${GMOD}.obj --crit SISIM --nsim ${NSIM}

################################
export OUTCOMP=${OUTWP}/compute/${VAR2}_${DIR}${DIM}_${GMOD}
export PDF_NAME=pdf_cat_
export NCAT=8
for ((i=1; i<=${NCAT}; i++))
do
    cp ${SCRIPT_DIR}/${PDF_NAME}${i}.txt ${OUTCOMP}/${VAR2}_${i}_pdf.csv
    rm ${SCRIPT_DIR}/${PDF_NAME}${i}.txt
done 



####################################################################### MUSE END

)
