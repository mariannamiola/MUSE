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
#set -x #print commands

######################################################
################ PROJECT NAME ########################

export setPROJECT_NAME=14_sec_3D_4000_tet_sGs_sis

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
	export DATA=amga_merged.csv
fi


# 1. Export variables
#######################################################################
export GEOM=sec ##boundary points defining the section parallel to x-z plane

export VAR0=cat ##categorical variable
export VAR1=phi ##continuous variable
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=qa2
export OPT_TET=q
export RESX=0.5
export RESY=0.5
export RESZ=1.0

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
cp -R ${DATA_SOURCE}/${GEOM}.xyz ${INGEOM}
muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GEOM}.xyz --opt ${OPT} --setz -1.0 --obj --rotaxis X --rotangle 270

muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM}.obj --abs -z 0.0 --obj
mv ${OUTSURF}/${GEOM}_absz.obj ${OUTSURF}/${GEOM}_absz-0.0.obj

muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM}.obj --abs -z 10.0 --obj
mv ${OUTSURF}/${GEOM}_absz.obj ${OUTSURF}/${GEOM}_absz-10.0.obj

muse_geometry -T -p ${WP} -m ${OUTSURF}/${GEOM}_absz-0.0.obj -m ${OUTSURF}/${GEOM}_absz-10.0.obj --obj
mv ${OUTSURF}/${GEOM}_absz-0.0-${GEOM}_absz-10.0.obj ${OUTSURF}/${GEOM}_box.obj

muse_geometry -M -p ${WP} -m ${OUTSURF}/${GEOM}_box.obj --tet --vtk --opt ${OPT_TET}
muse_geometry -Z -p ${WP} -m ${OUTVOL}/${GEOM}_box.vtk --rotaxis X --rotangle -270 --vtk ##only for visualization

####### computing mesh with wells ...
#wells:
muse_geometry -W -p ${WP} -m ${OUTSURF}/${GEOM}_box.obj -o test_box.off -w "50,-5,0,4,0.5" -w "20,-5,0,4,0.5"  -w "75,-5,0,4,0.5" -v --tet --vtk
##-m ${OUTSURF}/${GEOM}_box.obj -o ${GEOM}_box_tmp.off -w "0,0,-2,4,0.5" -v --tet ##--opt ... to complete with correct coordinate of wells in tetrahedral section model
###string for testing: #--generate-box "10,5,8" -o test_box.off -w "0,0,-2,4,0.5" -v --generate-tet

#### clean up geometry files from script folder and move to output geometry folder (volume/tmp)
#mkdir -p ${OUTVOL}/tmp
#cp ${SCRIPT_DIR}/black_faces.off ${OUTVOL}/tmp/black_faces.off
#cp ${SCRIPT_DIR}/box_mesh.off ${OUTVOL}/tmp/box_mesh.off
#cp ${SCRIPT_DIR}/cylinder_1.off ${OUTVOL}/tmp/cylinder_1.off
#cp ${SCRIPT_DIR}/cylinders.off ${OUTVOL}/tmp/cylinders.off
#rm ${SCRIPT_DIR}/black_faces.off ${SCRIPT_DIR}/box_mesh.off ${SCRIPT_DIR}/cylinder_1.off ${SCRIPT_DIR}/cylinders.off

############################## INDICATOR VARIOGRAM AND SIS COMPUTATION ##############################

#variosis:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR0} --vario MODEL --dir ${DIR} --dim ${DIM} --dirs 0,70,80,90,110 --degtol 15 --vclean 10 --rotaxis X --rotangle 270 --itype SPHERICAL!6 --itype SPHERICAL!8 --inugget 0!1 --inugget 0!2 --inugget 0!3 --inugget 0!4 --inugget 0!5 --inugget 0!6 --inugget 0!7 --inugget 0!8

#computesis:
##########  COMPUTE  ###########
export GMOD=${GEOM}_box

## decomment if you want to simulate with wells
GMOD=test_box
muse_compute -C -p ${WP} -v ${VAR0} --dir ${DIR} --dim ${DIM} -m ${OUTVOL}/${GMOD}.vtk --crit SISIM --nsim ${NSIM} --rotaxis X --rotangle 270 --scaleradius 1.5 --octant --simulated 7 --input 3


################################
export OUTCOMP=${OUTWP}/compute/${VAR0}_${DIR}${DIM}_${GMOD}
export PDF_NAME=pdf_cat_
export NCAT=8
for ((i=1; i<=${NCAT}; i++))
do
    cp ${SCRIPT_DIR}/${PDF_NAME}${i}.txt ${OUTCOMP}/${VAR0}_${i}_pdf.csv
    rm ${SCRIPT_DIR}/${PDF_NAME}${i}.txt
done 


############################## (CONTINUOUS) VARIOGRAM AND SGS COMPUTATION ##############################

#variosgs:
muse_vario -V -p ${WP} -v ${VAR1} --rotaxis X --rotangle 270 --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --dirs 0,70,80,90,110 --degtol 15 --vclean 10 --eps 5.0 --nugget 0.18

#computesgs:
muse_compute -C -p ${WP} -v ${VAR1} --rotaxis X --rotangle 270 -m ${OUTVOL}/${GMOD}.vtk --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS} --scaleradius 1.4 --octant --simulated 5 --input 10
#### stats for normal score values
muse_compute -S -p ${WP} -v ${VAR1} -m ${OUTVOL}/${GMOD}.vtk --dir ${DIR} --dim ${DIM} --csv

#back:
muse_compute -B -p ${WP} -v ${VAR1} -m ${OUTVOL}/${GMOD}.vtk --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0.0 --maxextr 1.0 --csv

#### stats for values in original space
muse_compute -S -p ${WP} -v ${VAR1} -m ${OUTVOL}/${GMOD}.vtk --dir ${DIR} --dim ${DIM} --space VAR --csv


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
