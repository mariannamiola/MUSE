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
echo $PATH

#######################################################################

export MUSE=${SCRIPT_DIR}/../../..

if [ "$OPTDATASOURCE" ]	#if a input data folder is provided
then
	export DATA_SOURCE=${OPTDATASOURCE}
else
	export DATA_SOURCE=${MUSE}/examples/10_nisyros_2D_mr_tri_mv/data
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
	export PROJ=10_nisyros_2D_mr_tri_mv
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
export LOWRES=a10000
export MIDRES=a1000
export HIGHRES=a100

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
muse_project -N -p ${WORK} --name ${PROJ}

##########  DATA  ###########
muse_data -N -p ${WP}
cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
muse_data -S -p ${WP}  --setX 1 --setY 2 --setZ 3
muse_data -C -p ${WP}

##########  GEOMETRY  ###########
#geometry:
#set +e	#do not exit if an error occours
muse_geometry -N -p ${WP}

###routine for triangulation and tets
cp -R ${DATA_SOURCE}/${GEOM2} ${INGEOM}

#muse_geometry -V -p ${WP} --tri --opt ${LOWRES} --obj
muse_geometry -P -p ${WP} --tri --polygon ${INGEOM}/${GEOM2} --opt ${LOWRES} --obj
mv -v ${OUTSURF}/nis.obj ${OUTSURF}/nis_coarse.obj

muse_geometry -L -p ${WP} -m ${OUTSURF}/nis_coarse.obj --splmet EDGE --obj
#muse_geometry -V -p ${WP} --tri --opt ${MIDRES} --obj
mv -v ${OUTSURF}/nis_coarse_res.obj ${OUTSURF}/nis_mid.obj
mv -v ${OUTSURF}/nis_coarse_res.json ${OUTSURF}/nis_mid.json

muse_geometry -L -p ${WP} -m ${OUTSURF}/nis_mid.obj --splmet EDGE --obj
#muse_geometry -V -p ${WP} --tri --opt ${HIGHRES} --obj
mv -v ${OUTSURF}/nis_mid_res.obj ${OUTSURF}/nis_fine.obj
mv -v ${OUTSURF}/nis_mid_res.json ${OUTSURF}/nis_fine.json

#muse_geometry -V -p ${WP} --grid --resx 10 --resy 10 --obj

#set -e	#exit if an error occours


#vario:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --deg 22.5 --degtol 22.5 --vclean 10 --eps 5.0 --type SPHERICAL --nugget 0.25
muse_vario -V -p ${WP} -v ${VAR1} --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --deg 22.5 --degtol 22.5 --vclean 10 --eps 5.0 --type SPHERICAL --nugget 0.25
muse_vario -V -p ${WP} -v ${VAR2} --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --deg 22.5 --degtol 22.5 --vclean 10 --eps 5.0 --type SPHERICAL --nugget 0.25


#compute:
export GMOD=nis_fine
export OUTCOM0=${OUTWP}/compute/${VAR0}_${DIR}${DIM}_${GMOD}
export OUTCOM1=${OUTWP}/compute/${VAR1}_${DIR}${DIM}_${GMOD}
export OUTCOM2=${OUTWP}/compute/${VAR2}_${DIR}${DIM}_${GMOD}

