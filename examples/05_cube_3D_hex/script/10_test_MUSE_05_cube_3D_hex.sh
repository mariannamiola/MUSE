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
	export DATA_SOURCE=${MUSE}/examples/05_cube_3D_hex/data
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
	export PROJ=05_cube_3D_hex
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=cube.csv
fi


# 1. Export variables
#######################################################################
export GEOM1=cube.xyz

export VAR=phi
#######################################################################


# 2. Export flags
#######################################################################

#For vario
export DIR=DIR
export DIM=3Dxy

#For compute
if [ "$OPTSIM" ]	#if a number of simulations is provided
then
	export NSIM=$OPTSIM
else
	export NSIM=100
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
        muse_data -S -p ${WP}  --setX 1 --setY 2 --setZ 3
        muse_data -C -p ${WP}
        
        ##########  GEOMETRY  ###########
        muse_geometry -N -p ${WP}
        cp -R ${DATA_SOURCE}/${GEOM1} ${INGEOM}
        muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/cube.xyz --obj
        muse_geometry -O -p ${WP} -m ${OUTSURF}/cube.obj --abs -z -32 --obj
        muse_geometry -T -p ${WP} -m ${OUTSURF}/cube.obj -m ${OUTSURF}/cube_absz.obj --obj
        muse_geometry -M -p ${WP} -m ${OUTSURF}/cube-cube_absz.obj --hex --resx 1 --resy 1 --resz 1 --vtk
        mv ${OUTVOL}/cube-cube_absz.vtk ${OUTVOL}/cube.vtk
        mv ${OUTVOL}/cube-cube_absz.json ${OUTVOL}/cube.json
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
    muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/cube.xyz --obj
    muse_geometry -O -p ${WP} -m ${OUTSURF}/cube.obj --abs -z -32 --obj
    muse_geometry -T -p ${WP} -m ${OUTSURF}/cube.obj -m ${OUTSURF}/cube_absz.obj --obj
    muse_geometry -M -p ${WP} -m ${OUTSURF}/cube-cube_absz.obj --hex --resx 1 --resy 1 --resz 1 --vtk
    mv ${OUTVOL}/cube-cube_absz.vtk ${OUTVOL}/cube.vtk
    mv ${OUTVOL}/cube-cube_absz.json ${OUTVOL}/cube.json
fi


###routine for gridding and voxel or hexmesh
#set +e	#do not exit if an error occours
#muse_geometry -G -p ${WP} -m ${INGEOM}/cube.xyz --boundary ${INGEOM}/cube.xyz --resx 1 --resy 1 --obj
#muse_geometry -A -m ${OUTSURF}/grid.obj -m ${OUTSURF}/gridabsz-32.obj --obj
#muse_geometry -M -p ${WP} -m ${OUTSURF}/grid_append.off --vox --nmaxvox 5 --vtk
#set -e	#exit if an error occours


#vario:
##########  VARIO  ###########
#muse_vario -V -p ${WP} -v ${VAR} --nscore YES --vario MODEL --eps 5.0
#muse_vario -V -p ${WP} -v ${VAR} --nscore YES --dir DIR --dim 2D --vario MODEL --deg 22.5 --degtol 22.5 --eps 5.0
muse_vario -V -p ${WP} -v ${VAR} --nscore YES --vario MODEL --eps 5.0 --dim ${DIM} --dir ${DIR} --maxdist 70 --dirs 0,45,67.5,90 --degtol 45 --zdegtol 5 --epsy 0.2
muse_vario -V -p ${WP} -v ${VAR} --nscore YES --dir ${DIR} --dim 3Dz --lagspac CONSTANT --zdegtol 5 --vario MODEL --eps 5.0



#compute:
##########  COMPUTE  ###########
export GMOD=cube
#export OUTCOM=${OUTWP}/compute/${VAR}_${DIR}${DIM}_${GMOD}

#export OUTNORMS=${OUTCOM}/_normspace
#export OUTVARS=${OUTCOM}/_varspace

if [[ $OUTSGS == 'MEAN'* ]]; then
  muse_compute -C -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 0 --maxextr 1 --dir ${DIR} --dim ${DIM} --zrange 7.9
else
  muse_compute -C -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --nsim ${NSIM} --out ${OUTSGS} --dir ${DIR} --dim ${DIM} --zrange 7.9

  muse_compute -S -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --csv --dir ${DIR} --dim ${DIM}

  #back:
  muse_compute -B -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --extr Extr --minextr 0 --maxextr 1 --csv --dir ${DIR} --dim ${DIM}

  #statsback:
  muse_compute -S -p ${WP} -v ${VAR} -m ${OUTVOL}/${GMOD}.vtk --space VAR --csv --dir ${DIR} --dim ${DIM}
fi


#clean:
#echo ${SCRIPT_DIR}
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
