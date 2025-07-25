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
	export DATA_SOURCE=${MUSE}/examples/11_matrac_3D_poly_mv_tet/data
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
	export PROJ=11_matrac_3D_poly_mv_tet
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=acquisition.csv
fi


# 1. Export variables
#######################################################################
export GEOM0=unito1_2
export GEOM1=unito1_2_Portovecchio
export GEOM2=unito1_2_Portovecchio_Senza_DucaDegliAbruzzi


export VAR=DO_mgl
export MINEX=0
export MAXEX=20
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=a500

#For vario
export DIR=OMNI
export DIM=3D

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
    
	#project:
	##########  PROJECT  ###########
	muse_project -N -p ${WORK} --name ${PROJ}
	
	#geometry:
	##########  GEOMETRY  ###########
	muse_geometry -N -p ${WP}
	cp -R ${DATA_SOURCE}/${GEOM1} ${INGEOM}
	muse_geometry -V -p ${WP} --tri --opt ${OPT} --obj
	muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM1}.obj --abs -z -5 --obj
	muse_geometry -T -p ${WP} -m ${OUTSURF}/${GEOM1}.obj -m ${OUTSURF}/${GEOM1}_absz.obj --obj

	mv -v ${OUTSURF}/${GEOM1}-${GEOM1}_absz.obj ${OUTSURF}/porto.obj
	mv -v ${OUTSURF}/${GEOM1}-${GEOM1}_absz.json ${OUTSURF}/porto.json

	muse_geometry -M -p ${WP} -m ${OUTSURF}/porto.obj --tet --opt Yq --vtk
	   
       ##########  DATA  ###########
       muse_data -N -p ${WP}
	cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
	muse_data -S -p ${WP} --setX 2 --setY 3 --setZ 14
	muse_data -C -p ${WP}
    
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
   
	#geometry:
	##########  GEOMETRY  ###########
	muse_geometry -N -p ${WP}
	cp -R ${DATA_SOURCE}/${GEOM1} ${INGEOM}
	muse_geometry -V -p ${WP} --tri --opt ${OPT} --obj
	muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM1}.obj --abs -z -5 --obj
	muse_geometry -T -p ${WP} -m ${OUTSURF}/${GEOM1}.obj -m ${OUTSURF}/${GEOM1}_absz.obj --obj

	mv -v ${OUTSURF}/${GEOM1}-${GEOM1}_absz.obj ${OUTSURF}/porto.obj
	mv -v ${OUTSURF}/${GEOM1}-${GEOM1}_absz.json ${OUTSURF}/porto.json

	muse_geometry -M -p ${WP} -m ${OUTSURF}/porto.obj --tet --opt Yq --vtk
	
	    ##########  DATA  ###########
	muse_data -N -p ${WP}
	cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
	muse_data -S -p ${WP} --setX 2 --setY 3 --setZ 14
	muse_data -C -p ${WP}
fi



#manip:
##########  MANIPULATE  ###########
muse_manipulate -E -p ${WP} --geom ${OUTSURF}/porto.obj


#vario:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR} --nscore YES --vario MODEL --eps 5.0


#compute:
##########  COMPUTE  ###########
export GMOD=porto
##export OUTCOM=${OUTWP}/compute/${VAR}_${DIR}${DIM}_${GMOD}

if [[ $OUTSGS == 'MEAN'* ]]; then
  muse_compute -C -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr ${MINEX} --maxextr ${MAXEX}
else
  muse_compute -C -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --nsim ${NSIM} --out ${OUTSGS}
  muse_compute -S -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --csv

  #back:
  muse_compute -B -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --extr Extr --minextr ${MINEX} --maxextr ${MAXEX} --csv

  #statsback:
  muse_compute -S -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --space VAR --csv
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
