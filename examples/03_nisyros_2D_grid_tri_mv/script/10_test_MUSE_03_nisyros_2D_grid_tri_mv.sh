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
	export DATA_SOURCE=${MUSE}/examples/03_nisyros_2D_grid_tri_mv/data
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
	export PROJ=03_nisyros_2D_grid_tri_mv
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=nisyros.csv
fi


# 1. Export variables
#######################################################################
export GEOM1=nis.gpkg
export GEOM2=nis.xyz

export VAR0=T
export VAR1=CO2
export VAR2=lCO2
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=a100

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
### check se esiste la cartella di progetto
if [ "$OPTREPL" ]	#if a number of simulations is provided
then
    export RMPROJ=$OPTREPL
else
    export RMPROJ=true ###Flag to REPLACE DATA
fi


########## REPORT  ###########

echo "
    	Running: 10_test_MUSE 
    			[ -d | --data 	    ] $DATA
    			[ -D | --datasource ] ${DATA_SOURCE}
    			[ -p | --proj       ] $PROJ
    			[ -r | --repl       ] $REPL
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
        muse_data -S -p ${WP}  --setX 1 --setY 2 --setZ 3
        muse_data -C -p ${WP}
    
        ##########  GEOMETRY  ###########
        muse_geometry -N -p ${WP}
        cp -R ${DATA_SOURCE}/${GEOM1} ${INGEOM}
        muse_geometry -V -p ${WP} --tri --opt ${OPT} --obj
        mv -v ${OUTSURF}/nis.obj ${OUTSURF}/nis_tri.obj
        mv -v ${OUTSURF}/nis.json ${OUTSURF}/nis_tri.json

        muse_geometry -V -p ${WP} --grid --resx 10 --resy 10 --obj
        mv -v ${OUTSURF}/nis.obj ${OUTSURF}/nis_grid.obj
        mv -v ${OUTSURF}/nis.json ${OUTSURF}/nis_grid.json
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
    muse_data -S -p ${WP}  --setX 1 --setY 2 --setZ 3
    muse_data -C -p ${WP}
    
    ##########  GEOMETRY  ###########
    muse_geometry -N -p ${WP}
    cp -R ${DATA_SOURCE}/${GEOM1} ${INGEOM}
    muse_geometry -V -p ${WP} --tri --opt ${OPT} --obj
    mv -v ${OUTSURF}/nis.obj ${OUTSURF}/nis_tri.obj
    mv -v ${OUTSURF}/nis.json ${OUTSURF}/nis_tri.json

    muse_geometry -V -p ${WP} --grid --resx 10 --resy 10 --obj
    mv -v ${OUTSURF}/nis.obj ${OUTSURF}/nis_grid.obj
    mv -v ${OUTSURF}/nis.json ${OUTSURF}/nis_grid.json
fi




#muse_geometry -O -p ${WP} -m ${OUTSURF}/nis.obj --abs -z 100 --obj
#muse_geometry -T -p ${WP} -m ${OUTSURF}/nis.obj -m ${OUTSURF}/nisabsz100.obj --obj
#muse_geometry -M -p ${WP} -m ${OUTSURF}/nis-nisabsz100.obj --tet --vtk

###routine for gridding and voxel or hexmesh
#cp -R ${DATA_SOURCE}/${GEOM2} ${INGEOM}
#muse_geometry -G -p ${WP} -m ${INGEOM}/nis.xyz --boundary ${INGEOM}/nis.xyz --resx 100 --resy 50 --obj
#muse_geometry -O -p ${WP} -m ${OUTSURF}/grid.obj --abs -z 100 --obj
#muse_geometry -A -m ${OUTSURF}/grid.off -m ${OUTSURF}/gridabsz100.obj --obj
#muse_geometry -M -p ${WP} -m ${OUTSURF}/grid_append.off --vox --nmaxvox 5 --vtk
#muse_geometry -M -p ${WP} -m ${OUTSURF}/grid_append.obj --hex --resx 100 --resy 50 --resz 50 --vtk

###new commands for geometry
#cp -R ${DATA_SOURCE}/${GEOM1} ${INGEOM}
#muse_geometry -O -p ${WP} -m ${OUTSURF}/nis.obj --abs -z 100 --obj
#muse_geometry -Q -p ${WP} -m ${OUTSURF}/nis.obj -m ${OUTSURF}/nis_absz.obj --obj ##for lateral closure meshes by quads
#muse_geometry -M -p ${WP} -m ${OUTSURF}/nis-nis_absz.obj --hex --resx 100 --resy 50 --resz 50 --vtk



