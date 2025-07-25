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
echo $PATH

#######################################################################

export MUSE=${SCRIPT_DIR}/../../..

if [ "$OPTDATASOURCE" ]	#if a input data folder is provided
then
	export DATA_SOURCE=${OPTDATASOURCE}
else
	export DATA_SOURCE=${MUSE}/examples/19_Polcevera_tet_cutmesh/data
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
	export PROJ=19_Polcevera_tet_cutmesh
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=dataset/log_samples.csv ##log_samples.csv
fi


# 1. Export variables
#######################################################################
#export GEOM2D=all_smpl_2
export VEC3D=polcevera_dense ##test ##polcevera_dense
export DEM=piana ##subDEM ##piana
export MESH=tetmesh4536911 ##tetmesh

export VAR=cat
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export DELTA=-50 ##metri
export OPTVOL=Y

#For vario
export DIR=DIR
export DIM=3Dxy

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
##########  PROJECT  ###########
muse_project -N -p ${WORK} --name ${PROJ}
    
##########  DATA  ###########
muse_data -N -p ${WP}
cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
muse_data -S -p ${WP} --setX 1 --setY 2 --setZ 3
muse_data -C -p ${WP}

##########  GEOMETRY  ###########
export GEOMNAME=${DEM}-${DEM}_dz

muse_geometry -N -p ${WP}
mkdir -p ${OUTSURF}
mkdir -p ${OUTVOL}

cp -R ${DATA_SOURCE}/${MESH}.vtk ${OUTVOL}
mv ${OUTVOL}/${MESH}.vtk ${OUTVOL}/${GEOMNAME}.vtk 
muse_geometry -Z -p ${WP} -m ${OUTVOL}/${GEOMNAME}.vtk --surf --obj

#manip:
##########  MANIPULATE  ###########
export GEOMNAME=${DEM}-${DEM}_dz

##verificare i punti all'interno del guscio esterno della mesh volumetrica
muse_manipulate -E -p ${WP} --geom ${OUTSURF}/${GEOMNAME}.obj 

##calcolo proiezioni dei campioni su guscio esterno mesh volumetrica/ oppure passo top e bottom separati
muse_manipulate -V -p ${WP} --type SAMPLES --sub ${GEOMNAME} --mgeom ${OUTSURF}/${GEOMNAME}.obj --prdir Z

##calcolo proiezioni dei vertici della mesh passando top e bottom separati/ oppure caso semplice: solo top
muse_manipulate -V -p ${WP} --type TET --reggrow --geom ${OUTVOL}/${GEOMNAME}.vtk --prdir Z
#muse_manipulate -V -p ${WP} --type HEX --geom ${OUTVOL}/${GEOMNAME}.vtk --prdir Z
#muse_manipulate -V -p ${WP} --type TET --geom ${OUTVOL}/${GEOMNAME}.vtk --mgeom ${OUTSURF}/${DEM}.obj --mgeom ${OUTSURF}/${DEM}_dz.obj --prdir Z --step 3 ##il numero di step corrisponde ai nodi di discretizzazione della superficie laterali (esclusi top e bottom)

#strat:
muse_manipulate -T -p ${WP} --sttype PROPORTIONAL --type SAMPLES --sub ${GEOMNAME} --top ${GEOMNAME}_top --bot ${GEOMNAME}_bot --name ${GEOMNAME} --prdir Z
muse_manipulate -T -p ${WP} --sttype PROPORTIONAL --type VOLUME --top ${GEOMNAME}_top --bot ${GEOMNAME}_bot --geom ${OUTVOL}/${GEOMNAME}.vtk --vtk --name ${GEOMNAME} --prdir Z


#vario:
##########  VARIO  ###########
export GEOMNAME=${DEM}-${DEM}_dz

muse_vario -V -p ${WP} -v ${VAR} --sttype PROPORTIONAL -f samples_${GEOMNAME} --sub ${GEOMNAME} --vario MODEL --dir ${DIR} --dim ${DIM} --eps 5.0 --lagspac CONSTANT --zdegtol 5 --itype SPHERICAL!4 --inugget 0.04!4 --itype SPHERICAL!5 --inugget 0.06!5 --itype SPHERICAL!6 --inugget 0.12!6 --imaxdist 2000!5 ##--inugget 0.07!4
muse_vario -V -p ${WP} -v ${VAR} --sttype PROPORTIONAL -f samples_${GEOMNAME} --sub ${GEOMNAME} --vario MODEL --dir ${DIR} --dim 3Dz --lagspac CONSTANT --zdegtol 5 --eps 5.0

#compute:
##########  COMPUTE  ###########
export GEOMNAME=${DEM}-${DEM}_dz

muse_compute -C -p ${WP} -v ${VAR} --dir ${DIR} --dim ${DIM} -m ${OUTMAN}/geom_${GEOMNAME}.vtk --crit SISIM --nsim ${NSIM} --sub ${GEOMNAME} --zrange 20 ### --scaleradius 1.5 --input 10 --simulated 6

#copy:
################################
export GEOMNAME=${DEM}-${DEM}_dz
export OUTCOMP=${OUTWP}/compute/${VAR}_${GEOMNAME}_${DIR}${DIM}_geom_${GEOMNAME}
export PDF_NAME=pdf_cat_
export NCAT=(1 3 4 5 6)
for i in "${NCAT[@]}"
do
    cp ${SCRIPT_DIR}/${PDF_NAME}${i}.txt ${OUTCOMP}/${PDF_NAME}${i}.csv
    rm ${SCRIPT_DIR}/${PDF_NAME}${i}.txt
done 
 
#clean:

#plot:
##########  PLOT  ###########
#-H -p ${WP} -v <FILE>

####################################################################### MUSE END
)