export OUTNORMS0=${OUTCOM0}/_normspace
export OUTNORMS1=${OUTCOM1}/_normspace
export OUTNORMS2=${OUTCOM2}/_normspace


  if [[ $OUTSGS == 'MEAN'* ]]; then
    ##########  COMPUTE on tri  ###########
    echo "Computing stochastic simulations on " $i " mesh ... "
    muse_compute -C -p ${WP} -v ${VAR0} -m ${OUTSURF}/${GMOD}.obj --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 10 --maxextr 100 --dir ${DIR} --dim ${DIM}
    muse_compute -C -p ${WP} -v ${VAR1} -m ${OUTSURF}/${GMOD}.obj --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 0 --maxextr 6500 --dir ${DIR} --dim ${DIM}
    muse_compute -C -p ${WP} -v ${VAR2} -m ${OUTSURF}/${GMOD}.obj --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr -1 --maxextr 4 --dir ${DIR} --dim ${DIM}
  
  else
    echo "Computing stochastic simulations on " $i " mesh ... "
    ##########  COMPUTE on tri  ###########
    muse_compute -C -p ${WP} -v ${VAR0} -m ${OUTSURF}/${GMOD}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
    muse_compute -C -p ${WP} -v ${VAR1} -m ${OUTSURF}/${GMOD}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
    muse_compute -C -p ${WP} -v ${VAR2} -m ${OUTSURF}/${GMOD}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
  
    #stats:
    muse_compute -S -p ${WP} -v ${VAR0} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --csv
    muse_compute -S -p ${WP} -v ${VAR1} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --csv
    muse_compute -S -p ${WP} -v ${VAR2} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --csv
  
    #back:
    muse_compute -B -p ${WP} -v ${VAR0} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 10 --maxextr 100 --csv
    muse_compute -B -p ${WP} -v ${VAR1} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 6500 --csv
    muse_compute -B -p ${WP} -v ${VAR2} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr -1 --maxextr 4 --csv
    #for (( id=0; id<=$NSIM-1; id++ ))
    #do
    #  muse_compute -B -p ${WP} -v ${VAR0} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 10 --maxextr 100 -f ${OUTNORMS0}/${VAR}_sgs_$id.csv --csv
    #  muse_compute -B -p ${WP} -v ${VAR1} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 6500 -f ${OUTNORMS1}/${VAR}_sgs_$id.csv --csv
    #  muse_compute -B -p ${WP} -v ${VAR2} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr -1 --maxextr 4 -f ${OUTNORMS2}/${VAR}_sgs_$id.csv --csv
    #done
  
    #statsback:
    muse_compute -S -p ${WP} -v ${VAR0} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --space VAR --csv
    muse_compute -S -p ${WP} -v ${VAR1} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --space VAR --csv
    muse_compute -S -p ${WP} -v ${VAR2} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --space VAR --csv
  fi
  
  #db:
  ##########  DATABASE  ###########
  if [[ $OUTSGS == 'MEAN'* ]]; then
    echo "Dataset creation for MEAN SGS method ... NOT IMPLEMENTED!"
  else
    muse_compute -D -p ${WP} -v ${VAR0} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --space VAR
    muse_compute -D -p ${WP} -v ${VAR1} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --space VAR
    muse_compute -D -p ${WP} -v ${VAR2} -m ${OUTSURF}/${GMOD}.obj --dir ${DIR} --dim ${DIM} --space VAR
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



#multires:
export GMOD=nis_fine
export OUTCOM0=${OUTWP}/compute/${VAR0}_${DIR}${DIM}_${GMOD}
export OUTCOM1=${OUTWP}/compute/${VAR1}_${DIR}${DIM}_${GMOD}
export OUTCOM2=${OUTWP}/compute/${VAR2}_${DIR}${DIM}_${GMOD}


export OUTVARS0=${OUTCOM0}/_varspace
export OUTVARS1=${OUTCOM1}/_varspace
export OUTVARS2=${OUTCOM2}/_varspace

muse_geometry -D -p ${WP} --refmod ${OUTSURF}/nis_fine.obj -f ${OUTVARS0}/_stats/${VAR0}_sgs_mean.csv --mesh ${OUTSURF}/nis_coarse.obj
muse_geometry -D -p ${WP} --refmod ${OUTSURF}/nis_fine.obj -f ${OUTVARS1}/_stats/${VAR1}_sgs_mean.csv --mesh ${OUTSURF}/nis_coarse.obj
muse_geometry -D -p ${WP} --refmod ${OUTSURF}/nis_fine.obj -f ${OUTVARS2}/_stats/${VAR2}_sgs_mean.csv --mesh ${OUTSURF}/nis_coarse.obj

#muse_compute -R -p ${WP} -m ${OUTSURF}/nis_fine.obj -f ${OUTVARS0}/_stats/${VAR0}_mean.csv --mesh ${OUTSURF}/nis_coarse.obj
#muse_compute -R -p ${WP} -m ${OUTSURF}/nis_fine.obj -f ${OUTVARS1}/_stats/${VAR1}_mean.csv --mesh ${OUTSURF}/nis_coarse.obj
#muse_compute -R -p ${WP} -m ${OUTSURF}/nis_fine.obj -f ${OUTVARS2}/_stats/${VAR2}_mean.csv --mesh ${OUTSURF}/nis_coarse.obj



#plot:
##########  PLOT  ###########
#-H -p ${WP} -v <FILE>

####################################################################### MUSE END
)