#vario:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --deg 22.5 --degtol 22.5 --vclean 10 --eps 5.0 --type SPHERICAL --nugget 0.2
muse_vario -V -p ${WP} -v ${VAR1} --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --deg 22.5 --degtol 22.5 --vclean 10 --eps 5.0 --type SPHERICAL --nugget 0.2
muse_vario -V -p ${WP} -v ${VAR2} --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --deg 22.5 --degtol 22.5 --vclean 10 --eps 5.0 --type SPHERICAL --nugget 0.2


#compute:
declare -a arr=("nis_tri" "nis_grid")

for i in "${arr[@]}"
do  
  if [[ $OUTSGS == 'MEAN'* ]]; then
    ##########  COMPUTE on tri  ###########
    echo "Computing stochastic simulations on " $i " mesh ... "
    muse_compute -C -p ${WP} -v ${VAR0} -m ${OUTSURF}/${i}.obj --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 10 --maxextr 100 --dir ${DIR} --dim ${DIM}
    muse_compute -C -p ${WP} -v ${VAR1} -m ${OUTSURF}/${i}.obj --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 0 --maxextr 6500 --dir ${DIR} --dim ${DIM}
    muse_compute -C -p ${WP} -v ${VAR2} -m ${OUTSURF}/${i}.obj --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr -1 --maxextr 4 --dir ${DIR} --dim ${DIM}
  else
    echo "Computing stochastic simulations on " $i " mesh ... "
    ##########  COMPUTE on tri  ###########
    muse_compute -C -p ${WP} -v ${VAR0} -m ${OUTSURF}/${i}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
    muse_compute -C -p ${WP} -v ${VAR1} -m ${OUTSURF}/${i}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
    muse_compute -C -p ${WP} -v ${VAR2} -m ${OUTSURF}/${i}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
  
    #stats:
    muse_compute -S -p ${WP} -v ${VAR0} -m ${OUTSURF}/${i}.obj --dir ${DIR} --dim ${DIM} --csv
    muse_compute -S -p ${WP} -v ${VAR1} -m ${OUTSURF}/${i}.obj --dir ${DIR} --dim ${DIM} --csv
    muse_compute -S -p ${WP} -v ${VAR2} -m ${OUTSURF}/${i}.obj --dir ${DIR} --dim ${DIM} --csv
  
    #back: 
    muse_compute -B -p ${WP} -v ${VAR0} -m ${OUTSURF}/${i}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 10 --maxextr 100 --csv
    muse_compute -B -p ${WP} -v ${VAR1} -m ${OUTSURF}/${i}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 6500 --csv
    muse_compute -B -p ${WP} -v ${VAR2} -m ${OUTSURF}/${i}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr -1 --maxextr 4 --csv
    
    #statsback:
    muse_compute -S -p ${WP} -v ${VAR0} -m ${OUTSURF}/${i}.obj --dir ${DIR} --dim ${DIM} --space VAR --csv
    muse_compute -S -p ${WP} -v ${VAR1} -m ${OUTSURF}/${i}.obj --dir ${DIR} --dim ${DIM} --space VAR --csv
    muse_compute -S -p ${WP} -v ${VAR2} -m ${OUTSURF}/${i}.obj --dir ${DIR} --dim ${DIM} --space VAR --csv
  fi
  
  #db:
  ##########  DATABASE  ###########
  if [[ $OUTSGS == 'MEAN'* ]]; then
    echo "Dataset creation for MEAN SGS method ... NOT IMPLEMENTED!"
  else
    muse_compute -D -p ${WP} -v ${VAR0} -m ${OUTSURF}/${i}.obj --dir ${DIR} --dim ${DIM} --space VAR
    muse_compute -D -p ${WP} -v ${VAR1} -m ${OUTSURF}/${i}.obj --dir ${DIR} --dim ${DIM} --space VAR
    muse_compute -D -p ${WP} -v ${VAR2} -m ${OUTSURF}/${i}.obj --dir ${DIR} --dim ${DIM} --space VAR
  fi
  
done


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
